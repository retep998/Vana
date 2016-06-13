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

#include "common/data/type/buff_info.hpp"
#include "common/data/type/buff_source_type.hpp"
#include "common/i_packet.hpp"
#include "common/types.hpp"
#include "channel_server/buffs.hpp"
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

namespace vana {
	class packet_builder;
	class packet_reader;
	namespace timer {
		class container;
	}
	namespace data {
		namespace type {
			struct skill_level_info;
		}
	}

	namespace channel_server {
		class player;

		class player_active_buffs {
			NONCOPYABLE(player_active_buffs);
			NO_DEFAULT_CONSTRUCTOR(player_active_buffs);
		public:
			player_active_buffs(player *player);

			// Buff handling
			auto translate_to_source(int32_t buff_id) const -> data::type::buff_source;
			auto add_buff(const data::type::buff_source &source, const data::type::buff &buff, const seconds &time) -> result;
			auto remove_buff(const data::type::buff_source &source, const data::type::buff &buff, bool from_timer = false) -> void;
			auto remove_buffs() -> void;
			auto dispel_buffs() -> void;

			// Buff info
			auto get_buff_skill_info(const data::type::buff_source &source) const -> const data::type::skill_level_info * const;
			auto get_map_buff_values() -> buff_packet_structure;

			// Debuffs
			auto use_player_dispel() -> void;
			auto use_debuff_healing_item(int32_t mask) -> void;
			auto get_zombified_potency(int16_t base_potency) -> int16_t;
			auto remove_debuff(game_mob_skill_id skill_id) -> void;

			// Combo Attack
			auto reset_combo() -> void;
			auto add_combo() -> void;
			auto get_combo() const -> uint8_t;

			// Berserk
			auto get_berserk() const -> bool;
			auto check_berserk(bool display = false) -> void;

			// Energy Charge
			auto get_energy_charge_level() const -> int16_t;
			auto increase_energy_charge_level(int8_t targets = 1) -> void;

			// White Knight/Paladin charges
			auto stop_charge() -> void;
			auto has_ice_charge() const -> bool;

			// Pickpocket
			auto get_pickpocket_counter() -> int32_t;

			// Battleship
			auto get_battleship_hp() const -> int32_t;
			auto reset_battleship_hp() -> void;

			// Homing Beacon
			auto get_homing_beacon_mob() const -> game_map_object;
			auto reset_homing_beacon_mob(game_map_object map_mob_id = 0) -> void;

			// Commonly referred to (de)buffs on the server end
			auto has_infinity() const -> bool;
			auto has_holy_shield() const -> bool;
			auto has_shadow_partner() const -> bool;
			auto has_shadow_stars() const -> bool;
			auto has_soul_arrow() const -> bool;
			auto has_buff(data::type::buff_source_type type, int32_t buff_id) const -> bool;
			auto is_using_gm_hide() const -> bool;
			auto is_cursed() const -> bool;
			auto get_holy_symbol_rate() const -> int16_t;
			auto get_mount_item_id() const -> game_item_id;
			auto get_holy_symbol_source() const -> optional<data::type::buff_source>;
			auto get_power_stance_source() const -> optional<data::type::buff_source>;
			auto get_hyper_body_hp_source() const -> optional<data::type::buff_source>;
			auto get_hyper_body_mp_source() const -> optional<data::type::buff_source>;
			auto get_magic_guard_source() const -> optional<data::type::buff_source>;
			auto get_meso_guard_source() const -> optional<data::type::buff_source>;
			auto get_meso_up_source() const -> optional<data::type::buff_source>;
			auto get_homing_beacon_source() const -> optional<data::type::buff_source>;
			auto get_combo_source() const -> optional<data::type::buff_source>;
			auto get_charge_source() const -> optional<data::type::buff_source>;
			auto get_dark_sight_source() const -> optional<data::type::buff_source>;
			auto get_pickpocket_source() const -> optional<data::type::buff_source>;
			auto get_hamstring_source() const -> optional<data::type::buff_source>;
			auto get_blind_source() const -> optional<data::type::buff_source>;
			auto get_concentrate_source() const -> optional<data::type::buff_source>;
			auto get_buff_source(const data::type::buff_info &buff) const -> optional<data::type::buff_source>;
			auto end_morph() -> void;
			auto swap_weapon() -> void;
			auto take_damage(game_damage damage) -> void;

			auto get_transfer_packet() const -> packet_builder;
			auto parse_transfer_packet(packet_reader &reader) -> void;
		private:
			struct local_buff_info {
				data::type::buff raw;
				data::type::buff_source_type type;
				int32_t identifier;
				int32_t level;

				auto to_source() const -> data::type::buff_source;
			};

			auto translate_to_packet(const data::type::buff_source &source) const -> int32_t;
			auto has_buff(const data::type::buff_info &buff) const -> bool;
			auto has_buff(uint8_t bit_position) const -> bool;
			auto get_buff(uint8_t bit_position) const -> optional<data::type::buff_source>;
			auto get_buff_level(data::type::buff_source_type type, int32_t buff_id) const -> game_skill_level;
			auto get_buff_seconds_remaining(data::type::buff_source_type type, int32_t buff_id) const -> seconds;
			auto get_buff_seconds_remaining(const data::type::buff_source &source) const -> seconds;
			auto calculate_debuff_mask_bit(game_mob_skill_id skill_id) const -> int32_t;
			auto decrease_energy_charge_level() -> void;
			auto start_energy_charge_timer() -> void;
			auto stop_energy_charge_timer() -> void;
			auto stop_booster() -> void;
			auto stop_bullet_skills() -> void;
			auto stop_skill(const data::type::buff_source &source) -> void;
			auto set_combo(uint8_t combo) -> void;

			bool m_berserk = false;
			uint8_t m_combo = 0;
			uint8_t m_energy_charge_timer_counter = 0;
			int16_t m_energy_charge = 0;
			int16_t m_zombify_potency = 0;
			game_item_id m_mount_item_id = 0;
			int32_t m_pickpocket_counter = 0;
			int32_t m_battleship_hp = 0;
			game_map_object m_marked_mob = 0;
			uint32_t m_debuff_mask = 0;
			ref_ptr<player> m_player = nullptr;
			vector<local_buff_info> m_buffs;
		};
	}
}