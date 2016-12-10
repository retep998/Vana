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
#include "ranking_calculator.hpp"
#include "common/constant/job/beginner_jobs.hpp"
#include "common/constant/job/id.hpp"
#include "common/constant/job/track.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/timer/timer.hpp"
#include "common/timer/thread.hpp"
#include "common/util/game_logic/job.hpp"
#include "common/util/stop_watch.hpp"
#include "common/util/string.hpp"
#include "common/util/time.hpp"
#include "login_server/login_server.hpp"
#include "login_server/world.hpp"
#include "login_server/worlds.hpp"
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
	//	nullptr, vana::util::time::get_distance_to_next_occurring_second_of_hour(0), hours{1});
	// Calculate ranking every 1 hour, starting on the hour
}

auto ranking_calculator::run_thread() -> void {
	// Ranking on larger servers may take a long time and we don't want that to be blocking
	// The thread_t object will be deleted immediately, but the thread will continue to run
	auto p = make_owned_ptr<std::thread>([] { ranking_calculator::all(); });
	p->detach();
}

auto ranking_calculator::all() -> void {
	// There's no guarantee what effect running two at once will have, but it's likely to be bad
	owned_lock<mutex> l{g_rankings_mutex, std::try_to_lock};
	if (!l) {
		return;
	}

	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Calculating rankings... " << std::endl;
	vana::util::stop_watch sw;

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	rank_player out;
	soci::statement statement = (sql.prepare
		<< "SELECT c.character_id, c.exp, c.fame, c.job, c.level, c.world_id, c.time_level, c.fame_cpos, c.world_cpos, c.job_cpos, c.overall_cpos "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "INNER JOIN " << db.make_table(vana::table::accounts) << " u ON u.account_id = c.account_id "
		<< "WHERE "
		<< "	(u.banned = 0 OR u.ban_expire >= NOW()) "
		<< "	AND u.gm_level IS NULL "
		<< "	AND u.admin IS NULL "
		<< "	AND ("
		<< "		("
		<< "			c.job IN (" << vana::util::str::delimit(",", constant::job::beginner_jobs) << ")"
		<< "			AND c.level > 9"
		<< "		)"
		<< "		OR c.job NOT IN (" << vana::util::str::delimit(",", constant::job::beginner_jobs) << ")"
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
		out.job_level_max = vana::util::game_logic::job::get_max_level(out.job_stat);
		v.push_back(out);
	}

	if (v.size() > 0) {
		overall(v);
		world(v);
		job(v);
		fame(v);

		game_player_id char_id = 0;
		opt_int32_t original_fame_rank = 0;
		int32_t current_fame_rank = 0;
		opt_int32_t original_world_rank = 0;
		int32_t current_world_rank = 0;
		opt_int32_t original_job_rank = 0;
		int32_t current_job_rank = 0;
		opt_int32_t original_overall_rank = 0;
		int32_t current_overall_rank = 0;

		soci::statement st = (sql.prepare
			<< "UPDATE " << db.make_table(vana::table::characters) << " "
			<< "SET "
			<< "	fame_opos = :original_fame_rank,"
			<< "	fame_cpos = :current_fame_rank,"
			<< "	world_opos = :original_world_rank,"
			<< "	world_cpos = :current_world_rank,"
			<< "	job_opos = :original_job_rank,"
			<< "	job_cpos = :current_job_rank,"
			<< "	overall_opos = :original_overall_rank,"
			<< "	overall_cpos = :current_overall_rank "
			<< "	WHERE character_id = :char",
			soci::use(char_id, "char"),
			soci::use(original_fame_rank, "original_fame_rank"),
			soci::use(current_fame_rank, "current_fame_rank"),
			soci::use(original_world_rank, "original_world_rank"),
			soci::use(current_world_rank, "current_world_rank"),
			soci::use(original_job_rank, "original_job_rank"),
			soci::use(current_job_rank, "current_job_rank"),
			soci::use(original_overall_rank, "original_overall_rank"),
			soci::use(current_overall_rank, "current_overall_rank"));

		for (const auto &p : v) {
			char_id = p.char_id;
			original_fame_rank = p.fame.old_rank;
			current_fame_rank = p.fame.new_rank.get();
			original_world_rank = p.world.old_rank;
			current_world_rank = p.world.new_rank.get();
			original_job_rank = p.job.old_rank;
			current_job_rank = p.job.new_rank.get();
			original_overall_rank = p.overall.old_rank;
			current_overall_rank = p.overall.new_rank.get();
			st.execute(true);
		}
	}

	login_server::get_instance().log(vana::log::type::info, [&](out_stream &str) {
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
			THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
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
		int8_t job1 = vana::util::game_logic::job::get_job_track(t1.job_stat);
		int8_t job2 = vana::util::game_logic::job::get_job_track(t2.job_stat);

		if (job1 == job2) {
			return base_compare(t1, t2);
		}
		return job1 > job2;
	});

	// We will iterate through each job track
	for (const auto &job_track : constant::job::track::all) {
		game_player_level last_level = 0;
		time_t last_time = 0;
		game_experience last_exp = 0;
		bool first = true;
		size_t rank = 1;

		for (size_t i = 0; i < v.size(); ++i) {
			rank_player &p = v[i];
			bool valid = false;
			bool is_track = vana::util::game_logic::job::get_job_track(p.job_stat) == job_track;

			// These exceptions have beginner jobs that are not in their tracks ID-wise
			// Which means we also need to account for them within the tracks they aren't supposed to be in as well
			switch (job_track) {
				case constant::job::track::legend: valid = (p.job_stat != constant::job::id::evan && p.job_stat != constant::job::id::mercedes && is_track); break;
				case constant::job::track::evan: valid = (p.job_stat == constant::job::id::evan || is_track); break;
				case constant::job::track::mercedes: valid = (p.job_stat == constant::job::id::mercedes || is_track); break;
				case constant::job::track::citizen: valid = (p.job_stat != constant::job::id::demon_slayer && is_track); break;
				case constant::job::track::demon_slayer: valid = (p.job_stat == constant::job::id::demon_slayer || is_track); break;
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