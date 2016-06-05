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

#include "Common/Types.hpp"
#include <unordered_map>

namespace vana {
	class packet_builder;
	enum class mystic_door_result;
	struct skill_level_info;

	namespace channel_server {
		class mystic_door;
		class party;
		class player;

		struct player_skill_info {
			game_skill_level level = 0;
			game_skill_level max_skill_level = 0;
			game_skill_level player_max_skill_level = 0;
		};

		class player_skills {
			NONCOPYABLE(player_skills);
			NO_DEFAULT_CONSTRUCTOR(player_skills);
		public:
			player_skills(player *player);

			auto load() -> void;
			auto save(bool save_cooldowns = false) -> void;
			auto connect_packet(packet_builder &builder) const -> void;
			auto connect_packet_for_blessing(packet_builder &builder) const -> void;

			auto add_skill_level(game_skill_id skill_id, game_skill_level amount, bool send_packet = true) -> bool;
			auto get_skill_level(game_skill_id skill_id) const -> game_skill_level;
			auto get_max_skill_level(game_skill_id skill_id) const -> game_skill_level;
			auto set_max_skill_level(game_skill_id skill_id, game_skill_level max_level, bool send_packet = true) -> void;
			auto get_skill_info(game_skill_id skill_id) const -> const skill_level_info * const;

			auto has_elemental_amp() const -> bool;
			auto has_energy_charge() const -> bool;
			auto has_hp_increase() const -> bool;
			auto has_mp_increase() const -> bool;
			auto has_venomous_weapon() const -> bool;
			auto has_achilles() const -> bool;
			auto has_dark_sight_interruption_skill() const -> bool;
			auto has_no_damage_skill() const -> bool;
			auto has_follow_the_lead() const -> bool;
			auto has_legendary_spirit() const -> bool;
			auto has_maker() const -> bool;
			auto has_blessing_of_the_fairy() const -> bool;
			auto get_elemental_amp() const -> game_skill_id;
			auto get_energy_charge() const -> game_skill_id;
			auto get_advanced_combo() const -> game_skill_id;
			auto get_alchemist() const -> game_skill_id;
			auto get_hp_increase() const -> game_skill_id;
			auto get_mp_increase() const -> game_skill_id;
			auto get_mastery() const -> game_skill_id;
			auto get_mp_eater() const -> game_skill_id;
			auto get_venomous_weapon() const -> game_skill_id;
			auto get_achilles() const -> game_skill_id;
			auto get_dark_sight_interruption_skill() const -> game_skill_id;
			auto get_no_damage_skill() const -> game_skill_id;
			auto get_follow_the_lead() const -> game_skill_id;
			auto get_legendary_spirit() const -> game_skill_id;
			auto get_maker() const -> game_skill_id;
			auto get_blessing_of_the_fairy() const -> game_skill_id;
			auto get_rechargeable_bonus() const -> game_slot_qty;

			auto add_cooldown(game_skill_id skill_id, seconds time) -> void;
			auto remove_cooldown(game_skill_id skill_id) -> void;
			auto remove_all_cooldowns() -> void;

			auto open_mystic_door(const point &pos, seconds door_time) -> mystic_door_result;
			auto close_mystic_door(bool from_timer) -> void;
			auto get_mystic_door() const -> ref_ptr<mystic_door>;
			auto on_join_party(party *party, ref_ptr<player> player) -> void;
			auto on_leave_party(party *party, ref_ptr<player> player, bool kicked) -> void;
			auto on_party_disband(party *party) -> void;
			auto on_map_change() const -> void;
			auto on_disconnect() -> void;
		private:
			auto has_skill(game_skill_id skill_id) const -> bool;

			player *m_player = nullptr;
			hash_map<game_skill_id, player_skill_info> m_skills;
			hash_map<game_skill_id, seconds> m_cooldowns;
			ref_ptr<mystic_door> m_mystic_door;
			string m_blessing_player;
		};
	}
}