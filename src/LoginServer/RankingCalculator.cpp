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
#include "RankingCalculator.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "SkillConstants.h"
#include "Timer.h"
#include "TimerThread.h"
#include "TimeUtilities.h"
#include "World.h"
#include "Worlds.h"
#include <algorithm>
#include <boost/scoped_ptr.hpp>
#include <functional>
#include <iomanip>
#include <iostream>

using boost::scoped_ptr;
using std::tr1::bind;
using Initializing::OutputWidth;

namespace RankingCalculator {
	const int8_t JobTracks[JobTrackCount] = {
		Jobs::JobTracks::Beginner, Jobs::JobTracks::Warrior, Jobs::JobTracks::Magician, Jobs::JobTracks::Bowman, Jobs::JobTracks::Thief, Jobs::JobTracks::Pirate,
		Jobs::JobTracks::Noblesse, Jobs::JobTracks::DawnWarrior, Jobs::JobTracks::BlazeWizard, Jobs::JobTracks::WindArcher, Jobs::JobTracks::NightWalker, Jobs::JobTracks::ThunderBreaker,
		Jobs::JobTracks::Legend, Jobs::JobTracks::Aran, Jobs::JobTracks::Evan, Jobs::JobTracks::Mercedes,
		Jobs::JobTracks::Citizen, Jobs::JobTracks::DemonSlayer, Jobs::JobTracks::BattleMage, Jobs::JobTracks::WildHunter, Jobs::JobTracks::Mechanic
	};
	const int16_t BeginnerJobs[BeginnerJobCount] = {
		Jobs::JobIds::Beginner,
		Jobs::JobIds::Noblesse,
		Jobs::JobIds::Legend, Jobs::JobIds::Evan, Jobs::JobIds::Mercedes,
		Jobs::JobIds::Citizen, Jobs::JobIds::DemonSlayer
	};

	const int16_t EvanBeginner = Jobs::JobIds::Evan;
	const int16_t MercedesBeginner = Jobs::JobIds::Mercedes;
	const int16_t DemonSlayerBeginner = Jobs::JobIds::DemonSlayer;
}

void RankingCalculator::setTimer() {
	new Timer::Timer(RankingCalculator::runThread,
		Timer::Id(Timer::Types::RankTimer, 0, 0), nullptr, TimeUtilities::nthSecondOfHour(0), 60 * 60 * 1000);
	// Calculate ranking every 1 hour, starting on the hour
}

void RankingCalculator::runThread() {
	// Ranking on larger servers may take a long time and we don't want that to be blocking
	// The boost::thread object will be deleted immediately, but the thread will continue to run
	scoped_ptr<boost::thread>(new boost::thread(bind(&RankingCalculator::all)));
}

void RankingCalculator::all() {
	std::cout << std::setw(OutputWidth) << std::left << "Calculating rankings... ";
	clock_t startTime = TimeUtilities::getTickCount();
	auto func = [](const string &connector, const string &operation) -> string {
		std::ostringstream q("");
		for (int32_t i = 0; i < BeginnerJobCount; ++i) {
			if (i != 0) {
				q << connector;
			}
			q << "c.job " << operation << " " << BeginnerJobs[i];
		}
		return q.str();
	};

	soci::session &sql = Database::getCharDb();
	soci::rowset<> rs = (sql.prepare << "SELECT * " <<
										"FROM characters c " <<
										"INNER JOIN user_accounts u ON u.user_id = c.user_id " <<
										"WHERE " <<
										"	(u.ban_expire IS NULL OR u.ban_expire = 0 OR u.ban_expire >= NOW()) " <<
										"	AND u.gm_level = 0 " <<
										"	AND u.admin = 0 " <<
										"	AND (" <<
										"		(" <<
										"			(" << func(" OR ", "=") << ")" <<
										"			AND c.level > 9" <<
										"		)" <<
										"		OR (" << func(" AND ", "<>") << ")" <<
										"	) " <<
										"ORDER BY c.overall_cpos DESC");

	vector<RankPlayer> v;
	RankPlayer out;
	auto maxLevel = [](int16_t job) -> uint8_t {
		bool c = GameLogicUtilities::isCygnus(job);
		return (c ? Stats::CygnusLevels : Stats::PlayerLevels);
	};
	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		out = RankPlayer();
		out.charId = row.get<int32_t>("character_id");
		out.expStat = row.get<int32_t>("exp");
		out.fameStat = row.get<int16_t>("fame");
		out.jobStat = row.get<int16_t>("job");
		out.jobLevelMax = maxLevel(out.jobStat);
		out.levelStat = row.get<uint8_t>("level");
		out.worldId = row.get<uint8_t>("world_id");
		out.levelTime = row.get<unix_time_t>("time_level");
		out.fame.newRank = row.get<int32_t>("fame_cpos");
		out.fame.oldRank = row.get<int32_t>("fame_opos");
		out.world.newRank = row.get<int32_t>("world_cpos");
		out.world.oldRank = row.get<int32_t>("world_opos");
		out.job.newRank = row.get<int32_t>("job_cpos");
		out.job.oldRank = row.get<int32_t>("job_opos");
		out.overall.newRank = row.get<int32_t>("overall_cpos");
		out.overall.oldRank = row.get<int32_t>("overall_opos");
		v.push_back(out);
	}

	if (v.size() > 0) {
		overall(v);
		world(v);
		job(v);
		fame(v);

		int32_t charId = 0;
		int32_t oFame = 0;
		int32_t cFame = 0;
		int32_t oWorld = 0;
		int32_t cWorld = 0;
		int32_t oJob = 0;
		int32_t cJob = 0;
		int32_t oOverall = 0;
		int32_t cOverall = 0;

		soci::statement st = (sql.prepare << "UPDATE characters SET " <<
												"fame_opos = :ofame," <<
												"fame_cpos = :cfame," <<
												"world_opos = :oworld," <<
												"world_cpos = :cworld," <<
												"job_opos = :ojob," <<
												"job_cpos = :cjob," <<
												"overall_opos = :ooverall," <<
												"overall_cpos = :coverall " <<
												"WHERE character_id = :char",
												soci::use(charId, "char"),
												soci::use(oFame, "ofame"),
												soci::use(cFame, "cfame"),
												soci::use(oWorld, "oworld"),
												soci::use(cWorld, "cworld"),
												soci::use(oJob, "ojob"),
												soci::use(cJob, "cjob"),
												soci::use(oOverall, "ooverall"),
												soci::use(cOverall, "coverall"));

		for (vector<RankPlayer>::const_iterator iter = v.begin(); iter != v.end(); ++iter) {
			const RankPlayer &p = *iter;
			charId = p.charId;
			oFame = p.fame.oldRank;
			cFame = p.fame.newRank;
			oWorld = p.world.oldRank;
			cWorld = p.world.newRank;
			oJob = p.job.oldRank;
			cJob = p.job.newRank;
			oOverall = p.overall.oldRank;
			cOverall = p.overall.newRank;
			st.execute(true);
		}
	}

	float loadingTime = (TimeUtilities::getTickCount() - startTime) / 1000.0f;
	std::cout << "DONE in " << std::setprecision(3) << loadingTime << " seconds!" << std::endl;
}

bool RankingCalculator::increaseRank(uint8_t level, uint8_t maxLevel, uint8_t lastLevel, int32_t exp, int32_t lastExp, int16_t job) {
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

bool RankingCalculator::baseCompare(const RankPlayer &t1, const RankPlayer &t2) {
	if (t1.levelStat == t2.levelStat) {
		if (t1.expStat == t2.expStat) {
			return t1.levelTime < t2.levelTime;
		}
		return t1.expStat > t2.expStat;
	}
	return t1.levelStat > t2.levelStat;
}

void RankingCalculator::updateRank(Rank &r, int32_t newRank) {
	r.oldRank = r.newRank;
	r.newRank = newRank;
}

void RankingCalculator::overall(vector<RankPlayer> &v) {
	std::sort(v.begin(), v.end(), &baseCompare);

	uint8_t lastLevel = 0;
	time_t lastTime = 0;
	int32_t lastExp = 0;
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

void RankingCalculator::world(vector<RankPlayer> &v) {
	std::sort(v.begin(), v.end(), [](const RankPlayer &t1, const RankPlayer &t2) -> bool {
		if (t1.worldId == t2.worldId) {
			return baseCompare(t1, t2);
		}
		return t1.worldId > t2.worldId;
	});

	int i = 0;
 	Worlds::Instance()->runFunction([&v](World *world) -> bool {
		uint8_t worldId = world->getId();
		uint8_t lastLevel = 0;
		time_t lastTime = 0;
		int32_t lastExp = 0;
		bool first = true;
		size_t rank = 1;

		for (size_t i = 0; i < v.size(); ++i) {
			RankPlayer &p = v[i];
			if (p.worldId != worldId) {
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

void RankingCalculator::job(vector<RankPlayer> &v) {
	std::sort(v.begin(), v.end(), [](const RankPlayer &t1, const RankPlayer &t2) -> bool {
		int16_t job1 = GameLogicUtilities::getJobTrack(t1.jobStat);
		int16_t job2 = GameLogicUtilities::getJobTrack(t2.jobStat);

		if (job1 == job2) {
			return baseCompare(t1, t2);
		}
		return job1 > job2;
	});

	// We will iterate through each job track
	for (int32_t j = 0; j < JobTrackCount; ++j) {
		const int16_t jobTrack = JobTracks[j];
		uint8_t lastLevel = 0;
		time_t lastTime = 0;
		int32_t lastExp = 0;
		bool first = true;
		size_t rank = 1;

		for (size_t i = 0; i < v.size(); ++i) {
			RankPlayer &p = v[i];
			bool valid = false;
			bool isTrack = GameLogicUtilities::getJobTrack(p.jobStat) == jobTrack;

			// These exceptions have beginner jobs that are not in their tracks ID-wise
			// Which means we also need to account for them within the tracks they aren't supposed to be in as well
			switch (jobTrack) {
				case Jobs::JobTracks::Legend: valid = (p.jobStat != EvanBeginner && p.jobStat != MercedesBeginner && isTrack); break;
				case Jobs::JobTracks::Evan: valid = (p.jobStat == EvanBeginner || isTrack); break;
				case Jobs::JobTracks::Mercedes: valid = (p.jobStat == MercedesBeginner || isTrack); break;
				case Jobs::JobTracks::Citizen: valid = (p.jobStat != DemonSlayerBeginner && isTrack); break;
				case Jobs::JobTracks::DemonSlayer: valid = (p.jobStat == DemonSlayerBeginner || isTrack); break;
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

void RankingCalculator::fame(vector<RankPlayer> &v) {
	std::sort(v.begin(), v.end(), [](const RankPlayer &t1, const RankPlayer &t2) -> bool {
		return t1.fameStat > t2.fameStat;
	});

	int16_t lastFame = 0;
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