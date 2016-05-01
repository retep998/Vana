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
#include "RankingCalculator.hpp"
#include "Common/Database.hpp"
#include "Common/GameConstants.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/InitializeCommon.hpp"
#include "Common/JobConstants.hpp"
#include "Common/StopWatch.hpp"
#include "Common/StringUtilities.hpp"
#include "Common/Timer.hpp"
#include "Common/TimerThread.hpp"
#include "Common/TimeUtilities.hpp"
#include "LoginServer/LoginServer.hpp"
#include "LoginServer/World.hpp"
#include "LoginServer/Worlds.hpp"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

namespace Vana {
namespace LoginServer {

mutex_t RankingCalculator::RankingsMutex;

auto RankingCalculator::setTimer() -> void {
	//Timer::Timer::create([]() { RankingCalculator::runThread(); },
	//	Timer::Id{TimerType::RankTimer),
	//	nullptr, TimeUtilities::getDistanceToNextOccurringSecondOfHour(0), hours_t{1});
	// Calculate ranking every 1 hour, starting on the hour
}

auto RankingCalculator::runThread() -> void {
	// Ranking on larger servers may take a long time and we don't want that to be blocking
	// The thread_t object will be deleted immediately, but the thread will continue to run
	auto p = make_owned_ptr<thread_t>([] { RankingCalculator::all(); });
	p->detach();
}

auto RankingCalculator::all() -> void {
	// There's no guarantee what effect running two at once will have, but it's likely to be bad
	owned_lock_t<mutex_t> l{RankingsMutex, std::try_to_lock};
	if (!l) {
		return;
	}

	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Calculating rankings... " << std::endl;
	StopWatch sw;

	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	RankPlayer out;
	soci::statement statement = (sql.prepare
		<< "SELECT c.character_id, c.exp, c.fame, c.job, c.level, c.world_id, c.time_level, c.fame_cpos, c.world_cpos, c.job_cpos, c.overall_cpos "
		<< "FROM " << db.makeTable("characters") << " c "
		<< "INNER JOIN " << db.makeTable("accounts") << " u ON u.account_id = c.account_id "
		<< "WHERE "
		<< "	(u.banned = 0 OR u.ban_expire >= NOW()) "
		<< "	AND u.gm_level IS NULL "
		<< "	AND u.admin IS NULL "
		<< "	AND ("
		<< "		("
		<< "			c.job IN (" << StringUtilities::delimit(",", Jobs::Beginners::Jobs) << ")"
		<< "			AND c.level > 9"
		<< "		)"
		<< "		OR c.job NOT IN (" << StringUtilities::delimit(",", Jobs::Beginners::Jobs) << ")"
		<< "	) "
		<< "ORDER BY c.overall_cpos DESC",
		soci::into(out.charId),
		soci::into(out.expStat),
		soci::into(out.fameStat),
		soci::into(out.jobStat),
		soci::into(out.levelStat),
		soci::into(out.worldId),
		soci::into(out.levelTime),
		soci::into(out.fame.newRank),
		soci::into(out.world.newRank),
		soci::into(out.job.newRank),
		soci::into(out.overall.newRank));

	vector_t<RankPlayer> v;
	statement.execute();

	while (statement.fetch()) {
		out.jobLevelMax = GameLogicUtilities::getMaxLevel(out.jobStat);
		v.push_back(out);
	}

	if (v.size() > 0) {
		overall(v);
		world(v);
		job(v);
		fame(v);

		player_id_t charId = 0;
		opt_int32_t oFame = 0;
		int32_t cFame = 0;
		opt_int32_t oWorld = 0;
		int32_t cWorld = 0;
		opt_int32_t oJob = 0;
		int32_t cJob = 0;
		opt_int32_t oOverall = 0;
		int32_t cOverall = 0;

		soci::statement st = (sql.prepare
			<< "UPDATE " << db.makeTable("characters") << " "
			<< "SET "
			<< "	fame_opos = :ofame,"
			<< "	fame_cpos = :cfame,"
			<< "	world_opos = :oworld,"
			<< "	world_cpos = :cworld,"
			<< "	job_opos = :ojob,"
			<< "	job_cpos = :cjob,"
			<< "	overall_opos = :ooverall,"
			<< "	overall_cpos = :coverall "
			<< "	WHERE character_id = :char",
			soci::use(charId, "char"),
			soci::use(oFame, "ofame"),
			soci::use(cFame, "cfame"),
			soci::use(oWorld, "oworld"),
			soci::use(cWorld, "cworld"),
			soci::use(oJob, "ojob"),
			soci::use(cJob, "cjob"),
			soci::use(oOverall, "ooverall"),
			soci::use(cOverall, "coverall"));

		for (const auto &p : v) {
			charId = p.charId;
			oFame = p.fame.oldRank;
			cFame = p.fame.newRank.get();
			oWorld = p.world.oldRank;
			cWorld = p.world.newRank.get();
			oJob = p.job.oldRank;
			cJob = p.job.newRank.get();
			oOverall = p.overall.oldRank;
			cOverall = p.overall.newRank.get();
			st.execute(true);
		}
	}

	LoginServer::getInstance().log(LogType::Info, [&](out_stream_t &str) {
		str << "Calculating rankings completed in " << std::setprecision(3) << sw.elapsed<milliseconds_t>() / 1000.f << " seconds!";
	});

	l.unlock();
}

auto RankingCalculator::increaseRank(player_level_t level, player_level_t maxLevel, player_level_t lastLevel, experience_t exp, experience_t lastExp, job_id_t job) -> bool {
	if (level == maxLevel) {
		return true;
	}
	else if (lastLevel != level) {
		return true;
	}
	else if (lastExp != exp) {
		return true;
	}
	// Level time only matters for level 200/120 Cygnus (taken care of in the first case)
	return false;
}

auto RankingCalculator::baseCompare(const RankPlayer &t1, const RankPlayer &t2) -> bool {
	if (t1.levelStat == t2.levelStat) {
		if (t1.expStat == t2.expStat) {
			return t1.levelTime < t2.levelTime;
		}
		return t1.expStat > t2.expStat;
	}
	return t1.levelStat > t2.levelStat;
}

auto RankingCalculator::updateRank(Rank &r, int32_t newRank) -> void {
	r.oldRank = r.newRank;
	r.newRank = newRank;
}

auto RankingCalculator::overall(vector_t<RankPlayer> &v) -> void {
	std::sort(std::begin(v), std::end(v), &baseCompare);

	player_level_t lastLevel = 0;
	time_t lastTime = 0;
	experience_t lastExp = 0;
	bool first = true;
	size_t rank = 1;

	for (size_t i = 0; i < v.size(); ++i) {
		RankPlayer &p = v[i];

		if (!first && increaseRank(p.levelStat, p.jobLevelMax, lastLevel, p.expStat, lastExp, p.jobStat)) {
			++rank;
		}

		updateRank(p.overall, rank);

		first = false;
		lastLevel = p.levelStat;
		lastExp = p.expStat;
		lastTime = p.levelTime;
	}
}

auto RankingCalculator::world(vector_t<RankPlayer> &v) -> void {
	std::sort(std::begin(v), std::end(v), [](const RankPlayer &t1, const RankPlayer &t2) -> bool {
		if (t1.worldId == t2.worldId) {
			return baseCompare(t1, t2);
		}
		return t1.worldId > t2.worldId;
	});

	LoginServer::getInstance().getWorlds().runFunction([&v](World *world) -> bool {
		optional_t<world_id_t> worldId = world->getId();
		if (!worldId.is_initialized()) {
			throw CodePathInvalidException{"!worldId.is_initialized()"};
		}

		player_level_t lastLevel = 0;
		time_t lastTime = 0;
		experience_t lastExp = 0;
		bool first = true;
		size_t rank = 1;
		world_id_t cached = worldId.get();

		for (size_t i = 0; i < v.size(); ++i) {
			RankPlayer &p = v[i];
			if (p.worldId != cached) {
				continue;
			}

			if (!first && increaseRank(p.levelStat, p.jobLevelMax, lastLevel, p.expStat, lastExp, p.jobStat)) {
				++rank;
			}

			updateRank(p.world, rank);

			first = false;
			lastLevel = p.levelStat;
			lastExp = p.expStat;
			lastTime = p.levelTime;
		}
		return false;
	});
}

auto RankingCalculator::job(vector_t<RankPlayer> &v) -> void {
	std::sort(std::begin(v), std::end(v), [](const RankPlayer &t1, const RankPlayer &t2) -> bool {
		int8_t job1 = GameLogicUtilities::getJobTrack(t1.jobStat);
		int8_t job2 = GameLogicUtilities::getJobTrack(t2.jobStat);

		if (job1 == job2) {
			return baseCompare(t1, t2);
		}
		return job1 > job2;
	});

	// We will iterate through each job track
	for (const auto &jobTrack : Jobs::JobTracks::JobTracks) {
		player_level_t lastLevel = 0;
		time_t lastTime = 0;
		experience_t lastExp = 0;
		bool first = true;
		size_t rank = 1;

		for (size_t i = 0; i < v.size(); ++i) {
			RankPlayer &p = v[i];
			bool valid = false;
			bool isTrack = GameLogicUtilities::getJobTrack(p.jobStat) == jobTrack;

			// These exceptions have beginner jobs that are not in their tracks ID-wise
			// Which means we also need to account for them within the tracks they aren't supposed to be in as well
			switch (jobTrack) {
				case Jobs::JobTracks::Legend: valid = (p.jobStat != Jobs::JobIds::Evan && p.jobStat != Jobs::JobIds::Mercedes && isTrack); break;
				case Jobs::JobTracks::Evan: valid = (p.jobStat == Jobs::JobIds::Evan || isTrack); break;
				case Jobs::JobTracks::Mercedes: valid = (p.jobStat == Jobs::JobIds::Mercedes || isTrack); break;
				case Jobs::JobTracks::Citizen: valid = (p.jobStat != Jobs::JobIds::DemonSlayer && isTrack); break;
				case Jobs::JobTracks::DemonSlayer: valid = (p.jobStat == Jobs::JobIds::DemonSlayer || isTrack); break;
				default: valid = isTrack;
			}

			if (!valid) {
				continue;
			}

			if (!first && increaseRank(p.levelStat, p.jobLevelMax, lastLevel, p.expStat, lastExp, p.jobStat)) {
				++rank;
			}

			updateRank(p.job, rank);

			first = false;
			lastLevel = p.levelStat;
			lastExp = p.expStat;
			lastTime = p.levelTime;
		}
	}
}

auto RankingCalculator::fame(vector_t<RankPlayer> &v) -> void {
	std::sort(std::begin(v), std::end(v), [](const RankPlayer &t1, const RankPlayer &t2) -> bool {
		return t1.fameStat > t2.fameStat;
	});

	fame_t lastFame = 0;
	bool first = true;
	size_t rank = 1;

	for (size_t i = 0; i < v.size(); ++i) {
		RankPlayer &p = v[i];
		if (p.fameStat <= 0) {
			continue;
		}

		if (!first && lastFame != p.fameStat) {
			++rank;
		}

		updateRank(p.fame, rank);

		first = false;
		lastFame = p.fameStat;
	}
}

}
}