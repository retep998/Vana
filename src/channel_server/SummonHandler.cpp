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
#include "SummonHandler.hpp"
#include "common_temp/BuffDataProvider.hpp"
#include "common_temp/GameLogicUtilities.hpp"
#include "common_temp/IdPool.hpp"
#include "common_temp/PacketReader.hpp"
#include "common_temp/PacketWrapper.hpp"
#include "common_temp/SkillConstants.hpp"
#include "common_temp/SkillConstants.hpp"
#include "common_temp/SkillDataProvider.hpp"
#include "channel_server/BuffsPacket.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/Map.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/MovementHandler.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerPacket.hpp"
#include "channel_server/Summon.hpp"
#include "channel_server/SummonsPacket.hpp"

namespace vana {
namespace channel_server {

id_pool<game_summon_id> summon_handler::g_summon_ids;

auto summon_handler::use_summon(ref_ptr<player> player, game_skill_id skill_id, game_skill_level level) -> void {
	// Determine if any summons need to be removed and do it
	switch (skill_id) {
		case vana::skills::ranger::puppet:
		case vana::skills::sniper::puppet:
		case vana::skills::wind_archer::puppet:
			player->get_summons()->for_each([player, skill_id](summon *summon) {
				if (summon->get_skill_id() == skill_id) {
					remove_summon(player, summon->get_id(), false, summon_messages::none);
				}
			});
			break;
		case vana::skills::ranger::silver_hawk:
		case vana::skills::bowmaster::phoenix:
		case vana::skills::sniper::golden_eagle:
		case vana::skills::marksman::frostprey:
			player->get_summons()->for_each([player, skill_id](summon *summon) {
				if (!game_logic_utilities::is_puppet(summon->get_skill_id())) {
					remove_summon(player, summon->get_id(), false, summon_messages::none);
				}
			});
			break;
		case vana::skills::outlaw::gaviota:
		case vana::skills::outlaw::octopus:
		case vana::skills::corsair::wrath_of_the_octopi: {
			int8_t max_count = -1;
			int8_t current_count = 0;
			switch (skill_id) {
				case vana::skills::outlaw::octopus: max_count = 2; break;
				case vana::skills::corsair::wrath_of_the_octopi: max_count = 3; break;
				case vana::skills::outlaw::gaviota: max_count = 4; break;
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
	bool puppet = game_logic_utilities::is_puppet(skill_id);
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

	// I am not certain what this is, but in the Odin source they seemed to think it was original position. However, it caused AIDS.
	reader.unk<uint32_t>();

	summon *summon = player->get_summons()->get_summon(summon_id);
	if (summon == nullptr || summon->get_movement_type() == summon::fixed) {
		// Up to no good, lag, or something else
		return;
	}

	movement_handler::parse_movement(summon, reader);
	reader.reset(10);
	player->send_map(packets::move_summon(player->get_id(), summon, summon->get_pos(), reader.get_buffer(), (reader.get_buffer_length() - 9)));
}

auto summon_handler::damage_summon(ref_ptr<player> player, packet_reader &reader) -> void {
	game_summon_id summon_id = reader.get<game_summon_id>();
	int8_t unk = reader.get<int8_t>();
	game_damage damage = reader.get<game_damage>();
	game_map_object mob_id = reader.get<game_map_object>();

	if (summon *summon = player->get_summons()->get_summon(summon_id)) {
		if (!game_logic_utilities::is_puppet(summon->get_skill_id())) {
			// Hacking
			return;
		}

		summon->do_damage(damage);
		if (summon->get_hp() <= 0) {
			remove_summon(player, summon_id, false, summon_messages::none, true);
		}
	}
}

auto summon_handler::make_buff(ref_ptr<player> player, game_item_id item_id) -> buff_info {
	const auto &buff_data = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	switch (item_id) {
		case items::beholder_hex_watk: return buff_data.physical_attack;
		case items::beholder_hex_wdef: return buff_data.physical_defense;
		case items::beholder_hex_mdef: return buff_data.magic_defense;
		case items::beholder_hex_acc: return buff_data.accuracy;
		case items::beholder_hex_avo: return buff_data.avoid;
	}
	// Hacking?
	throw std::invalid_argument{"invalid_argument"};
}

auto summon_handler::make_active_buff(ref_ptr<player> player, const buff_info &data, game_item_id item_id, const skill_level_info *skill_info) -> buff_packet_values {
	buff_packet_values buff;
	buff.player.types[data.get_buff_byte()] = static_cast<uint8_t>(data.get_buff_type());
	switch (item_id) {
		case items::beholder_hex_wdef: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->w_def)); break;
		case items::beholder_hex_mdef: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->m_def)); break;
		case items::beholder_hex_acc: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->acc)); break;
		case items::beholder_hex_avo: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->avo)); break;
		case items::beholder_hex_watk: buff.player.values.push_back(buff_packet_value::from_value(2, skill_info->w_atk)); break;
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
		case vana::skills::dark_knight::hex_of_beholder: {
			int8_t buff_id = reader.get<int8_t>();
			if (buff_id < 0 || buff_id > ((level - 1) / 5)) {
				// Hacking
				return;
			}

			game_item_id item_id = items::beholder_hex_wdef + buff_id;
			seconds duration = skill_info->buff_time;
			if (buffs::add_buff(player, item_id, duration) == result::failure) {
				return;
			}
			break;
		}
		case vana::skills::dark_knight::aura_of_beholder:
			player->get_stats()->modify_hp(skill_info->hp_prop);
			break;
		default:
			// Hacking
			return;
	}

	player->send_map(packets::summon_skill(player->get_id(), skill_id, display, level), true);
	player->send_map(packets::summon_skill_effect(player->get_id(), vana::skills::dark_knight::beholder, display, level));
}

}
}