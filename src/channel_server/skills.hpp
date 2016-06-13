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

#include "common/types.hpp"
#include <vector>

namespace vana {
	class packet_reader;
	namespace data {
		namespace type {
			class buff_source;
		}
	}

	namespace channel_server {
		class party;
		class player;

		namespace skills {
			auto add_skill(ref_ptr<player> player, packet_reader &reader) -> void;
			auto cancel_skill(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_skill(ref_ptr<player> player, packet_reader &reader) -> void;
			auto get_affected_party_members(party *party, int8_t affected, int8_t members) -> const vector<ref_ptr<player>>;
			auto apply_skill_costs(ref_ptr<player> player, game_skill_id skill_id, game_skill_level level, bool elemental_amp = false) -> result;
			auto use_attack_skill(ref_ptr<player> player, game_skill_id skill_id) -> result;
			auto use_attack_skill_ranged(ref_ptr<player> player, game_skill_id skill_id, game_inventory_slot projectile_pos, game_inventory_slot cash_projectile_pos, game_item_id projectile_id) -> result;
			auto heal(ref_ptr<player> player, int64_t value, const data::type::buff_source &source) -> void;
			auto hurt(ref_ptr<player> player, int64_t value, const data::type::buff_source &source) -> void;
			auto stop_skill(ref_ptr<player> player, const data::type::buff_source &source, bool from_timer = false) -> void;
			auto start_cooldown(ref_ptr<player> player, game_skill_id skill_id, seconds cool_time, bool initial_load = false) -> void;
			auto stop_cooldown(ref_ptr<player> player, game_skill_id skill_id) -> void;
			auto is_cooling(ref_ptr<player> player, game_skill_id skill_id) -> bool;
			auto get_cooldown_time_left(ref_ptr<player> player, game_skill_id skill_id) -> int16_t;
		}
	}
}