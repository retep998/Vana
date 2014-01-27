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

#include "Types.h"
#include <mutex>
#include <string>
#include <vector>

namespace RankingCalculator {
	struct Rank {
		opt_int32_t oldRank;
		opt_int32_t newRank;
	};
	struct RankPlayer {
		uint8_t levelStat;
		uint8_t jobLevelMax;
		uint8_t worldId;
		int16_t jobStat;
		int16_t fameStat;
		int32_t expStat;
		int32_t charId;
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
	auto increaseRank(uint8_t level, uint8_t maxLevel, uint8_t lastLevel, int32_t exp, int32_t lastExp, int16_t job) -> bool;
	auto baseCompare(const RankPlayer &t1, const RankPlayer &t2) -> bool;
	auto updateRank(Rank &r, int32_t newRank) -> void;

	extern mutex_t RankingsMutex;
}