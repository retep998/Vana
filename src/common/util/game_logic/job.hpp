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

#include "common/constant/job/beginner_jobs.hpp"
#include "common/constant/job/id.hpp"
#include "common/constant/job/progression.hpp"
#include "common/types.hpp"

namespace vana {
	namespace util {
		namespace game_logic {
			namespace job {
				inline auto is_adventurer(game_job_id job_id) -> bool { return job_id != 128 && job_id != 256 && (job_id == constant::job::id::beginner || (job_id >= 100 && job_id <= 910)); }
				inline auto is_cygnus(game_job_id job_id) -> bool { return job_id != 1024 && job_id >= 1000 && job_id < 2000; }
				inline auto is_legend(game_job_id job_id) -> bool { return job_id != 2048 && job_id >= 2000 && job_id < 3000; }
				inline auto is_resistance(game_job_id job_id) -> bool { return job_id >= 3000 && job_id < 4000; }
				inline auto get_job_type(game_job_id job_id) -> job_type {
					if (is_adventurer(job_id)) return job_type::adventurer;
					if (is_cygnus(job_id)) return job_type::cygnus;
					if (is_legend(job_id)) return job_type::legend;
					if (is_resistance(job_id)) return job_type::resistance;
					throw std::invalid_argument{"job_id must be a valid type"};
				}
				inline auto is_beginner_job(game_job_id job_id) -> bool {
					for (const auto &job : constant::job::beginner_jobs) {
						if (job_id == job) {
							return true;
						}
					}
					return false;
				}
				inline auto get_job_track(game_job_id job_id) -> int8_t { return job_id / 100; }
				inline auto get_job_line(game_job_id job_id) -> int8_t { return is_adventurer(job_id) ? (job_id / 100) : ((job_id / 100) % 10); }
				inline auto get_job_progression(game_job_id job_id) -> int8_t {
					if (is_beginner_job(job_id)) {
						return constant::job::progression::beginner;
					}

					int16_t job_progression = (job_id % 10);
					if (job_progression == 0) {
						// Might be first job or second job
						if (get_job_track(job_id) == 0) {
							return constant::job::progression::first_job;
						}
					}
					return constant::job::progression::second_job + job_progression;
				}

				inline auto get_max_level(game_job_id job_id) -> game_player_level { return is_cygnus(job_id) ? constant::stat::cygnus_levels : constant::stat::player_levels; }
			}
		}
	}
}