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
#include "drop_handler.hpp"
#include "common/data/provider/drop.hpp"
#include "common/data/provider/item.hpp"
#include "common/data/provider/map.hpp"
#include "common/data/provider/quest.hpp"
#include "common/data/provider/skill.hpp"
#include "common/item.hpp"
#include "common/packet_reader.hpp"
#include "common/point.hpp"
#include "common/util/game_logic/item.hpp"
#include "common/util/randomizer.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/drop.hpp"
#include "channel_server/drops_packet.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/party.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/reactor_handler.hpp"
#include "channel_server/skills.hpp"
#include <algorithm>

namespace vana {
namespace channel_server {

auto drop_handler::do_drops(game_player_id player_id, game_map_id map_id, int32_t dropping_level, int32_t dropping_id, const point &origin, bool explosive, bool ffa, int32_t taunt, bool is_steal) -> void {
	auto &channel = channel_server::get_instance();
	auto &global_drops = channel.get_drop_data_provider().get_global_drops();

	// Make a copy of the data so we can modify the object with global drops
	auto drops = channel.get_drop_data_provider().get_drops(dropping_id);

	auto player = channel.get_player_data_provider().get_player(player_id);
	game_coord drop_pos_counter = 0;
	game_party_id party_id = 0;
	point pos;

	if (player != nullptr) {
		if (party *party = player->get_party()) {
			party_id = party->get_id();
		}
	}

	auto &config = channel.get_config();
	int32_t drop_rate = config.rates.drop_rate;
	int32_t global_drop_rate = config.rates.global_drop_rate;
	int32_t meso_rate = config.rates.drop_meso;
	int32_t global_meso_rate = config.rates.global_drop_meso;
	if (config.rates.is_global_drop_consistent_with_regular_drop_rate()) {
		global_drop_rate = drop_rate;
	}
	if (config.rates.is_global_drop_meso_consistent_with_regular_drop_meso_rate()) {
		global_meso_rate = meso_rate;
	}

	if (dropping_level != 0 && global_drops.size() != 0) {
		data::type::drop_info d;
		int8_t continent = channel.get_map_data_provider().get_continent(map_id).get(0);

		if (global_drop_rate > 0) {
			for (const auto &global_drop : global_drops) {
				if (dropping_level >= global_drop.min_level && dropping_level <= global_drop.max_level) {
					if (global_drop.continent == 0 || (continent == global_drop.continent)) {
						d = data::type::drop_info{};
						d.is_global = true;
						d.chance = global_drop.chance;
						d.is_mesos = global_drop.is_mesos;
						d.item_id = global_drop.item_id;
						d.min_amount = global_drop.min_amount;
						d.max_amount = global_drop.max_amount;
						d.quest_id = global_drop.quest_id;
						drops.push_back(d);
					}
				}
			}
		}
	}

	if (config.rates.drop_rate == 0) {
		return;
	}

	vana::util::randomizer::shuffle(drops);
	game_coord mod = explosive ? 35 : 25;
	for (const auto &drop_info : drops) {
		if (drop_info.is_mesos && meso_rate == 0) {
			continue;
		}
		if (drop_info.is_global && drop_info.is_mesos && global_meso_rate == 0) {
			continue;
		}

		game_slot_qty amount = static_cast<game_slot_qty>(vana::util::randomizer::rand<int32_t>(drop_info.max_amount, drop_info.min_amount));
		drop *value = nullptr;
		uint32_t chance = drop_info.chance;

		if (is_steal) {
			chance = chance * 3 / 10;
		}
		else {
			chance = chance * taunt / 100;
			chance *= drop_info.is_global ?
				global_drop_rate :
				drop_rate;
		}

		if (vana::util::randomizer::rand<uint32_t>(999999) < chance) {
			pos.x = origin.x + ((drop_pos_counter % 2) ?
				(mod * (drop_pos_counter + 1) / 2) :
				-(mod * (drop_pos_counter / 2)));
			pos.y = origin.y;

			/*
			// get_foothold_at_position doesn't work correctly
			if (maps::get_map(map_id)->get_foothold_at_position(pos) == 0) {
				pos = maps::get_map(map_id)->find_floor(pos);
			}
			*/

			if (!drop_info.is_mesos) {
				game_item_id item_id = drop_info.item_id;
				game_quest_id quest_id = drop_info.quest_id;

				if (quest_id > 0) {
					if (player == nullptr || player->get_quests()->item_drop_allowed(item_id, quest_id) == allow_quest_item_result::disallow) {
						continue;
					}
				}

				item f = vana::util::game_logic::item::is_equip(item_id) ?
					item{channel_server::get_instance().get_equip_data_provider(),
						item_id,
						stat_variance::normal,
						player != nullptr && player->has_gm_benefits()} :
					item{item_id, amount};

				value = new drop{map_id, f, pos, player_id};

				if (quest_id > 0) {
					value->set_quest(quest_id);
				}
			}
			else {
				game_mesos mesos = amount;
				if (!is_steal) {
					mesos *= drop_info.is_global ?
						global_meso_rate :
						meso_rate;

					if (player != nullptr) {
						auto meso_up = player->get_active_buffs()->get_meso_up_source();
						if (meso_up.is_initialized()) {
							mesos = (mesos * player->get_active_buffs()->get_buff_skill_info(meso_up.get())->x) / 100;
						}
					}
				}
				value = new drop{map_id, mesos, pos, player_id};
			}
		}

		if (value != nullptr) {
			if (explosive) {
				value->set_type(drop::explosive);
			}
			else if (ffa) {
				value->set_type(drop::free_for_all);
			}
			else if (party_id > 0) {
				value->set_type(drop::party);
				value->set_owner(party_id);
			}
			value->set_time(100);
			value->do_drop(origin);
			drop_pos_counter++;
			reactor_handler::check_drop(player, value);
		}
	}
}

auto drop_handler::drop_mesos(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	int32_t amount = reader.get<int32_t>();
	if (amount < vana::constant::inventory::min_drop_mesos || amount > vana::constant::inventory::max_drop_mesos) {
		// Hacking
		return;
	}
	auto query = player->get_inventory()->take_mesos(amount, false, true);
	if (query.get_result() == stack_result::none) {
		// Hacking
	}

	drop *value = new drop{player->get_map_id(), amount, player->get_pos(), player->get_id(), true};
	value->set_time(0);
	value->do_drop(player->get_pos());
}

auto drop_handler::pet_loot(ref_ptr<player> player, packet_reader &reader) -> void {
	game_pet_id pet_id = reader.get<game_pet_id>();
	loot_item(player, reader, pet_id);
}

auto drop_handler::loot_item(ref_ptr<player> player_value, packet_reader &reader, game_pet_id pet_id) -> void {
	reader.unk<uint8_t>();
	reader.skip<game_tick_count>();
	point player_pos = reader.get<point>();
	game_map_object drop_id = reader.get<game_map_object>();
	drop *drop = player_value->get_map()->get_drop(drop_id);

	if (drop == nullptr) {
		player_value->send(packets::drops::dont_take());
		return;
	}
	else if (drop->get_pos() - player_value->get_pos() > 300) {
		// Hacking
		return;
	}
	else if (player_value->is_using_gm_hide()) {
		player_value->send(packets::drops::drop_not_available_for_pickup());
		player_value->send(packets::drops::dont_take());
		return;
	}

	if (drop->is_quest()) {
		if (player_value->get_quests()->item_drop_allowed(drop->get_object_id(), drop->get_quest()) == allow_quest_item_result::disallow) {
			player_value->send(packets::drops::drop_not_available_for_pickup());
			player_value->send(packets::drops::dont_take());
			return;
		}
	}
	if (drop->is_mesos()) {
		int32_t player_rate = 100;
		game_mesos raw_mesos = drop->get_object_id();

		// TODO FIXME
		// This section is a bit broken
		// I'm not sure how partial meso pickups work in any cases
		// There are also questions of what happens to mesos if one or more party members cannot accept either in whole or in part the mesos they're given
		// Clear cases (accounted for):
		//	1. No party, has plenty of space to fit all mesos
		//	2. No party, has 0 space for mesos
		//	3. Has party, player has plenty of space to fit all mesos and so do all party members
		//	4. Has party, player has 0 space for mesos
		// Unclear cases:
		//	1. No party, player has some space for mesos but not all
		//	2. Has party, player has plenty of space for mesos but one or more party members has 0 space for mesos
		//	3. Has party, player has plenty of space for mesos but one or more party members has some space for mesos but not all
		//	4. Has party, player has some space for mesos but not all and all party members have plenty of space to fit all mesos
		//	5. Has party, player has some space for mesos but one or more party members has 0 space for mesos
		//	6. Has party, player has some space for mesos but one or more party members has some space for mesos but not all

		auto give_mesos = [](ref_ptr<player> p, game_mesos mesos) -> result {
			auto query = p->get_inventory()->add_mesos(mesos, true, true);
			switch (query.get_result()) {
				case stack_result::full:
					p->send(packets::drops::pickup_drop(mesos, 0, true));
					break;
				case stack_result::partial:
					// Packets?
					p->send(packets::drops::dont_take());
					break;
				case stack_result::none:
					p->send(packets::drops::dont_take());
					return result::failure;
				default: THROW_CODE_EXCEPTION(not_implemented_exception, "stack_result");
			}
			return result::success;
		};

		if (player_value->get_party() != nullptr && !drop->is_player_drop()) {
			// Player gets 100% unless partied and having others on the map, in which case it's 60%
			vector<ref_ptr<player>> members = player_value->get_party()->get_party_members(player_value->get_map_id());
			if (members.size() != 1) {
				player_rate = 60;
				game_mesos mesos = raw_mesos * player_rate / 100;

				if (give_mesos(player_value, mesos) == result::failure) {
					// Can't pick up the mesos
					return;
				}

				player_rate = 40 / (members.size() - 1);
				mesos = raw_mesos * player_rate / 100;
				ref_ptr<player> p = nullptr;

				for (uint8_t j = 0; j < members.size(); ++j) {
					p = members[j];
					if (p != player_value) {
						give_mesos(p, mesos);
					}
				}
			}
		}
		if (player_rate == 100) {
			if (give_mesos(player_value, raw_mesos) == result::failure) {
				return;
			}
		}
	}
	else {
		item drop_item = drop->get_item();
		auto cons = channel_server::get_instance().get_item_data_provider().get_consume_info(drop_item.get_id());
		if (cons != nullptr && cons->auto_consume) {
			if (vana::util::game_logic::item::is_monster_card(drop->get_object_id())) {
				player_value->send(packets::drops::pickup_drop_special(drop->get_object_id()));
				inventory::use_item(player_value, drop_item.get_id());
				player_value->send(packets::drops::dont_take());
				drop->take_drop(player_value, pet_id);
				return;
			}
			inventory::use_item(player_value, drop_item.get_id());
		}
		else {
			auto new_item = new item{drop_item};
			game_slot_qty drop_amount = drop->get_amount();
			if (new_item->has_karma()) {
				new_item->set_karma(false);
			}
			game_slot_qty amount = inventory::add_item(player_value, new_item, true);
			if (amount > 0) {
				if ((drop_amount - amount) > 0) {
					player_value->send(packets::drops::pickup_drop(drop->get_object_id(), drop_amount - amount));
					drop->set_item_amount(amount);
				}
				player_value->send(packets::drops::cant_get_anymore_items());
				player_value->send(packets::drops::dont_take());
				return;
			}
		}
		// TODO FIXME Bug here? drop->get_object_id is going to be either a meso count or item identifier
		// pickup_drop packet calls for game_map_object and it's unclear which is correct and which isn't
		player_value->send(packets::drops::pickup_drop(drop->get_object_id(), drop->get_amount()));
	}
	reactor_handler::check_loot(drop);
	drop->take_drop(player_value, pet_id);
}

}
}