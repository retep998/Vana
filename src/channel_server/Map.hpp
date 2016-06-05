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

#include "common/foothold_info.hpp"
#include "common/id_pool.hpp"
#include "common/map_constants.hpp"
#include "common/point_temp.hpp"
#include "common/portal_info.hpp"
#include "common/rect_temp.hpp"
#include "common/respawnable_temp.hpp"
#include "common/seat_info.hpp"
#include "common/spawn_info.hpp"
#include "common/timer_container_holder.hpp"
#include "common/types_temp.hpp"
#include "channel_server/MapDataProvider.hpp"
#include "channel_server/Mob.hpp"
#include <ctime>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	class packet_builder;
	struct split_packet_builder;

	namespace channel_server {
		class drop;
		class instance;
		class mist;
		class mob;
		class player;
		class reactor;

		namespace spawn_types {
			enum {
				mob = 0x01,
				reactor = 0x02,
				all = mob | reactor
			};
		}

		struct mystic_door_open_result {
			mystic_door_open_result(mystic_door_result result) :
				result{result},
				town_id{vana::maps::no_map},
				portal{nullptr}
			{
			}

			mystic_door_open_result(game_map_id town_id, const portal_info * const portal) :
				result{mystic_door_result::success},
				town_id{town_id},
				portal{portal}
			{
			}

			mystic_door_result result;
			game_map_id town_id;
			const portal_info * const portal;
		};

		class map : public timer_container_holder {
			NONCOPYABLE(map);
			NO_DEFAULT_CONSTRUCTOR(map);
		public:
			map(ref_ptr<map_info> info, game_map_id id);

			auto boat_dock(bool is_docked) -> void;
			static auto set_map_unload_time(seconds new_time) -> void;

			// Map info
			static auto make_npc_id(game_map_object received_id) -> size_t;
			static auto make_reactor_id(game_map_object received_id) -> size_t;
			auto make_npc_id() -> game_map_object;
			auto make_reactor_id() -> game_map_object;
			auto set_music(const string &music_name) -> void;
			auto set_mob_spawning(game_mob_id spawn) -> void;
			auto can_vip() const -> bool { return !m_info->limitations.vip_rock; }
			auto can_chalkboard() const -> bool { return !m_info->limitations.chalkboard; }
			auto lose_one_percent() const -> bool { return m_info->limitations.regular_exp_loss || m_info->town || m_mobs.size() == 0; }
			auto get_continent() const -> uint8_t { return m_info->continent; }
			auto get_forced_return() const -> game_map_id { return m_info->forced_return; }
			auto get_return_map() const -> game_map_id { return m_info->return_map; }
			auto get_id() const -> game_map_id { return m_id; }
			auto get_dimensions() const -> rect { return m_real_dimensions; }
			auto get_music() const -> string { return m_music; }

			// Footholds
			auto find_floor(const point &pos, point &floor_pos, game_coord start_height_modifier = 0, const rect &search_area = rect{}) -> search_result;
			auto get_foothold_at_position(const point &pos) -> game_foothold_id;
			auto is_valid_foothold(game_foothold_id id) -> bool;
			auto is_vertical_foothold(game_foothold_id id) -> bool;
			auto get_position_at_foothold(game_foothold_id id) -> point;

			// Seats
			auto seat_occupied(game_seat_id id) -> bool;
			auto player_seated(game_seat_id id, ref_ptr<player> player) -> void;

			// Portals
			auto get_portal(const string &name) const -> const portal_info * const;
			auto get_spawn_point(game_portal_id portal_id = -1) const -> const portal_info * const;
			auto get_nearest_spawn_point(const point &pos) const -> const portal_info * const;
			auto query_portal_name(const string &name, ref_ptr<player> player = nullptr) const -> const portal_info * const;
			auto set_portal_state(const string &name, bool enabled) -> void;
			auto get_portal_names() const -> vector<string>;
			auto get_town_mystic_door_portal(ref_ptr<player> player) const -> mystic_door_open_result;
			auto get_town_mystic_door_portal(ref_ptr<player> player, uint8_t zero_based_party_index) const -> mystic_door_open_result;
			auto get_mystic_door_portal(ref_ptr<player> player) const -> mystic_door_open_result;
			auto get_mystic_door_portal(ref_ptr<player> player, uint8_t zero_based_party_index) const -> mystic_door_open_result;

			// Players
			auto add_player(ref_ptr<player> player) -> void;
			auto get_num_players() const -> size_t;
			auto get_player(size_t player_index) const -> ref_ptr<player>;
			auto get_player_names() -> string;
			auto remove_player(ref_ptr<player> player) -> void;
			auto check_player_equip(ref_ptr<player> player) -> void;
			auto run_function_players(const rect &dimensions, int16_t prop, function<void(ref_ptr<player>)> success_func) -> void;
			auto run_function_players(const rect &dimensions, int16_t prop, int16_t count, function<void(ref_ptr<player>)> success_func) -> void;
			auto run_function_players(function<void(ref_ptr<player>)> success_func) -> void;
			auto gm_hide_change(ref_ptr<player> player) -> void;
			auto get_all_player_ids() const -> vector<game_player_id>;

			// NPCs
			auto add_npc(const npc_spawn_info &npc) -> game_map_object;
			auto remove_npc(size_t npc_index) -> void;
			auto is_valid_npc_index(size_t npc_index) const -> bool;
			auto get_npc(size_t npc_index) const -> npc_spawn_info;

			// Mobs
			auto add_webbed_mob(game_map_object map_mob_id) -> void;
			auto remove_webbed_mob(game_map_object map_mob_id) -> void;
			auto mob_death(ref_ptr<mob> mob, bool from_explosion) -> void;
			auto heal_mobs(int32_t base_hp, int32_t hp_range, const rect &dimensions) -> void;
			auto status_mobs(vector<status_info> &statuses, const rect &dimensions) -> void;
			auto spawn_zakum(const point &pos, game_foothold_id foothold = 0) -> void;
			auto convert_shell_to_normal(ref_ptr<mob> mob) -> void;
			auto spawn_mob(game_mob_id mob_id, const point &pos, game_foothold_id foothold = 0, ref_ptr<mob> owner = nullptr, int8_t summon_effect = 0) -> ref_ptr<mob>;
			auto spawn_mob(int32_t spawn_id, const mob_spawn_info &info) -> ref_ptr<mob>;
			auto kill_mobs(ref_ptr<player> player, bool distribute_exp_and_drops, game_mob_id mob_id = 0) -> int32_t;
			auto count_mobs(game_mob_id mob_id = 0) -> int32_t;
			auto get_mob(game_map_object map_mob_id) -> ref_ptr<mob>;
			auto run_function_mobs(function<void(ref_ptr<const mob>)> func) -> void;
			auto switch_controller(ref_ptr<mob> mob, ref_ptr<player> new_controller) -> void;
			auto mob_summon_skill_used(ref_ptr<mob> mob, const mob_skill_level_info * const skill) -> void;

			// Reactors
			auto add_reactor(reactor *reactor) -> void;
			auto remove_reactor(size_t reactor_index) -> void;
			auto kill_reactors(bool show_packet = true) -> void;
			auto get_reactor(size_t reactor_index) const -> reactor *;
			auto get_num_reactors() const -> size_t;

			// Drops
			auto add_drop(drop *drop) -> void;
			auto get_drop(game_map_object id) -> drop *;
			auto remove_drop(game_map_object id) -> void;
			auto clear_drops(bool show_packet = true) -> void;

			// Mists
			auto add_mist(mist *mist) -> void;

			// Timer stuff
			auto set_map_timer(const seconds &timer) -> void;
			auto respawn(int8_t types = spawn_types::all) -> void;

			// Show all map objects
			auto show_objects(ref_ptr<player> player) -> void;

			// Packet stuff
			auto send(const packet_builder &builder, ref_ptr<player> sender = nullptr) -> void;
			auto send(const split_packet_builder &builder, ref_ptr<player> sender) -> void;

			// Instance
			auto set_instance(instance *inst) -> void { m_instance = inst; }
			auto end_instance(bool reset) -> void;
			auto get_instance() const -> instance * { return m_instance; }

			// Weather cash item
			auto create_weather(ref_ptr<player> player, bool admin_weather, int32_t time, game_item_id item_id, const string &message) -> bool;
		private:
			static const game_map_object npc_start = 100;
			static const game_map_object reactor_start = 200;
			// TODO FIXME msvc
			// Remove this crap comment once MSVC supports static initializers
			static int32_t s_map_unload_time/* = 0*/;

			friend class map_data_provider;
			auto add_foothold(const foothold_info &foothold) -> void;
			auto add_seat(const seat_info &seat) -> void;
			auto add_portal(const portal_info &portal) -> void;
			auto add_mob_spawn(const mob_spawn_info &spawn) -> void;
			auto add_reactor_spawn(const reactor_spawn_info &spawn) -> void;
			auto add_time_mob(ref_ptr<time_mob> info) -> void;
			auto check_spawn(time_point time) -> void;
			auto check_shadow_web() -> void;
			auto check_mists() -> void;
			auto clear_drops(time_point time) -> void;
			auto check_time_mob_spawn(bool first_load = true) -> void;
			auto spawn_shell(game_mob_id mob_id, const point &pos, game_foothold_id foothold) -> ref_ptr<mob>;
			auto update_mob_control(ref_ptr<player> player) -> void;
			auto update_mob_control(ref_ptr<mob> mob, mob_spawn_type spawn = mob_spawn_type::existing, ref_ptr<player> display = nullptr) -> void;
			auto map_tick(const time_point &now) -> void;
			auto get_time_mob_id() const -> game_map_object { return m_time_mob; }
			auto get_time_mob() const -> time_mob * { return m_time_mob_info.get(); }
			auto get_mist(game_mist_id id) -> mist *;
			auto find_controller(ref_ptr<mob> mob) -> ref_ptr<player>;
			auto clear_mists(bool show_packet = true) -> void;
			auto remove_mist(mist *mist) -> void;
			auto find_random_floor_pos() -> point;
			auto find_random_floor_pos(const rect &area) -> point;
			auto buff_players(game_item_id buff_id) -> void;

			// Longer-lived data
			bool m_ship = false;
			bool m_run_unloader = true;
			bool m_infer_size_from_footholds = false;
			game_map_id m_id = 0;
			game_map_object m_time_mob = 0;
			game_mob_id m_spawn_mobs = -1;
			int32_t m_empty_map_ticks = 0;
			int32_t m_min_spawn_count = 0;
			int32_t m_max_spawn_count = 0;
			int32_t m_max_mob_spawn_time = -1;
			instance *m_instance = nullptr;
			seconds m_timer = seconds{0};
			time_point m_timer_start = time_point{seconds{0}};
			time_point m_last_spawn = time_point{seconds{0}};
			string m_music;
			rect m_real_dimensions;
			id_pool<game_map_object> m_object_ids;
			id_pool<game_mist_id> m_mist_ids;
			recursive_mutex m_drops_mutex;
			ref_ptr<map_info> m_info;
			ref_ptr<time_mob> m_time_mob_info;
			vector<foothold_info> m_footholds;
			vector<reactor_spawn_info> m_reactor_spawns;
			vector<npc_spawn_info> m_npc_spawns;
			vector<mob_spawn_info> m_mob_spawns;
			ord_map<game_seat_id, seat_info> m_seats;
			hash_map<string, portal_info> m_portals;
			hash_map<game_portal_id, portal_info> m_spawn_points;
			vector<portal_info> m_door_points;
			hash_map<string, point> m_reactor_positions;

			// Shorter-lived objects
			vector<ref_ptr<player>> m_players;
			vector<reactor *> m_reactors;
			vector<respawnable> m_mob_respawns;
			vector<respawnable> m_reactor_respawns;
			hash_map<game_map_object, view_ptr<mob>> m_webbed;
			hash_map<game_map_object, ref_ptr<mob>> m_mobs;
			hash_map<game_player_id, ref_ptr<player>> m_players_without_protect_item;
			hash_map<game_map_object, drop *> m_drops;
			hash_map<game_mist_id, mist *> m_poison_mists;
			hash_map<game_mist_id, mist *> m_mists;
		};
	}
}