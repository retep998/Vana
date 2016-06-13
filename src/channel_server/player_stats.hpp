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
#include <map>

namespace vana {
	class item;
	class packet_builder;
	class packet_reader;

	namespace channel_server {
		class player;

		struct bonus_set {
			int32_t hp = 0;
			int32_t mp = 0;
			int32_t str = 0;
			int32_t dex = 0;
			int32_t intl = 0;
			int32_t luk = 0;
		};

		struct equip_bonus : public bonus_set {
			int32_t id = 0;
		};

		class player_stats {
			NONCOPYABLE(player_stats);
			NO_DEFAULT_CONSTRUCTOR(player_stats);
		public:
			player_stats(ref_ptr<player> player,
				game_player_level level,
				game_job_id job,
				game_fame fame,
				game_stat str,
				game_stat dex,
				game_stat intl,
				game_stat luk,
				game_stat ap,
				game_health_ap hp_mp_ap,
				game_stat sp,
				game_health hp,
				game_health max_hp,
				game_health mp,
				game_health max_mp,
				game_experience exp);

			// Data modification
			auto check_hp_mp() -> void;
			auto set_level(game_player_level level) -> void;
			auto modify_hp(int32_t hp_mod, bool send_packet = true) -> void;
			auto modify_mp(int32_t mp_mod, bool send_packet = false) -> void;
			auto damage_hp(int32_t damage_hp) -> void;
			auto damage_mp(int32_t damage_mp) -> void;
			auto set_hp(game_health hp, bool send_packet = true) -> void;
			auto set_mp(game_health mp, bool send_packet = false) -> void;
			auto set_max_hp(game_health max_hp) -> void;
			auto set_max_mp(game_health max_mp) -> void;
			auto modify_max_hp(game_health mod) -> void;
			auto modify_max_mp(game_health mod) -> void;
			auto set_hyper_body_hp(int16_t mod) -> void;
			auto set_hyper_body_mp(int16_t mod) -> void;
			auto set_hp_mp_ap(game_health_ap ap) -> void { m_hp_mp_ap = ap; }
			auto set_exp(game_experience exp) -> void;
			auto set_ap(game_stat ap) -> void;
			auto set_sp(game_stat sp) -> void;

			auto set_fame(game_fame fame) -> void;
			auto set_job(game_job_id job) -> void;
			auto set_str(game_stat str) -> void;
			auto set_dex(game_stat dex) -> void;
			auto set_int(game_stat intl) -> void;
			auto set_luk(game_stat luk) -> void;
			auto set_maple_warrior(int16_t mod) -> void;
			auto lose_exp() -> void;

			auto set_equip(game_inventory_slot slot, item *equip, bool is_loading = false) -> void;

			// Level related functions
			auto give_exp(uint64_t exp, bool in_chat = false, bool white = true) -> void;
			auto add_stat(packet_reader &reader) -> void;
			auto add_stat_multi(packet_reader &reader) -> void;
			auto add_stat(int32_t type, int16_t mod = 1, bool is_reset = false) -> void;
			auto ap_reset_hp(bool is_reset, bool is_subtract, int16_t val, int16_t s_val = 0) -> int16_t;
			auto ap_reset_mp(bool is_reset, bool is_subtract, int16_t val, int16_t s_val = 0) -> int16_t;
			auto get_x(game_skill_id skill_id) -> int16_t;
			auto get_y(game_skill_id skill_id) -> int16_t;
			auto get_exp(game_player_level level) -> game_experience;

			// Data acquisition
			auto connect_packet(packet_builder &builder) -> void;
			auto get_level() const -> game_player_level { return m_level; }
			auto get_job() const -> game_job_id { return m_job; }
			auto get_exp() const -> game_experience { return m_exp; }
			auto get_ap() const -> game_stat { return m_ap; }
			auto get_hp_mp_ap() const -> game_health_ap { return m_hp_mp_ap; }
			auto get_sp() const -> game_stat { return m_sp; }
			auto get_fame() const -> game_fame { return m_fame; }

			auto get_str(bool with_bonus = false) -> game_stat;
			auto get_dex(bool with_bonus = false) -> game_stat;
			auto get_int(bool with_bonus = false) -> game_stat;
			auto get_luk(bool with_bonus = false) -> game_stat;
			auto get_hp() const -> game_health { return m_hp; }
			auto get_max_hp(bool without_bonus = false) -> game_health;
			auto get_mp() const -> game_health { return m_mp; }
			auto get_max_mp(bool without_bonus = false) -> game_health;
			auto is_dead() const -> bool;
		private:
			auto update_bonuses(bool update_equips = false, bool is_loading = false) -> void;
			auto modified_hp() -> void;
			auto rand_hp() -> game_health;
			auto rand_mp() -> game_health;
			auto level_hp(game_health val, game_health bonus = 0) -> game_health;
			auto level_mp(game_health val, game_health bonus = 0) -> game_health;
			auto stat_utility(int32_t test) -> int16_t;

			game_player_level m_level = 0;
			game_job_id m_job = 0;
			game_stat m_ap = 0;
			game_stat m_sp = 0;
			game_fame m_fame = 0;
			game_health m_hp = 0;
			game_health m_max_hp = 0;
			game_health m_mp = 0;
			game_health m_max_mp = 0;
			game_stat m_str = 0;
			game_stat m_dex = 0;
			game_stat m_int = 0;
			game_stat m_luk = 0;
			int16_t m_hyper_body_x = 0;
			int16_t m_hyper_body_y = 0;
			int16_t m_maple_warrior = 0;
			game_health_ap m_hp_mp_ap = 0;
			game_experience m_exp = 0;

			bonus_set m_equip_bonuses;
			bonus_set m_buff_bonuses;
			view_ptr<player> m_player;
			ord_map<int16_t, equip_bonus> m_equip_stats;
		};
	}
}