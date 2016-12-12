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
#include "summon_handler.hpp"
#include "common/data/provider/buff.hpp"
#include "common/data/provider/skill.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/util/game_logic/player_skill.hpp"
#include "common/util/id_pool.hpp"
#include "channel_server/buffs_packet.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/map.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/move_path.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/summon.hpp"
#include "channel_server/summons_packet.hpp"

namespace vana {
namespace channel_server {

vana::util::id_pool<game_summon_id> summon_handler::g_summon_ids;

auto summon_handler::use_summon(ref_ptr<player> player, game_skill_id skill_id, game_skill_level level) -> void {
	// Determine if any summons need to be removed and do it
	switch (skill_id) {
		case constant::skill::ranger::puppet:
		case constant::skill::sniper::puppet:
		case constant::skill::wind_archer::puppet:
			player->get_summons()->for_each([player, skill_id](summon *summon) {
				if (summon->get_skill_id() == skill_id) {
					remove_summon(player, summon->get_id(), false, summon_messages::none);
				}
			});
			break;
		case constant::skill::ranger::silver_hawk:
		case constant::skill::bowmaster::phoenix:
		case constant::skill::sniper::golden_eagle:
		case constant::skill::marksman::frostprey:
			player->get_summons()->for_each([player, skill_id](summon *summon) {
				if (!vana::util::game_logic::player_skill::is_puppet(summon->get_skill_id())) {
					remove_summon(player, summon->get_id(), false, summon_messages::none);
				}
			});
			break;
		case constant::skill::outlaw::gaviota:
		case constant::skill::outlaw::octopus:
		case constant::skill::corsair::wrath_of_the_octopi: {
			int8_t max_count = -1;
			int8_t current_count = 0;
			switch (skill_id) {
				case constant::skill::outlaw::octopus: max_count = 2; break;
				case constant::skill::corsair::wrath_of_the_octopi: max_count = 3; break;
				case constant::skill::outlaw::gaviota: max_count = 4; break;
			}

			player->get_summons()->for_each([player, skill_id, &current_count](summon *summon) {
				if (summon->get_skill_id() == skill_id) {
					current_count++;
				}
			});

			if (current_count == max_count) {
				// We have to remove one
				bool removed = false;
				player->get_summons()->for_each([player, skill_id, &removed](summon *summon) {
					if (summon->get_skill_id() != skill_id || removed) {
						return;
					}
					remove_summon(player, summon->get_id(), false, summon_messages::none);
					removed = true;
				});
			}
			break;
		}
		default:
			// By default, you can only have one summon out
			player->get_summons()->for_each([player, skill_id](summon *summon) {
				remove_summon(player, summon->get_id(), false, summon_messages::none);
			});
	}

	point player_position = player->get_pos();
	point summon_position;
	game_foothold_id foothold = player->get_foothold();
	bool puppet = vana::util::game_logic::player_skill::is_puppet(skill_id);
	if (puppet) {
		// TODO FIXME formula
		// TODO FIXME skill
		// This is not kosher
		player_position.x += 200 * (player->is_facing_right() ? 1 : -1);
		player->get_map()->find_floor(player_position, summon_position, -5);
		foothold = player->get_map()->get_foothold_at_position(summon_position);
	}
	else {
		summon_position = player_position;
	}
	summon *value = new summon{g_summon_ids.lease(), skill_id, level, player->is_facing_left() && !puppet, summon_position};
	if (value->get_movement_type() == summon::fixed) {
		value->reset_movement(foothold, value->get_pos(), value->get_stance());
	}

	auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level);
	player->get_summons()->add_summon(value, skill->buff_time);
	player->send_map(packets::show_summon(player->get_id(), value, false));
}

auto summon_handler::remove_summon(ref_ptr<player> player, game_summon_id summon_id, bool packet_only, int8_t show_message, bool from_timer) -> void {
	summon *summon = player->get_summons()->get_summon(summon_id);
	if (summon != nullptr) {
		player->send_map(packets::remove_summon(player->get_id(), summon, show_message));
		if (!packet_only) {
			player->get_summons()->remove_summon(summon_id, from_timer);
		}
	}
}

auto summon_handler::show_summon(ref_ptr<player> player) -> void {
	player->get_summons()->for_each([player](summon *summon) {
		summon->set_pos(player->get_pos());
		player->send_map(packets::show_summon(player->get_id(), summon));
	});
}

auto summon_handler::show_summons(ref_ptr<player> from_player, ref_ptr<player> to_player) -> void {
	from_player->get_summons()->for_each([from_player, to_player](summon *summon) {
		to_player->send(packets::show_summon(from_player->get_id(), summon));
	});
}

auto summon_handler::move_summon(ref_ptr<player> player, packet_reader &reader) -> void {
	game_summon_id summon_id = reader.get<game_summon_id>();

	summon *summon = player->get_summons()->get_summon(summon_id);
	if (summon == nullptr || summon->get_movement_type() == summon::fixed) {
		// Up to no good, lag, or something else
		return;
	}

	move_path path(reader);
	summon->reset_from_move_path(path);
	player->send_map(packets::move_summon(player->get_id(), summon, path), true);
}

auto summon_handler::damage_summon(ref_ptr<player> player, packet_reader &reader) -> void {
	game_summon_id summon_id = reader.get<game_summon_id>();
	int8_t unk = reader.get<int8_t>();
	game_damage damage = reader.get<game_damage>();
	game_map_object mob_id = reader.get<game_map_object>();

	if (summon *summon = player->get_summons()->get_summon(summon_id)) {
		if (!vana::util::game_logic::player_skill::is_puppet(summon->get_skill_id())) {
			// Hacking
			return;
		}

		summon->do_damage(damage);
		if (summon->get_hp() <= 0) {
			remove_summon(player, summon_id, false, summon_messages::none, true);
		}
	}
}

auto summon_handler::make_buff(ref_ptr<player> player, game_item_id item_id) -> data::type::buff_info {
	const auto &buff_data = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	switch (item_id) {
		case constant::item::beholder_hex_watk: return buff_data.physical_attack;
		case constant::item::beholder_hex_wdef: return buff_data.physical_defense;
		case constant::item::beholder_hex_mdef: return buff_data.magic_defense;
		case constant::item::beholder_hex_acc: return buff_data.accuracy;
		case constant::item::beholder_hex_avo: return buff_data.avoid;
	}
	// Hacking?
	throw std::invalid_argument{"invalid_argument"};
}

auto summon_handler::make_active_buff(ref_ptr<player> player, const data::type::buff_info &data, game_item_id item_id, const data::type::skill_level_info *skill_info) -> buff_packet_values {
	buff_packet_values buff;
	buff.player.types[data.get_buff_byte()] = static_cast<uint8_t>(data.get_buff_type());
	switch (item_id) {
		case constant::item::beholder_hex_wdef: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->w_def)); break;
		case constant::item::beholder_hex_mdef: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->m_def)); break;
		case constant::item::beholder_hex_acc: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->acc)); break;
		case constant::item::beholder_hex_avo: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->avo)); break;
		case constant::item::beholder_hex_watk: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->w_atk)); break;
	}
	return buff;
}

auto summon_handler::summon_skill(ref_ptr<player> player, packet_reader &reader) -> void {
	game_summon_id summon_id = reader.get<game_summon_id>();
	summon *summon = player->get_summons()->get_summon(summon_id);
	if (summon == nullptr) {
		return;
	}

	game_skill_id skill_id = reader.get<game_skill_id>();
	uint8_t display = reader.get<uint8_t>();
	game_skill_level level = player->get_skills()->get_skill_level(skill_id);
	auto skill_info = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level);
	if (skill_info == nullptr) {
		// Hacking
		return;
	}
	switch (skill_id) {
		case constant::skill::dark_knight::hex_of_beholder: {
			int8_t buff_id = reader.get<int8_t>();
			if (buff_id < 0 || buff_id > ((level - 1) / 5)) {
				// Hacking
				return;
			}

			game_item_id item_id = constant::item::beholder_hex_wdef + buff_id;
			seconds duration = skill_info->buff_time;
			if (buffs::add_buff(player, item_id, duration) == result::failure) {
				return;
			}
			break;
		}
		case constant::skill::dark_knight::aura_of_beholder:
			player->get_stats()->modify_hp(skill_info->hp_prop);
			break;
		default:
			// Hacking
			return;
	}

	player->send_map(packets::summon_skill(player->get_id(), skill_id, display, level), true);
	player->send_map(packets::summon_skill_effect(player->get_id(), constant::skill::dark_knight::beholder, display, level));
}

}
}