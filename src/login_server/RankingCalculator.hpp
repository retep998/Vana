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

#include "common_temp/Types.hpp"
#include "common_temp/UnixTime.hpp"
#include <mutex>
#include <string>
#include <vector>

namespace vana {
	namespace login_server {
		namespace ranking_calculator {
			struct rank {
				opt_int32_t old_rank;
				opt_int32_t new_rank;
			};
			struct rank_player {
				game_player_level level_stat;
				game_player_level job_level_max;
				game_world_id world_id;
				game_job_id job_stat;
				game_fame fame_stat;
				game_experience exp_stat;
				game_player_id char_id;
				unix_time level_time;
				rank overall;
				rank world;
				rank job;
				rank fame;
			};

			auto set_timer() -> void;
			auto run_thread() -> void;
			auto all() -> void;
			auto overall(vector<rank_player> &v) -> void;
			auto world(vector<rank_player> &v) -> void;
			auto job(vector<rank_player> &v) -> void;
			auto fame(vector<rank_player> &v) -> void;
			auto increase_rank(game_player_level level, game_player_level max_level, game_player_level last_level, game_experience exp, game_experience last_exp, game_job_id job) -> bool;
			auto base_compare(const rank_player &t1, const rank_player &t2) -> bool;
			auto update_rank(rank &r, int32_t new_rank) -> void;

			extern mutex g_rankings_mutex;
		}
	}
}