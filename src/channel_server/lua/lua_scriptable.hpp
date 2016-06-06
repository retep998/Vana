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

#include "common/lua/lua_environment.hpp"
#include "common/types.hpp"
#include <string>

namespace vana {
	namespace channel_server {
		class instance;
		class player;

		namespace lua {
			using namespace ::vana::lua;

			namespace variable_type {
				enum type : int32_t {
					boolean,
					string,
					number,
					integer,
				};
			}

			class lua_scriptable : public lua_environment {
				NONCOPYABLE(lua_scriptable);
				NO_DEFAULT_CONSTRUCTOR(lua_scriptable);
			protected:
				lua_scriptable(const string &filename, game_player_id player_id);
				lua_scriptable(const string &filename, game_player_id player_id, bool use_thread);

				auto handle_error(const string &filename, const string &error) -> void override;
				game_player_id m_player_id = -1;
			private:
				auto initialize() -> void;
				auto set_environment_variables() -> void;
				// TODO FIXME msvc
				// Remove this when MSVC supports static init
				static string s_api_version/* = "1.0.0"*/;
			};

			namespace lua_exports {
				auto get_environment(lua_State *lua_vm) -> lua_environment &;
				auto get_player(lua_State *lua_vm, lua_environment &env) -> ref_ptr<player>;
				auto get_player_deduced(int parameter, lua_State *lua_vm, lua_environment &env) -> ref_ptr<player>;
				auto get_instance(lua_State *lua_vm, lua_environment &env) -> instance *;
				auto obtain_set_variable_pair(lua_State *lua_vm, lua_environment &env) -> pair<string, string>;
				auto push_get_variable_data(lua_State *lua_vm, lua_environment &env, const string &value, variable_type::type return_type) -> lua_return;
				auto is_boss_channel(lua_State *lua_vm, const vector<game_channel_id> &channels) -> lua_return;

				// Global exports

				// Miscellaneous
				auto console_output(lua_State *lua_vm) -> lua_return;
				auto get_random_number(lua_State *lua_vm) -> lua_return;
				auto log(lua_State *lua_vm) -> lua_return;
				auto select_discrete(lua_State *lua_vm) -> lua_return;
				auto show_global_message(lua_State *lua_vm) -> lua_return;
				auto show_world_message(lua_State *lua_vm) -> lua_return;
				auto test_export(lua_State *lua_vm) -> lua_return;

				// Channel
				auto delete_channel_variable(lua_State *lua_vm) -> lua_return;
				auto get_channel(lua_State *lua_vm) -> lua_return;
				auto get_channel_variable(lua_State *lua_vm) -> lua_return;
				auto get_world(lua_State *lua_vm) -> lua_return;
				auto set_channel_variable(lua_State *lua_vm) -> lua_return;
				auto show_channel_message(lua_State *lua_vm) -> lua_return;

				// Bosses
				auto get_horntail_channels(lua_State *lua_vm) -> lua_return;
				auto get_max_horntail_battles(lua_State *lua_vm) -> lua_return;
				auto get_max_papulatus_battles(lua_State *lua_vm) -> lua_return;
				auto get_max_pianus_battles(lua_State *lua_vm) -> lua_return;
				auto get_max_pink_bean_battles(lua_State *lua_vm) -> lua_return;
				auto get_max_zakum_battles(lua_State *lua_vm) -> lua_return;
				auto get_papulatus_channels(lua_State *lua_vm) -> lua_return;
				auto get_pianus_channels(lua_State *lua_vm) -> lua_return;
				auto get_pink_bean_channels(lua_State *lua_vm) -> lua_return;
				auto get_zakum_channels(lua_State *lua_vm) -> lua_return;
				auto is_horntail_channel(lua_State *lua_vm) -> lua_return;
				auto is_papulatus_channel(lua_State *lua_vm) -> lua_return;
				auto is_pianus_channel(lua_State *lua_vm) -> lua_return;
				auto is_pink_bean_channel(lua_State *lua_vm) -> lua_return;
				auto is_zakum_channel(lua_State *lua_vm) -> lua_return;

				// NPC
				auto is_busy(lua_State *lua_vm) -> lua_return;
				auto remove_npc(lua_State *lua_vm) -> lua_return;
				auto run_npc(lua_State *lua_vm) -> lua_return;
				auto show_shop(lua_State *lua_vm) -> lua_return;
				auto spawn_npc(lua_State *lua_vm) -> lua_return;

				// Beauty
				auto get_all_faces(lua_State *lua_vm) -> lua_return;
				auto get_all_hairs(lua_State *lua_vm) -> lua_return;
				auto get_all_skins(lua_State *lua_vm) -> lua_return;
				auto get_random_face(lua_State *lua_vm) -> lua_return;
				auto get_random_hair(lua_State *lua_vm) -> lua_return;
				auto get_random_skin(lua_State *lua_vm) -> lua_return;
				auto is_valid_face(lua_State *lua_vm) -> lua_return;
				auto is_valid_hair(lua_State *lua_vm) -> lua_return;
				auto is_valid_skin(lua_State *lua_vm) -> lua_return;

				// Buddy
				auto add_buddy_slots(lua_State *lua_vm) -> lua_return;
				auto get_buddy_slots(lua_State *lua_vm) -> lua_return;

				// Skill
				auto add_skill_level(lua_State *lua_vm) -> lua_return;
				auto get_skill_level(lua_State *lua_vm) -> lua_return;
				auto get_max_skill_level(lua_State *lua_vm) -> lua_return;
				auto set_max_skill_level(lua_State *lua_vm) -> lua_return;

				// Quest
				auto get_quest_data(lua_State *lua_vm) -> lua_return;
				auto is_quest_active(lua_State *lua_vm) -> lua_return;
				auto is_quest_inactive(lua_State *lua_vm) -> lua_return;
				auto is_quest_completed(lua_State *lua_vm) -> lua_return;
				auto set_quest_data(lua_State *lua_vm) -> lua_return;

				// Inventory
				auto add_slots(lua_State *lua_vm) -> lua_return;
				auto add_storage_slots(lua_State *lua_vm) -> lua_return;
				auto destroy_equipped_item(lua_State *lua_vm) -> lua_return;
				auto get_equipped_item_in_slot(lua_State *lua_vm) -> lua_return;
				auto get_item_amount(lua_State *lua_vm) -> lua_return;
				auto get_max_stack_size(lua_State *lua_vm) -> lua_return;
				auto get_mesos(lua_State *lua_vm) -> lua_return;
				auto get_open_slots(lua_State *lua_vm) -> lua_return;
				auto give_item(lua_State *lua_vm) -> lua_return;
				auto give_item_gachapon(lua_State *lua_vm) -> lua_return;
				auto give_mesos(lua_State *lua_vm) -> lua_return;
				auto has_open_slots_for(lua_State *lua_vm) -> lua_return;
				auto is_equipped_item(lua_State *lua_vm) -> lua_return;
				auto is_valid_item(lua_State *lua_vm) -> lua_return;
				auto use_item(lua_State *lua_vm) -> lua_return;

				// Player
				auto delete_player_variable(lua_State *lua_vm) -> lua_return;
				auto end_morph(lua_State *lua_vm) -> lua_return;
				auto get_ap(lua_State *lua_vm) -> lua_return;
				auto get_dex(lua_State *lua_vm) -> lua_return;
				auto get_exp(lua_State *lua_vm) -> lua_return;
				auto get_face(lua_State *lua_vm) -> lua_return;
				auto get_fame(lua_State *lua_vm) -> lua_return;
				auto get_fh(lua_State *lua_vm) -> lua_return;
				auto get_gender(lua_State *lua_vm) -> lua_return;
				auto get_gm_level(lua_State *lua_vm) -> lua_return;
				auto get_hair(lua_State *lua_vm) -> lua_return;
				auto get_hp(lua_State *lua_vm) -> lua_return;
				auto get_hp_mp_ap(lua_State *lua_vm) -> lua_return;
				auto get_id(lua_State *lua_vm) -> lua_return;
				auto get_int(lua_State *lua_vm) -> lua_return;
				auto get_job(lua_State *lua_vm) -> lua_return;
				auto get_level(lua_State *lua_vm) -> lua_return;
				auto get_luk(lua_State *lua_vm) -> lua_return;
				auto get_map(lua_State *lua_vm) -> lua_return;
				auto get_max_hp(lua_State *lua_vm) -> lua_return;
				auto get_max_mp(lua_State *lua_vm) -> lua_return;
				auto get_mp(lua_State *lua_vm) -> lua_return;
				auto get_name(lua_State *lua_vm) -> lua_return;
				auto get_player_variable(lua_State *lua_vm) -> lua_return;
				auto get_pos_x(lua_State *lua_vm) -> lua_return;
				auto get_pos_y(lua_State *lua_vm) -> lua_return;
				auto get_real_max_hp(lua_State *lua_vm) -> lua_return;
				auto get_real_max_mp(lua_State *lua_vm) -> lua_return;
				auto get_skin(lua_State *lua_vm) -> lua_return;
				auto get_sp(lua_State *lua_vm) -> lua_return;
				auto get_str(lua_State *lua_vm) -> lua_return;
				auto give_ap(lua_State *lua_vm) -> lua_return;
				auto give_exp(lua_State *lua_vm) -> lua_return;
				auto give_fame(lua_State *lua_vm) -> lua_return;
				auto give_sp(lua_State *lua_vm) -> lua_return;
				auto is_active_item(lua_State *lua_vm) -> lua_return;
				auto is_active_skill(lua_State *lua_vm) -> lua_return;
				auto is_gm(lua_State *lua_vm) -> lua_return;
				auto is_online(lua_State *lua_vm) -> lua_return;
				auto revert_player(lua_State *lua_vm) -> lua_return;
				auto set_ap(lua_State *lua_vm) -> lua_return;
				auto set_dex(lua_State *lua_vm) -> lua_return;
				auto set_exp(lua_State *lua_vm) -> lua_return;
				auto set_hp(lua_State *lua_vm) -> lua_return;
				auto set_int(lua_State *lua_vm) -> lua_return;
				auto set_job(lua_State *lua_vm) -> lua_return;
				auto set_level(lua_State *lua_vm) -> lua_return;
				auto set_luk(lua_State *lua_vm) -> lua_return;
				auto set_map(lua_State *lua_vm) -> lua_return;
				auto set_max_hp(lua_State *lua_vm) -> lua_return;
				auto set_max_mp(lua_State *lua_vm) -> lua_return;
				auto set_mp(lua_State *lua_vm) -> lua_return;
				auto set_player(lua_State *lua_vm) -> lua_return;
				auto set_player_variable(lua_State *lua_vm) -> lua_return;
				auto set_sp(lua_State *lua_vm) -> lua_return;
				auto set_str(lua_State *lua_vm) -> lua_return;
				auto set_style(lua_State *lua_vm) -> lua_return;
				auto show_instruction_bubble(lua_State *lua_vm) -> lua_return;
				auto show_message(lua_State *lua_vm) -> lua_return;

				// Effects
				auto play_field_sound(lua_State *lua_vm) -> lua_return;
				auto play_minigame_sound(lua_State *lua_vm) -> lua_return;
				auto set_music(lua_State *lua_vm) -> lua_return;
				auto show_map_effect(lua_State *lua_vm) -> lua_return;
				auto show_map_event(lua_State *lua_vm) -> lua_return;

				// Map
				auto clear_drops(lua_State *lua_vm) -> lua_return;
				auto clear_mobs(lua_State *lua_vm) -> lua_return;
				auto count_mobs(lua_State *lua_vm) -> lua_return;
				auto set_portal_state(lua_State *lua_vm) -> lua_return;
				auto get_all_map_player_ids(lua_State *lua_vm) -> lua_return;
				auto get_num_players(lua_State *lua_vm) -> lua_return;
				auto get_reactor_state(lua_State *lua_vm) -> lua_return;
				auto kill_mobs(lua_State *lua_vm) -> lua_return;
				auto set_boat_docked(lua_State *lua_vm) -> lua_return;
				auto set_map_spawn(lua_State *lua_vm) -> lua_return;
				auto set_reactor_state(lua_State *lua_vm) -> lua_return;
				auto show_map_message(lua_State *lua_vm) -> lua_return;
				auto show_map_timer(lua_State *lua_vm) -> lua_return;
				auto spawn_mob(lua_State *lua_vm) -> lua_return;
				auto spawn_mob_pos(lua_State *lua_vm) -> lua_return;

				// Mob
				auto get_mob_fh(lua_State *lua_vm) -> lua_return;
				auto get_mob_hp(lua_State *lua_vm) -> lua_return;
				auto get_mob_max_hp(lua_State *lua_vm) -> lua_return;
				auto get_mob_max_mp(lua_State *lua_vm) -> lua_return;
				auto get_mob_mp(lua_State *lua_vm) -> lua_return;
				auto get_mob_pos_x(lua_State *lua_vm) -> lua_return;
				auto get_mob_pos_y(lua_State *lua_vm) -> lua_return;
				auto get_real_mob_id(lua_State *lua_vm) -> lua_return;
				auto kill_mob(lua_State *lua_vm) -> lua_return;
				auto mob_drop_item(lua_State *lua_vm) -> lua_return;

				// Time
				auto get_date(lua_State *lua_vm) -> lua_return;
				auto get_day(lua_State *lua_vm) -> lua_return;
				auto get_hour(lua_State *lua_vm) -> lua_return;
				auto get_minute(lua_State *lua_vm) -> lua_return;
				auto get_month(lua_State *lua_vm) -> lua_return;
				auto get_nearest_minute(lua_State *lua_vm) -> lua_return;
				auto get_second(lua_State *lua_vm) -> lua_return;
				auto get_time(lua_State *lua_vm) -> lua_return;
				auto get_time_zone_offset(lua_State *lua_vm) -> lua_return;
				auto get_week(lua_State *lua_vm) -> lua_return;
				auto get_year(lua_State *lua_vm) -> lua_return;
				auto is_dst(lua_State *lua_vm) -> lua_return;

				// Rates
				auto get_drop_meso(lua_State *lua_vm) -> lua_return;
				auto get_drop_rate(lua_State *lua_vm) -> lua_return;
				auto get_exp_rate(lua_State *lua_vm) -> lua_return;
				auto get_global_drop_meso(lua_State *lua_vm) -> lua_return;
				auto get_global_drop_rate(lua_State *lua_vm) -> lua_return;
				auto get_quest_exp_rate(lua_State *lua_vm) -> lua_return;

				// Party
				auto check_party_footholds(lua_State *lua_vm) -> lua_return;
				auto get_all_party_player_ids(lua_State *lua_vm) -> lua_return;
				auto get_party_count(lua_State *lua_vm) -> lua_return;
				auto get_party_id(lua_State *lua_vm) -> lua_return;
				auto get_party_map_count(lua_State *lua_vm) -> lua_return;
				auto is_party_in_level_range(lua_State *lua_vm) -> lua_return;
				auto is_party_leader(lua_State *lua_vm) -> lua_return;
				auto verify_party_footholds(lua_State *lua_vm) -> lua_return;
				auto warp_party(lua_State *lua_vm) -> lua_return;

				// Instance
				auto add_instance_map(lua_State *lua_vm) -> lua_return;
				auto add_instance_party(lua_State *lua_vm) -> lua_return;
				auto add_instance_player(lua_State *lua_vm) -> lua_return;
				auto check_instance_timer(lua_State *lua_vm) -> lua_return;
				auto create_instance(lua_State *lua_vm) -> lua_return;
				auto delete_instance_variable(lua_State *lua_vm) -> lua_return;
				auto get_all_instance_player_ids(lua_State *lua_vm) -> lua_return;
				auto get_instance_player_count(lua_State *lua_vm) -> lua_return;
				auto get_instance_player_id(lua_State *lua_vm) -> lua_return;
				auto get_instance_variable(lua_State *lua_vm) -> lua_return;
				auto is_instance(lua_State *lua_vm) -> lua_return;
				auto is_instance_map(lua_State *lua_vm) -> lua_return;
				auto is_instance_persistent(lua_State *lua_vm) -> lua_return;
				auto mark_for_delete(lua_State *lua_vm) -> lua_return;
				auto move_all_players(lua_State *lua_vm) -> lua_return;
				auto pass_players_between_instances(lua_State *lua_vm) -> lua_return;
				auto remove_all_instance_players(lua_State *lua_vm) -> lua_return;
				auto remove_instance_player(lua_State *lua_vm) -> lua_return;
				auto respawn_instance_mobs(lua_State *lua_vm) -> lua_return;
				auto respawn_instance_reactors(lua_State *lua_vm) -> lua_return;
				auto revert_instance(lua_State *lua_vm) -> lua_return;
				auto set_instance(lua_State *lua_vm) -> lua_return;
				auto set_instance_persistence(lua_State *lua_vm) -> lua_return;
				auto set_instance_reset(lua_State *lua_vm) -> lua_return;
				auto set_instance_variable(lua_State *lua_vm) -> lua_return;
				auto show_instance_time(lua_State *lua_vm) -> lua_return;
				auto start_instance_future_timer(lua_State *lua_vm) -> lua_return;
				auto start_instance_second_of_hour_timer(lua_State *lua_vm) -> lua_return;
				auto stop_all_instance_timers(lua_State *lua_vm) -> lua_return;
				auto stop_instance_timer(lua_State *lua_vm) -> lua_return;
			}
		}
	}
}