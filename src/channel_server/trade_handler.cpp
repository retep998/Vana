/*
Copyright (C) 2008-2016 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "trade_handler.hpp"
#include "common/data/provider/item.hpp"
#include "common/maple_version.hpp"
#include "common/packet_reader.hpp"
#include "common/timer/thread.hpp"
#include "common/timer/timer.hpp"
#include "common/util/game_logic/item.hpp"
#include "common/util/time.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/trade.hpp"
#include "channel_server/trades.hpp"
#include "channel_server/trades_packet.hpp"
#include <functional>
#include <string>

namespace vana {
namespace channel_server {

namespace trade_opcodes {
	enum opcodes : int8_t {
		open_trade = 0x00,
		send_trade_request = 0x02,
		deny_request = 0x03,
		accept_request = 0x04,
		chat = 0x06,
		cancel_trade = 0x0A,
		add_item = 0x0E,
		add_mesos = 0x0F,
		accept_trade = 0x10
	};
}

namespace trade_slots {
	enum slots : int8_t {
		one = 0x00,
		two = 0x01
	};
}

auto trade_handler::trade_handler(ref_ptr<player> player, packet_reader &reader) -> void {
	int8_t sub_opcode = reader.get<int8_t>();
	switch (sub_opcode) {
		case trade_opcodes::open_trade: // Open trade - this usually comes with 03 00 - I think that implies the type of thing getting opened (trade, minigame, etc.)
			player->send(packets::trades::send_open_trade(player, nullptr));
			break;
		case trade_opcodes::send_trade_request: {
			if (player->is_trading()) {
				// Most likely hacking
				return;
			}
			game_player_id recv_id = reader.get<game_player_id>();
			auto receiver = channel_server::get_instance().get_player_data_provider().get_player(recv_id);
			if (receiver != nullptr) {
				if (!receiver->is_trading()) {
					receiver->send(packets::trades::send_trade_request(player->get_name(), channel_server::get_instance().get_trades().new_trade(player, receiver)));
				}
				else {
					player->send(packets::trades::send_trade_message(receiver->get_name(), packets::trades::message_types::deny_trade, packets::trades::messages::doing_something_else));
				}
			}
			break;
		}
		case trade_opcodes::deny_request: {
			game_trade_id trade_id = reader.get<game_trade_id>();
			active_trade *trade = channel_server::get_instance().get_trades().get_trade(trade_id);
			if (trade != nullptr) {
				auto one = trade->get_sender();
				auto two = trade->get_receiver();
				trade_handler::remove_trade(trade_id);
				one->send(packets::trades::send_trade_message(two->get_name(),packets::trades::message_types::deny_trade, reader.get<int8_t>()));
			}
			break;
		}
		case trade_opcodes::accept_request: {
			game_trade_id trade_id = reader.get<game_trade_id>();
			active_trade *trade = channel_server::get_instance().get_trades().get_trade(trade_id);
			if (trade != nullptr) {
				auto one = trade->get_sender();
				auto two = trade->get_receiver();
				two->set_trading(true);
				one->send(packets::trades::send_add_user(two, trade_slots::two));
				player->send(packets::trades::send_open_trade(two, one));
				channel_server::get_instance().get_trades().stop_timeout(trade_id);
			}
			else {
				player->send(packets::trades::send_trade_entry_message(packets::trades::messages::room_already_closed));
			}
			break;
		}
		case trade_opcodes::chat: {
			active_trade *trade = channel_server::get_instance().get_trades().get_trade(player->get_trade_id());
			if (trade == nullptr) {
				// Hacking
				return;
			}
			auto one = trade->get_sender();
			auto two = trade->get_receiver();
			bool blue = (player == two);
			string chat = player->get_name() + " : " + reader.get<string>();
			one->send(packets::trades::send_trade_chat(blue, chat));
			if (two != nullptr) {
				two->send(packets::trades::send_trade_chat(blue, chat));
			}
			break;
		}
		case trade_opcodes::cancel_trade:
			trade_handler::cancel_trade(player);
			break;
		case trade_opcodes::add_item:
		case trade_opcodes::add_mesos:
		case trade_opcodes::accept_trade: {
			game_trade_id trade_id = player->get_trade_id();
			active_trade *trade = channel_server::get_instance().get_trades().get_trade(trade_id);
			if (trade == nullptr) {
				// Most likely hacking
				return;
			}
			trade_info *send = trade->get_sender_trade();
			trade_info *recv = trade->get_receiver_trade();
			auto one = trade->get_sender();
			auto two = trade->get_receiver();
			bool is_receiver = (player == two);
			trade_info *mod = (is_receiver ? trade->get_receiver_trade() : trade->get_sender_trade());
			switch (sub_opcode) {
				case trade_opcodes::add_item: {
					game_inventory inventory = reader.get<game_inventory>();
					game_inventory_slot slot = reader.get<game_inventory_slot>();
					game_slot_qty amount = reader.get<game_slot_qty>();
					game_trade_slot add_slot = reader.get<game_trade_slot>();
					item *value = player->get_inventory()->get_item(inventory, slot);
					if (value == nullptr || trade->is_item_in_slot(mod, add_slot) || value->has_trade_block() || value->has_lock()) {
						// Hacking
						return;
					}

					auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(value->get_id());
					if ((item_info->quest || item_info->no_trade) && !(item_info->karma_scissors || value->has_karma())) {
						// Hacking
						return;
					}
					if (vana::util::game_logic::item::is_gm_equip(value->get_id())) {
						// We don't allow these to be dropped or traded
						return;
					}

					if (vana::util::game_logic::item::is_rechargeable(value->get_id())) {
						amount = value->get_amount();
					}
					else if (amount > value->get_amount() || amount < 0) {
						// Hacking
						amount = value->get_amount();
					}
					item *use = trade->add_item(player, mod, value, add_slot, slot, inventory, amount);
					one->send(packets::trades::send_add_item(is_receiver ? trade_slots::two : trade_slots::one, add_slot, use));
					two->send(packets::trades::send_add_item(is_receiver ? trade_slots::one : trade_slots::two, add_slot, use));
					break;
				}
				case trade_opcodes::add_mesos: {
					game_mesos amount = reader.get<game_mesos>();
					if (amount < 0 || player->get_inventory()->can_modify_mesos(-amount) != stack_result::full) {
						// Hacking
						return;
					}
					game_mesos mesos = trade->add_mesos(player, mod, amount);
					one->send(packets::trades::send_add_mesos(is_receiver ? trade_slots::two : trade_slots::one, mesos));
					two->send(packets::trades::send_add_mesos(is_receiver ? trade_slots::one : trade_slots::two, mesos));
					break;
				}
				case trade_opcodes::accept_trade: {
					trade->accept(mod);
					(is_receiver ? one : two)->send(packets::trades::send_accepted());
					if (trade->both_accepted()) {
						// Do trade processing
						bool fail = !trade->both_can_trade();
						if (fail) {
							trade->return_trade();
						}
						else {
							trade->swap_trade();
						}

						int8_t message = (fail ? packets::trades::messages::failure : packets::trades::messages::success);
						one->send(packets::trades::send_end_trade(message));
						two->send(packets::trades::send_end_trade(message));
						trade_handler::remove_trade(trade_id);
					}
					break;
				}
			}
			break;
		}
	}
}

auto trade_handler::cancel_trade(ref_ptr<player> player) -> void {
	game_trade_id trade_id = player->get_trade_id();
	active_trade *trade = channel_server::get_instance().get_trades().get_trade(trade_id);
	if (trade != nullptr) {
		auto one = trade->get_sender();
		auto two = trade->get_receiver();
		bool is_receiver = (player == two);
		if (is_receiver || (!is_receiver && two != nullptr && two->get_trade_id() == one->get_trade_id())) {
			// Left while in trade, give items back
			(is_receiver ? one : two)->send(packets::trades::send_leave_trade());
			trade->return_trade();
		}
		trade_handler::remove_trade(trade_id);
	}
}

auto trade_handler::remove_trade(game_trade_id id) -> void {
	channel_server::get_instance().get_trades().remove_trade(id);
}

auto trade_handler::get_tax_level(game_mesos mesos) -> int32_t {
#if MAPLE_LOCALE == MAPLE_LOCALE_GLOBAL
#	if MAPLE_VERSION >= 67
	if (mesos < 100000) {
		return 0;
	}
	if (mesos >= 100000000) {
		return 600;
	}
	if (mesos >= 25000000) {
		return 500;
	}
	if (mesos >= 10000000) {
		return 400;
	}
	if (mesos >= 5000000) {
		return 300;
	}
	if (mesos >= 1000000) {
		return 180;
	}
	return 80;
#	elif MAPLE_VERSION >= 17
	if (mesos < 50000) {
		return 0;
	}
	if (mesos >= 10000000) {
		return 400;
	}
	if (mesos >= 5000000) {
		return 300;
	}
	if (mesos >= 1000000) {
		return 200;
	}
	if (mesos >= 100000) {
		return 100;
	}
	return 50;
#	endif /* MAPLE_VERSION check for tax levels */
#else
#	error Please specify trade costs
#endif
	return 0; // There was no tax prior to .17
}

}
}