/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "SkillConstants.h"
#include "Timer.h"
#include "TimerThread.h"
#include "TimeUtilities.h"
#include "World.h"
#include "Worlds.h"
#include <boost/scoped_ptr.hpp>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>

using boost::scoped_ptr;
using std::tr1::bind;
using Initializing::OutputWidth;

namespace RankingCalculator {
	// Consider moving some of this garbage to MySQL stored functions, maybe not because all the constants and stuff are here...
	const string jobClause() {
		std::ostringstream ret;
		ret << "(((";
		for (int32_t i = 0; i < BeginnerJobCount; i++) {
			if (i != 0) {
				ret << " OR ";
			}
			ret << "c.job = " << BeginnerJobs[i];
		}

		ret << ") AND c.level > 9) OR (";

		for (int32_t i = 0; i < BeginnerJobCount; i++) {
			if (i != 0) {
				ret << " AND ";
			}
			ret << "c.job <> " << BeginnerJobs[i];
		}
		ret << "))";
		return ret.str();
	}
	const string rankIfClause() {
		std::ostringstream ret;
		ret << "("
			<< "	(job DIV 1000 <> 1 AND level <> " << static_cast<int16_t>(Stats::PlayerLevels) << ") "
			<< "	OR (job DIV 1000 = 1 AND level <> " << static_cast<int16_t>(Stats::CygnusLevels) << ")"
			<< ") "
			<< "AND @level = level AND @exp = exp, @rank, @real_rank + 1";
		return ret.str();
	}
	const string accountRequirements() {
		std::ostringstream ret;
		ret << "u.ban_expire < NOW()"
			<< "AND u.gm_level = 0"
			<< "AND u.admin = 0";
		return ret.str();
	}

	const int8_t JobTracks[JobTrackCount] = {
		Jobs::JobTracks::Beginner, Jobs::JobTracks::Warrior, Jobs::JobTracks::Magician, Jobs::JobTracks::Bowman, Jobs::JobTracks::Thief, Jobs::JobTracks::Pirate,
		Jobs::JobTracks::Noblesse, Jobs::JobTracks::DawnWarrior, Jobs::JobTracks::BlazeWizard, Jobs::JobTracks::WindArcher, Jobs::JobTracks::NightWalker, Jobs::JobTracks::ThunderBreaker,
		Jobs::JobTracks::Legend, Jobs::JobTracks::Aran, Jobs::JobTracks::Evan,
		Jobs::JobTracks::Citizen, Jobs::JobTracks::BattleMage, Jobs::JobTracks::WildHunter, Jobs::JobTracks::Mechanic
	};
	const int16_t BeginnerJobs[BeginnerJobCount] = {
		Jobs::JobIds::Beginner, Jobs::JobIds::Noblesse, Jobs::JobIds::Legend, Jobs::JobIds::Evan, Jobs::JobIds::Citizen
	};

	const string VariableDefinition = "SET @rank := @real_rank := 0, @level := @exp := -1";
	const string JobClause = jobClause();
	const string RankIfClause = rankIfClause();
	const string AccountRequirements = accountRequirements();
	const int16_t EvanBeginner = Jobs::JobIds::Evan;
}

// Note: Calculations are done on the MySQL server because it is faster that way
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

	overall();
	world();
	job();
	fame();

	float loadingTime = (TimeUtilities::getTickCount() - startTime) / 1000.0f;
	std::cout << "DONE in " << std::setprecision(3) << loadingTime << " seconds!" << std::endl;
}

void RankingCalculator::overall() {
	mysqlpp::Query query = Database::getCharDb().query();

	// Set the variables we're going to use later
	query << VariableDefinition;
	query.exec();

	// Calculate the rank
	query << "UPDATE characters target"
			<< "	INNER JOIN ("
			<< "		SELECT c.character_id,"
			<< "			GREATEST("
			<< "				@rank := IF(" << RankIfClause << "),"
			<< "				LEAST(0, @real_rank := @real_rank + 1),"
			<< "				LEAST(0, @level := level),"
			<< "				LEAST(0, @exp := exp)"
			<< "			) AS rank"
			<< "		FROM characters c"
			<< "		LEFT JOIN user_accounts u ON u.user_id = c.user_id"
			<< "		WHERE "
			<< "			" << AccountRequirements
			<< "			AND " << JobClause
			<< "		ORDER BY"
			<< "			c.level DESC,"
			<< "			c.exp DESC,"
			<< "			c.time_level ASC"
			<< "	) AS source ON source.character_id = target.character_id"
			<< "	SET"
			<< "		target.overall_opos = target.overall_cpos,"
			<< "		target.overall_cpos = source.rank";
	query.exec();
}

namespace Functors {
	struct RankingFunctor {
		bool operator()(World *world) {
			mysqlpp::Query query = Database::getCharDb().query();

			query << RankingCalculator::VariableDefinition;
			query.exec();

			query << "UPDATE characters target"
				<< "	INNER JOIN ("
				<< "		SELECT c.character_id,"
				<< "			GREATEST("
				<< "				@rank := IF(" << RankingCalculator::RankIfClause << "),"
				<< "				LEAST(0, @real_rank := @real_rank + 1),"
				<< "				LEAST(0, @level := level),"
				<< "				LEAST(0, @exp := exp)"
				<< "			) AS rank"
				<< "		FROM characters c"
				<< "		LEFT JOIN user_accounts u ON u.user_id = c.user_id"
				<< "		WHERE "
				<< "			" << RankingCalculator::AccountRequirements
				<< "			AND " << RankingCalculator::JobClause
				<< "			AND world_id = " << static_cast<int32_t>(world->getId())
				<< "		ORDER BY"
				<< "			c.level DESC,"
				<< "			c.exp DESC,"
				<< "			c.time_level ASC"
				<< "	) AS source ON source.character_id = target.character_id"
				<< "	SET"
				<< "		target.world_opos = target.world_cpos,"
				<< "		target.world_cpos = source.rank";
			query.exec();
			return false;
		}
	};
}

void RankingCalculator::world() {
	Functors::RankingFunctor func = {};
	Worlds::Instance()->runFunction(func);
}

void RankingCalculator::job() {
	mysqlpp::Query query = Database::getCharDb().query();

	// We will iterate through each job track
	for (int32_t j = 0; j < JobTrackCount; j++) {
		const int16_t jobTrack = JobTracks[j];

		// Set the variables we're going to use later
		query << VariableDefinition;
		query.exec();

		// Calculate the rank
		query << "UPDATE characters target"
				<< "	INNER JOIN ("
				<< "		SELECT c.character_id,"
				<< "			GREATEST("
				<< "				@rank := IF(" << RankIfClause << "),"
				<< "				LEAST(0, @real_rank := @real_rank + 1),"
				<< "				LEAST(0, @level := level),"
				<< "				LEAST(0, @exp := exp)"
				<< "			) AS rank"
				<< "		FROM characters c"
				<< "		LEFT JOIN user_accounts u ON u.user_id = c.user_id"
				<< "		WHERE "
				<< "			" << AccountRequirements
				<< "			AND " << JobClause;

		switch (jobTrack) {
			case Jobs::JobTracks::Evan: query << "			AND (job = " << EvanBeginner << " OR job DIV 100 = " << jobTrack << ")"; break;
			case Jobs::JobTracks::Legend: query << "			AND (job <> " << EvanBeginner << " AND job DIV 100 = " << jobTrack << ")"; break;
			default: query << "			AND job DIV 100 = " << jobTrack;
		}

		query << "		ORDER BY"
				<< "			c.level DESC,"
				<< "			c.exp DESC,"
				<< "			c.time_level ASC"
				<< "	) AS source ON source.character_id = target.character_id"
				<< "	SET"
				<< "		target.job_opos = target.job_cpos,"
				<< "		target.job_cpos = source.rank";
		query.exec();
	}
}

void RankingCalculator::fame() {
	mysqlpp::Query query = Database::getCharDb().query();

	// Set the variables we're going to use later
	query << "SET @rank := 0";
	query.exec();

	// Calculate the rank
	query << "UPDATE characters target"
			<< "	INNER JOIN ("
			<< "		SELECT c.character_id, (@rank := @rank + 1) AS rank FROM characters c"
			<< "		LEFT JOIN user_accounts u ON u.user_id = c.user_id"
			<< "		WHERE "
			<< "			" << AccountRequirements
			<< "			AND " << JobClause
			<< "			AND fame > 0"
			<< "		ORDER BY"
			<< "			c.fame DESC,"
			<< "			c.level DESC,"
			<< "			c.exp DESC,"
			<< "			c.time_level ASC"
			<< "	) AS source ON source.character_id = target.character_id"
			<< "	SET"
			<< "		target.fame_opos = target.fame_cpos,"
			<< "		target.fame_cpos = source.rank";
	query.exec();
}