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
#include "common/Database.hpp"
#include "common/GameConstants.hpp"
#include "common/GameLogicUtilities.hpp"
#include "common/InitializeCommon.hpp"
#include "common/JobConstants.hpp"
#include "common/StopWatch.hpp"
#include "common/StringUtilities.hpp"
#include "common/Timer.hpp"
#include "common/TimerThread.hpp"
#include "common/TimeUtilities.hpp"
#include "login_server/LoginServer.hpp"
#include "login_server/World.hpp"
#include "login_server/Worlds.hpp"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

namespace vana {
namespace login_server {

mutex ranking_calculator::g_rankings_mutex;

auto ranking_calculator::set_timer() -> void {
	//timer::timer::create([]() { ranking_calculator::run_thread(); },
	//	timer::id{timer_type::rank_timer),
	//	nullptr, utilities::time::get_distance_to_next_occurring_second_of_hour(0), hours{1});
	// Calculate ranking every 1 hour, starting on the hour
}

auto ranking_calculator::run_thread() -> void {
	// Ranking on larger servers may take a long time and we don't want that to be blocking
	// The thread_t object will be deleted immediately, but the thread will continue to run
	auto p = make_owned_ptr<thread>([] { ranking_calculator::all(); });
	p->detach();
}

auto ranking_calculator::all() -> void {
	// There's no guarantee what effect running two at once will have, but it's likely to be bad
	owned_lock<mutex> l{g_rankings_mutex, std::try_to_lock};
	if (!l) {
		return;
	}

	std::cout << std::setw(initializing::output_width) << std::left << "Calculating rankings... " << std::endl;
	stop_watch sw;

	auto &db = database::get_char_db();
	auto &sql = db.get_session();
	rank_player out;
	soci::statement statement = (sql.prepare
		<< "SELECT c.character_id, c.exp, c.fame, c.job, c.level, c.world_id, c.time_level, c.fame_cpos, c.world_cpos, c.job_cpos, c.overall_cpos "
		<< "FROM " << db.make_table("characters") << " c "
		<< "INNER JOIN " << db.make_table("accounts") << " u ON u.account_id = c.account_id "
		<< "WHERE "
		<< "	(u.banned = 0 OR u.ban_expire >= NOW()) "
		<< "	AND u.gm_level IS NULL "
		<< "	AND u.admin IS NULL "
		<< "	AND ("
		<< "		("
		<< "			c.job IN (" << utilities::str::delimit(",", jobs::beginners::jobs) << ")"
		<< "			AND c.level > 9"
		<< "		)"
		<< "		OR c.job NOT IN (" << utilities::str::delimit(",", jobs::beginners::jobs) << ")"
		<< "	) "
		<< "ORDER BY c.overall_cpos DESC",
		soci::into(out.char_id),
		soci::into(out.exp_stat),
		soci::into(out.fame_stat),
		soci::into(out.job_stat),
		soci::into(out.level_stat),
		soci::into(out.world_id),
		soci::into(out.level_time),
		soci::into(out.fame.new_rank),
		soci::into(out.world.new_rank),
		soci::into(out.job.new_rank),
		soci::into(out.overall.new_rank));

	vector<rank_player> v;
	statement.execute();

	while (statement.fetch()) {
		out.job_level_max = game_logic_utilities::get_max_level(out.job_stat);
		v.push_back(out);
	}

	if (v.size() > 0) {
		overall(v);
		world(v);
		job(v);
		fame(v);

		game_player_id char_id = 0;
		opt_int32_t o_fame = 0;
		int32_t c_fame = 0;
		opt_int32_t o_world = 0;
		int32_t c_world = 0;
		opt_int32_t o_job = 0;
		int32_t c_job = 0;
		opt_int32_t o_overall = 0;
		int32_t c_overall = 0;

		soci::statement st = (sql.prepare
			<< "UPDATE " << db.make_table("characters") << " "
			<< "SET "
			<< "	fame_opos = :o_fame,"
			<< "	fame_cpos = :c_fame,"
			<< "	world_opos = :o_world,"
			<< "	world_cpos = :c_world,"
			<< "	job_opos = :o_job,"
			<< "	job_cpos = :c_job,"
			<< "	overall_opos = :o_overall,"
			<< "	overall_cpos = :c_overall "
			<< "	WHERE character_id = :char",
			soci::use(char_id, "char"),
			soci::use(o_fame, "o_fame"),
			soci::use(c_fame, "c_fame"),
			soci::use(o_world, "o_world"),
			soci::use(c_world, "c_world"),
			soci::use(o_job, "o_job"),
			soci::use(c_job, "c_job"),
			soci::use(o_overall, "o_overall"),
			soci::use(c_overall, "c_overall"));

		for (const auto &p : v) {
			char_id = p.char_id;
			o_fame = p.fame.old_rank;
			c_fame = p.fame.new_rank.get();
			o_world = p.world.old_rank;
			c_world = p.world.new_rank.get();
			o_job = p.job.old_rank;
			c_job = p.job.new_rank.get();
			o_overall = p.overall.old_rank;
			c_overall = p.overall.new_rank.get();
			st.execute(true);
		}
	}

	login_server::get_instance().log(log_type::info, [&](out_stream &str) {
		str << "Calculating rankings completed in " << std::setprecision(3) << sw.elapsed<milliseconds>() / 1000.f << " seconds!";
	});

	l.unlock();
}

auto ranking_calculator::increase_rank(game_player_level level, game_player_level max_level, game_player_level last_level, game_experience exp, game_experience last_exp, game_job_id job) -> bool {
	if (level == max_level) {
		return true;
	}
	else if (last_level != level) {
		return true;
	}
	else if (last_exp != exp) {
		return true;
	}
	// Level time only matters for level 200/120 Cygnus (taken care of in the first case)
	return false;
}

auto ranking_calculator::base_compare(const rank_player &t1, const rank_player &t2) -> bool {
	if (t1.level_stat == t2.level_stat) {
		if (t1.exp_stat == t2.exp_stat) {
			return t1.level_time < t2.level_time;
		}
		return t1.exp_stat > t2.exp_stat;
	}
	return t1.level_stat > t2.level_stat;
}

auto ranking_calculator::update_rank(rank &r, int32_t new_rank) -> void {
	r.old_rank = r.new_rank;
	r.new_rank = new_rank;
}

auto ranking_calculator::overall(vector<rank_player> &v) -> void {
	std::sort(std::begin(v), std::end(v), &base_compare);

	game_player_level last_level = 0;
	time_t last_time = 0;
	game_experience last_exp = 0;
	bool first = true;
	size_t rank = 1;

	for (size_t i = 0; i < v.size(); ++i) {
		rank_player &p = v[i];

		if (!first && increase_rank(p.level_stat, p.job_level_max, last_level, p.exp_stat, last_exp, p.job_stat)) {
			++rank;
		}

		update_rank(p.overall, rank);

		first = false;
		last_level = p.level_stat;
		last_exp = p.exp_stat;
		last_time = p.level_time;
	}
}

auto ranking_calculator::world(vector<rank_player> &v) -> void {
	std::sort(std::begin(v), std::end(v), [](const rank_player &t1, const rank_player &t2) -> bool {
		if (t1.world_id == t2.world_id) {
			return base_compare(t1, t2);
		}
		return t1.world_id > t2.world_id;
	});

	login_server::get_instance().get_worlds().run_function([&v](vana::login_server::world *world_value) -> bool {
		optional<game_world_id> world_id = world_value->get_id();
		if (!world_id.is_initialized()) {
			throw codepath_invalid_exception{"!world_id.is_initialized()"};
		}

		game_player_level last_level = 0;
		time_t last_time = 0;
		game_experience last_exp = 0;
		bool first = true;
		size_t rank = 1;
		game_world_id cached = world_id.get();

		for (size_t i = 0; i < v.size(); ++i) {
			rank_player &p = v[i];
			if (p.world_id != cached) {
				continue;
			}

			if (!first && increase_rank(p.level_stat, p.job_level_max, last_level, p.exp_stat, last_exp, p.job_stat)) {
				++rank;
			}

			update_rank(p.world, rank);

			first = false;
			last_level = p.level_stat;
			last_exp = p.exp_stat;
			last_time = p.level_time;
		}
		return false;
	});
}

auto ranking_calculator::job(vector<rank_player> &v) -> void {
	std::sort(std::begin(v), std::end(v), [](const rank_player &t1, const rank_player &t2) -> bool {
		int8_t job1 = game_logic_utilities::get_job_track(t1.job_stat);
		int8_t job2 = game_logic_utilities::get_job_track(t2.job_stat);

		if (job1 == job2) {
			return base_compare(t1, t2);
		}
		return job1 > job2;
	});

	// We will iterate through each job track
	for (const auto &job_track : jobs::job_tracks::job_tracks) {
		game_player_level last_level = 0;
		time_t last_time = 0;
		game_experience last_exp = 0;
		bool first = true;
		size_t rank = 1;

		for (size_t i = 0; i < v.size(); ++i) {
			rank_player &p = v[i];
			bool valid = false;
			bool is_track = game_logic_utilities::get_job_track(p.job_stat) == job_track;

			// These exceptions have beginner jobs that are not in their tracks ID-wise
			// Which means we also need to account for them within the tracks they aren't supposed to be in as well
			switch (job_track) {
				case jobs::job_tracks::legend: valid = (p.job_stat != jobs::job_ids::evan && p.job_stat != jobs::job_ids::mercedes && is_track); break;
				case jobs::job_tracks::evan: valid = (p.job_stat == jobs::job_ids::evan || is_track); break;
				case jobs::job_tracks::mercedes: valid = (p.job_stat == jobs::job_ids::mercedes || is_track); break;
				case jobs::job_tracks::citizen: valid = (p.job_stat != jobs::job_ids::demon_slayer && is_track); break;
				case jobs::job_tracks::demon_slayer: valid = (p.job_stat == jobs::job_ids::demon_slayer || is_track); break;
				default: valid = is_track;
			}

			if (!valid) {
				continue;
			}

			if (!first && increase_rank(p.level_stat, p.job_level_max, last_level, p.exp_stat, last_exp, p.job_stat)) {
				++rank;
			}

			update_rank(p.job, rank);

			first = false;
			last_level = p.level_stat;
			last_exp = p.exp_stat;
			last_time = p.level_time;
		}
	}
}

auto ranking_calculator::fame(vector<rank_player> &v) -> void {
	std::sort(std::begin(v), std::end(v), [](const rank_player &t1, const rank_player &t2) -> bool {
		return t1.fame_stat > t2.fame_stat;
	});

	game_fame last_fame = 0;
	bool first = true;
	size_t rank = 1;

	for (size_t i = 0; i < v.size(); ++i) {
		rank_player &p = v[i];
		if (p.fame_stat <= 0) {
			continue;
		}

		if (!first && last_fame != p.fame_stat) {
			++rank;
		}

		update_rank(p.fame, rank);

		first = false;
		last_fame = p.fame_stat;
	}
}

}
}