/*
Copyright (C) 2008-2012 Vana Development Team

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

using std::string;
using std::vector;

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

	void setTimer();
	void runThread();
	void all();
	void overall(vector<RankPlayer> &v);
	void world(vector<RankPlayer> &v);
	void job(vector<RankPlayer> &v);
	void fame(vector<RankPlayer> &v);
	bool increaseRank(uint8_t level, uint8_t maxLevel, uint8_t lastLevel, int32_t exp, int32_t lastExp, int16_t job);
	bool baseCompare(const RankPlayer &t1, const RankPlayer &t2);
	void updateRank(Rank &r, int32_t newRank);

	extern std::mutex RankingsMutex;
}