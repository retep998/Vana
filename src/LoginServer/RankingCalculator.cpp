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
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include "Timer/Thread.h"
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

// Note: calculations are done on the MySQL server because it is faster that way

void RankingCalculator::setTimer() {
	new Timer::Timer(RankingCalculator::runThread, 
		Timer::Id(Timer::Types::RankTimer, 0, 0), 0, Timer::Time::nthSecondOfHour(0), 3600000);
	// Calculate ranking every 1 hour, starting on the hour
}

void RankingCalculator::runThread() {
	// Ranking on larger servers may take a long time and we don't want that to be blocking
	// The boost::thread object will be deleted immediately, but the thread will continue to run
	scoped_ptr<boost::thread>(new boost::thread(bind(&RankingCalculator::all)));
}

void RankingCalculator::all() {
	std::cout << std::setw(outputWidth) << std::left << "Calculating rankings... ";
	clock_t startTime = TimeUtilities::getTickCount();

	RankingCalculator::overall();
	RankingCalculator::world();
	RankingCalculator::job();
	RankingCalculator::fame();

	float loadingTime = (TimeUtilities::getTickCount() - startTime) / (float) 1000;
	std::cout << "DONE in " << std::setprecision(3) << loadingTime << " seconds!" << std::endl;
}

void RankingCalculator::overall() {
	mysqlpp::Query query = Database::getCharDB().query();

	// Set the variables we're going to use later
	query << "SET @rank := @real_rank := 0, @level := @exp := -1";
	query.exec();

	// Calculate the rank
	query << "UPDATE characters target"
			<< "	INNER JOIN ("
			<< "		SELECT c.id,"
			<< "			GREATEST("
			<< "				@rank := IF(level <> 200 AND @level = level AND @exp = exp, @rank, @real_rank + 1),"
			<< "				LEAST(0, @real_rank := @real_rank + 1),"
			<< "				LEAST(0, @level := level),"
			<< "				LEAST(0, @exp := exp)"
			<< "			) AS rank"
			<< "		FROM characters c"
			<< "		LEFT JOIN users u ON u.id = c.userid"
			<< "		WHERE "
			<< "			u.ban_expire < NOW()"
			<< "			AND u.gm = 0"
			<< "			AND ((c.job = 0 AND c.level > 9) OR (c.job != 0))"
			<< "		ORDER BY"
			<< "			c.level DESC,"
			<< "			c.exp DESC,"
			<< "			c.time_level ASC"
			<< "	) AS source ON source.id = target.id"
			<< "	SET"
			<< "		target.overall_opos = target.overall_cpos,"
			<< "		target.overall_cpos = source.rank";
	query.exec();
}

namespace Functors {
	struct RankingFunctor {
		bool operator()(World *world) {
			mysqlpp::Query query = Database::getCharDB().query();

			query << "SET @rank := @real_rank := 0, @level := @exp := -1";
			query.exec();

			query << "UPDATE characters target"
				<< "	INNER JOIN ("
				<< "		SELECT c.id,"
				<< "			GREATEST("
				<< "				@rank := IF(level <> 200 AND @level = level AND @exp = exp, @rank, @real_rank + 1),"
				<< "				LEAST(0, @real_rank := @real_rank + 1),"
				<< "				LEAST(0, @level := level),"
				<< "				LEAST(0, @exp := exp)"
				<< "			) AS rank"
				<< "		FROM characters c"
				<< "		LEFT JOIN users u ON u.id = c.userid"
				<< "		WHERE "
				<< "			u.ban_expire < NOW()"
				<< "			AND u.gm = 0"
				<< "			AND ((c.job = 0 AND c.level > 9) OR (c.job != 0))"
				<< "			AND world_id = " << (int32_t) world->getId()
				<< "		ORDER BY"
				<< "			c.level DESC,"
				<< "			c.exp DESC,"
				<< "			c.time_level ASC"
				<< "	) AS source ON source.id = target.id"
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
	mysqlpp::Query query = Database::getCharDB().query();

	// We will iterate through each job class
	for (uint8_t j = 0; j < 6; j++) {
		// Set the variables we're going to use later
		query << "SET @rank := @real_rank := 0, @level := @exp := -1";
		query.exec();

		// Calculate the rank
		query << "UPDATE characters target"
				<< "	INNER JOIN ("
				<< "		SELECT c.id,"
				<< "			GREATEST("
				<< "				@rank := IF(level <> 200 AND @level = level AND @exp = exp, @rank, @real_rank + 1),"
				<< "				LEAST(0, @real_rank := @real_rank + 1),"
				<< "				LEAST(0, @level := level),"
				<< "				LEAST(0, @exp := exp)"
				<< "			) AS rank"
				<< "		FROM characters c"
				<< "		LEFT JOIN users u ON u.id = c.userid"
				<< "		WHERE "
				<< "			u.ban_expire < NOW()"
				<< "			AND u.gm = 0"
				<< "			AND ((c.job = 0 AND c.level > 9) OR (c.job != 0))"
				<< "			AND (job DIV 100) = " << (int32_t) j
				<< "		ORDER BY"
				<< "			c.level DESC,"
				<< "			c.exp DESC,"
				<< "			c.time_level ASC"
				<< "	) AS source ON source.id = target.id"
				<< "	SET"
				<< "		target.job_opos = target.job_cpos,"
				<< "		target.job_cpos = source.rank";
		query.exec();
	}
}

void RankingCalculator::fame() {
	mysqlpp::Query query = Database::getCharDB().query();

	// Set the variables we're going to use later
	query << "SET @rank := 0";
	query.exec();

	// Calculate the rank
	query << "UPDATE characters target"
			<< "	INNER JOIN ("
			<< "		SELECT c.id, (@rank := @rank + 1) AS rank FROM characters c"
			<< "		LEFT JOIN users u ON u.id = c.userid"
			<< "		WHERE "
			<< "			u.ban_expire < NOW()"
			<< "			AND u.gm = 0"
			<< "			AND ((c.job = 0 AND c.level > 9) OR (c.job != 0))"
			<< "			AND fame > 0"
			<< "		ORDER BY"
			<< "			c.fame DESC,"
			<< "			c.level DESC,"
			<< "			c.exp DESC,"
			<< "			c.time_level ASC"
			<< "	) AS source ON source.id = target.id"
			<< "	SET"
			<< "		target.fame_opos = target.fame_cpos,"
			<< "		target.fame_cpos = source.rank";
	query.exec();
}
