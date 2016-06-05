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

#include "common/TimerContainerHolder.hpp"
#include "common/Types.hpp"
#include "common/Variables.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	struct portal_info;

	namespace channel_server {
		class lua_instance;
		class map;
		class party;
		class player;
		class reactor;

		namespace timer {
			struct id;
		}

		class instance : public timer_container_holder {
			NONCOPYABLE(instance);
			NO_DEFAULT_CONSTRUCTOR(instance);
		public:
			instance(const string &name, game_map_id map, game_player_id player_id, const duration &time, const duration &persistent, bool show_timer);
			~instance();

			auto get_name() const -> string { return m_name; }
			auto get_start() const -> const time_point & { return m_start; }
			auto get_marked_for_delete() const -> bool { return m_marked_for_deletion; }
			auto mark_for_delete() -> void;
			auto get_variables() const -> variables * { return m_variables.get(); }

			// Players
			auto get_all_player_ids() -> vector<game_player_id>;
			auto add_player(ref_ptr<player> player) -> void;
			auto remove_player(ref_ptr<player> player) -> void;
			auto remove_player(game_player_id id) -> void;
			auto remove_all_players() -> void;
			auto move_all_players(game_map_id map_id, bool respect_instances, const portal_info * const portal = nullptr) -> void;
			auto instance_has_players() const -> bool;
			auto get_player_num() const -> size_t { return m_players.size(); }

			// Maps
			auto add_map(map *map) -> void;
			auto add_map(game_map_id map_id) -> void;
			auto is_instance_map(game_map_id map_id) const -> bool;
			auto set_reset_at_end(bool reset) -> void { m_reset_on_destroy = reset; }
			auto respawn_mobs(game_map_id map_id) -> void;
			auto respawn_reactors(game_map_id map_id) -> void;

			// Parties
			auto add_party(party *party) -> void;

			// Instance time
			auto set_persistence(const duration &persistence) -> void;
			auto get_persistence() const -> duration;
			auto show_timer() const -> bool;
			auto show_timer(bool show, bool do_it = false) -> void;

			// Timers
			auto remove_all_timers() -> void;
			auto remove_timer(const string &name) -> void;
			auto add_future_timer(const string &name, seconds time, seconds persistence) -> bool;
			auto add_second_of_hour_timer(const string &name, int16_t second_of_hour, seconds persistence) -> bool;
			auto is_timer_persistent(const string &name) -> bool;
			auto get_instance_seconds_remaining() -> seconds;
			auto get_timer_seconds_remaining(const string &name) -> seconds;

			// Lua interaction
			auto begin_instance() -> result;
			auto player_death(game_player_id player_id) -> result;
			auto party_disband(game_party_id party_id) -> result;
			auto timer_end(const string &name, bool from_timer) -> result;
			auto player_disconnect(game_player_id player_id, bool is_party_leader) -> result;
			auto remove_party_member(game_party_id party_id, game_player_id player_id) -> result;
			auto mob_death(game_mob_id mob_id, game_map_object map_mob_id, game_map_id map_id) -> result;
			auto mob_spawn(game_mob_id mob_id, game_map_object map_mob_id, game_map_id map_id) -> result;
			auto player_change_map(game_player_id player_id, game_map_id new_map_id, game_map_id old_map_id, bool is_party_leader) -> result;
			auto friendly_mob_hit(game_mob_id mob_id, game_map_object map_mob_id, game_map_id map_id, int32_t mob_hp, int32_t mob_max_hp) -> result;
		private:
			struct timer_action {
				bool is_persistent = false;
				uint32_t counter_id = 0;
			};

			template <typename ... TArgs>
			auto call_instance_function(const string &func, TArgs ... args) -> result;
			auto set_instance_timer(const duration &time, bool first_run = false) -> void;
			auto timer_complete(const string &name, bool from_timer = false) -> void;
			auto remove_timer(const string &name, bool perform_event) -> void;
			auto get_lua_instance() -> lua_instance * { return m_lua_instance.get(); }
			auto get_counter_id() -> uint32_t;
			auto instance_end(bool called_by_lua, bool from_timer = false) -> void;

			bool m_show_timer = false;
			bool m_reset_on_destroy = true;
			bool m_marked_for_deletion = false;
			uint32_t m_timer_counter = 1;
			time_point m_start;
			duration m_persistent;
			string m_name;
			owned_ptr<variables> m_variables;
			owned_ptr<lua_instance> m_lua_instance; // Lua instance for interacting with scripts
			vector<map *> m_maps;
			vector<party *> m_parties;
			hash_map<string, timer_action> m_timer_actions; // Timers indexed by name
			hash_map<game_player_id, ref_ptr<player>> m_players;
		};

		template <typename ... TArgs>
		auto instance::call_instance_function(const string &func, TArgs ... args) -> result {
			auto lua_inst = get_lua_instance();
			if (!lua_inst->exists(func)) {
				return result::failure;
			}
			return lua_inst->call(func, args...);
		}
	}
}