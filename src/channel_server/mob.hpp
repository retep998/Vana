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

#include "common/data/provider/mob.hpp"
#include "common/point.hpp"
#include "common/timer/container_holder.hpp"
#include "common/types.hpp"
#include "channel_server/movable_life.hpp"
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace vana {
	class packet_builder;
	struct mp_eater_data;

	namespace channel_server {
		class map;
		class party;
		class player;
		struct status_info;

		class mob : public movable_life, public enable_shared<mob>, public vana::timer::container_holder {
			NONCOPYABLE(mob);
			NO_DEFAULT_CONSTRUCTOR(mob);
		public:
			mob(game_map_object map_mob_id, game_map_id map_id, game_mob_id mob_id, view_ptr<mob> owner, const point &pos, int32_t spawn_id, bool faces_left, game_foothold_id foothold, mob_control_status control_status);

			auto apply_damage(game_player_id player_id, game_damage damage, bool poison = false) -> void;
			auto apply_web_damage() -> void;
			auto add_status(game_player_id player_id, vector<status_info> &status_info) -> void;
			auto skill_heal(int32_t heal_hp, int32_t heal_range) -> void;
			auto dispel_buffs() -> void;
			auto do_crash_skill(game_skill_id skill_id) -> void;
			auto explode() -> void;
			auto kill() -> void;
			auto consume_mp(int32_t mp) -> void;
			auto mp_eat(ref_ptr<player> player, mp_eater_data *mp) -> void;
			auto set_skill_feasibility(bool skill_feasible) -> void { m_skill_feasible = skill_feasible; }
			auto use_anticipated_skill() -> result;
			auto reset_anticipated_skill() -> void;
			auto get_status_bits() const -> int32_t;
			auto get_status_info() const -> const ord_map<int32_t, status_info> &;
			auto has_weapon_reflection() const -> bool;
			auto has_magic_reflection() const -> bool;
			auto get_weapon_reflection() -> optional<status_info>;
			auto get_magic_reflection() -> optional<status_info>;

			auto add_marker(ref_ptr<player> player) -> void;
			auto remove_marker(ref_ptr<player> player) -> void;
			auto choose_random_skill(ref_ptr<player> player, game_mob_skill_id &skill_id, game_mob_skill_level &skill_level) -> void;
			auto get_skill_feasibility() const -> bool { return m_skill_feasible; }
			auto get_anticipated_skill() const -> game_mob_skill_id { return m_anticipated_skill; }
			auto get_anticipated_skill_level() const -> game_mob_skill_level { return m_anticipated_skill_level; }
			auto get_hp_bar_color() const -> int8_t { return m_info->hp_color; }
			auto get_hp_bar_bg_color() const -> int8_t { return m_info->hp_background_color; }
			auto get_venom_count() const -> int8_t { return m_venom_count; }
			auto get_origin_foothold() const -> game_foothold_id { return m_origin_foothold; }
			auto get_level() const -> uint16_t { return m_info->level; }
			auto get_map_mob_id() const -> game_map_object { return m_map_mob_id; }
			auto get_map_id() const -> game_map_id { return m_map_id; }
			auto get_mob_id() const -> game_mob_id { return m_mob_id; }
			auto get_hp() const -> int32_t { return m_hp; }
			auto get_mp() const -> int32_t { return m_mp; }
			auto get_max_hp() const -> int32_t { return m_info->hp; }
			auto get_max_mp() const -> int32_t { return m_info->mp; }
			auto get_mob_id_or_link() const -> game_mob_id { return m_info->link != 0 ? m_info->link : m_mob_id; }
			auto get_self_destruct_hp() const -> int32_t { return m_info->self_destruction; }
			auto get_taunt_effect() const -> int32_t { return m_taunt_effect; }
			auto is_boss() const -> bool { return m_info->boss; }
			auto can_freeze() const -> bool { return m_info->can_freeze; }
			auto can_poison() const -> bool { return m_info->can_poison; }
			auto can_fly() const -> bool { return m_info->flying; }
			auto is_friendly() const -> bool { return m_info->friendly; }
			auto is_undead() const -> bool { return m_info->undead; }
			auto has_explosive_drop() const -> bool { return m_info->explosive_reward; }
			auto has_ffa_drop() const -> bool { return m_info->public_reward; }
			auto is_sponge() const -> bool { return is_sponge(get_mob_id()); }
			auto get_pos() const -> point override { return point{m_pos.x, m_pos.y - 1}; }
			auto get_control_status() const -> mob_control_status { return m_control_status; }

			auto get_controller() const -> ref_ptr<player> { return m_controller; }
			auto get_map() const -> map *;
		private:
			static auto is_sponge(game_mob_id mob_id) -> bool;
			static auto spawns_sponge(game_mob_id mob_id) -> bool;

			friend class map;

			auto set_controller(ref_ptr<player> control, mob_spawn_type spawn = mob_spawn_type::existing, ref_ptr<player> display = nullptr) -> void;
			auto die(ref_ptr<player> player, bool from_explosion = false) -> void;
			auto distribute_exp_and_get_drop_recipient(ref_ptr<player> killer) -> game_player_id;
			auto natural_heal(int32_t hp_heal, int32_t mp_heal) -> void;
			auto remove_status(int32_t status, bool from_timer = false) -> void;
			auto end_control() -> void;
			auto add_spawn(game_map_object map_mob_id, view_ptr<mob> mob) -> void { m_spawns[map_mob_id] = mob; }
			auto set_owner(view_ptr<mob> owner) -> void { m_owner = owner; }
			auto get_status_value(int32_t status) -> optional<status_info>;
			auto has_immunity() const -> bool;
			auto has_status(int32_t status) const -> bool;
			auto can_cast_skills() const -> bool;
			auto get_spawn_id() const -> int32_t { return m_spawn_id; }
			auto get_sponge() const -> view_ptr<mob> { return m_sponge; }

			bool m_skill_feasible = false;
			int8_t m_venom_count = 0;
			int8_t m_mp_eater_count = 0;
			game_skill_level m_web_level = 0;
			game_mob_skill_id m_anticipated_skill = 0;
			game_mob_skill_level m_anticipated_skill_level = 0;
			game_player_id m_anticipated_skill_player_id = 0;
			game_foothold_id m_origin_foothold = 0;
			int32_t m_taunt_effect = 100;
			game_map_object m_map_mob_id = 0;
			game_map_id m_map_id = 0;
			int32_t m_spawn_id = 0;
			game_mob_id m_mob_id = 0;
			int32_t m_hp = 0;
			int32_t m_mp = 0;
			int32_t m_status = 0;
			game_player_id m_web_player_id = 0;
			uint64_t m_total_health = 0;
			ref_ptr<player> m_controller = nullptr;
			mob_control_status m_control_status = mob_control_status::normal;
			time_point m_last_skill_use;
			view_ptr<mob> m_owner;
			view_ptr<mob> m_sponge;
			const ref_ptr<data::type::mob_info> m_info;
			vector<ref_ptr<player>> m_markers;
			ord_map<int32_t, status_info> m_statuses;
			hash_map<game_player_id, uint64_t> m_damages;
			hash_map<uint8_t, time_point> m_skill_use;
			hash_map<game_map_object, view_ptr<mob>> m_spawns;
		};
	}
}