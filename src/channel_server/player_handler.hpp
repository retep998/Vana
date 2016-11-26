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

namespace vana {
	class packet_reader;
	struct attack_data;
	namespace data {
		namespace type {
			enum class skill_type;
		}
	}

	namespace channel_server {
		class player;

		namespace player_handler {
			auto handle_door_use(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_damage(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_facial_expression(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_get_info(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_heal(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_moving(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_special_skills(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_monster_book(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_admin_messenger(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_stalk(ref_ptr<player> player, packet_reader &reader) -> void;

			auto use_bomb_skill(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_melee_attack(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_ranged_attack(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_spell_attack(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_energy_charge_attack(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_summon_attack(ref_ptr<player> player, packet_reader &reader) -> void;
			auto compile_attack(ref_ptr<player> player, packet_reader &reader, data::type::skill_type skill_type) -> attack_data;
		}
	}
}