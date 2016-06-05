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
#pragma once

#include "common/PacketBuilder.hpp"
#include "common/Types.hpp"
#include <string>

namespace vana {
	class item;

	namespace channel_server {
		class player;

		namespace packets {
			namespace trades {
				namespace messages {
					enum deny_trade_messages : int8_t {
						unable_to_find = 0x01,
						doing_something_else = 0x02,
						rejected = 0x03,
						trades_disabled = 0x04
					};
					enum shop_entry_messages : int8_t {
						no_trades_while_dead = 0x00, // This message uses the overload that takes 2 Player *s
						room_already_closed = 0x01, // ALL the others use the overload with 1
						room_already_full = 0x02,
						handling_other_requests = 0x03,
						no_trades_while_dead2 = 0x04,
						no_trades_during_events = 0x05,
						unable_to_trade = 0x06,
						unable_to_trade_other_items = 0x07, // No clue...
						requires_same_map = 0x09,
						no_stores_near_portal = 0x0A,
						no_minigame_room = 0x0B,
						no_game = 0x0C,
						no_shop = 0x0D,
						cut_off_message = 0x0E, // 'oom 7~22, located at the 2nd floor and above of th'
						BannedFromShop = 0x0F,
						undergoing_maintenance = 0x10,
						no_tournament_room = 0x11,
						unable_to_trade_other_items2 = 0x12, // No clue...
						not_enough_mesos = 0x13,
						incorrect_password = 0x14
					};
					enum end_trade_messages : int8_t {
						success = 0x06,
						failure = 0x07,
						trade_has_limited_items = 0x08, // Can't carry more than 1
						player_is_on_different_map = 0x09
					};
				}
				namespace message_types {
					enum types : int8_t {
						deny_trade = 0x03,
						shop_entry_messages = 0x05,
						end_trade = 0x0A
					};
				}

				PACKET(send_open_trade, ref_ptr<player> player1, ref_ptr<player> player2);
				PACKET(send_trade_request, const string &name, game_trade_id trade_id);
				PACKET(send_trade_message, const string &name, int8_t type, int8_t message);
				PACKET(send_end_trade, int8_t message);
				PACKET(send_trade_entry_message, int8_t message);
				PACKET(send_trade_chat, bool blue, const string &chat);
				PACKET(send_leave_trade);
				PACKET(send_add_user, ref_ptr<player> new_player, int8_t slot);
				PACKET(send_add_mesos, uint8_t slot, game_mesos amount);
				PACKET(send_accepted);
				PACKET(send_add_item, uint8_t player, uint8_t slot, item *item);
			}
		}
	}
}