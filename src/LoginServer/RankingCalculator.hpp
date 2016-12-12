/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "Types.hpp"
#include <mutex>
#include <string>
#include <vector>

namespace RankingCalculator {
	struct Rank {
		opt_int32_t oldRank;
		opt_int32_t newRank;
	};
	struct RankPlayer {
		player_level_t levelStat;
		player_level_t jobLevelMax;
		world_id_t worldId;
		job_id_t jobStat;
		fame_t fameStat;
		experience_t expStat;
		player_id_t charId;
		unix_time_t levelTime;
		Rank overall;
		Rank world;
		Rank job;
		Rank fame;
	};

	auto setTimer() -> void;
	auto runThread() -> void;
	auto all() -> void;
	auto overall(vector_t<RankPlayer> &v) -> void;
	auto world(vector_t<RankPlayer> &v) -> void;
	auto job(vector_t<RankPlayer> &v) -> void;
	auto fame(vector_t<RankPlayer> &v) -> void;
	auto increaseRank(player_level_t level, player_level_t maxLevel, player_level_t lastLevel, experience_t exp, experience_t lastExp, job_id_t job) -> bool;
	auto baseCompare(const RankPlayer &t1, const RankPlayer &t2) -> bool;
	auto updateRank(Rank &r, int32_t newRank) -> void;

	extern mutex_t RankingsMutex;
}