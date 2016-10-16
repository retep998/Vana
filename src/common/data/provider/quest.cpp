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
#include "quest.hpp"
#include "common/algorithm.hpp"
#include "common/constant/job/id.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/quest.hpp"
#include "common/util/game_logic/player.hpp"
#include "common/util/string.hpp"
#include <initializer_list>
#include <iomanip>
#include <iostream>

namespace vana {
namespace data {
namespace provider {

auto quest::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Quests... ";

	load_quest_data();
	load_requests();
	load_required_jobs();
	load_rewards();

	std::cout << "DONE" << std::endl;
}

auto quest::load_quest_data() -> void {
	m_quests.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::quest_data));

	for (const auto &row : rs) {
		vana::quest quest;
		game_quest_id quest_id = row.get<game_quest_id>("questid");

		quest.set_next_quest(row.get<game_quest_id>("next_quest"));
		quest.set_quest_id(quest_id);

		m_quests.push_back(quest);
	}
}

auto quest::load_requests() -> void {
	// TODO FIXME quest
	// Process the state when you add quest requests

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::quest_requests));

	for (const auto &row : rs) {
		game_quest_id quest_id = row.get<game_quest_id>("questid");
		data::type::quest_request_info quest_request;

		int32_t request = row.get<int32_t>("objectid");
		int16_t count = row.get<int16_t>("quantity");

		vana::util::str::run_enum(row.get<string>("request_type"), [&quest_request, request, count](const string &cmp) {
			if (cmp == "item") {
				quest_request.is_item = true;
				quest_request.id = request;
				quest_request.count = count;
			}
			else if (cmp == "mob") {
				quest_request.is_mob = true;
				quest_request.id = request;
				quest_request.count = count;
			}
			else if (cmp == "quest") {
				quest_request.is_quest = true;
				quest_request.id = request;
				quest_request.quest_state = static_cast<int8_t>(count);
			}
			else {
				THROW_CODE_EXCEPTION(not_implemented_exception, "request_type");
			}
		});

		bool found = false;
		for (auto &quest : m_quests) {
			if (quest.get_quest_id() == quest_id) {
				found = true;
				quest.add_request(false, quest_request);
				break;
			}
		}

		if (!found) {
			// Stupidly, this could be the case in the official data files
			vana::quest current;
			current.set_quest_id(quest_id);
			current.add_request(false, quest_request);
			m_quests.push_back(current);
		}
	}
}

auto quest::load_required_jobs() -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::quest_required_jobs));

	for (const auto &row : rs) {
		game_quest_id quest_id = row.get<game_quest_id>("questid");
		data::type::quest_request_info request;
		request.is_job = true;
		request.id = row.get<game_job_id>("valid_jobid");

		bool found = false;
		for (auto &quest : m_quests) {
			if (quest.get_quest_id() == quest_id) {
				found = true;
				quest.add_request(true, request);
				break;
			}
		}

		if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	}
}

auto quest::load_rewards() -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::quest_rewards));

	for (const auto &row : rs) {
		game_quest_id quest_id = row.get<game_quest_id>("questid");
		data::type::quest_reward_info reward;
		game_job_id job = row.get<game_job_id>("job");
		string job_tracks = row.get<string>("job_tracks");
		bool start = (row.get<string>("quest_state") == "start");

		vana::util::str::run_enum(row.get<string>("reward_type"), [&reward](const string &cmp) {
			if (cmp == "item") reward.is_item = true;
			else if (cmp == "exp") reward.is_exp = true;
			else if (cmp == "mesos") reward.is_mesos = true;
			else if (cmp == "fame") reward.is_fame = true;
			else if (cmp == "skill") reward.is_skill = true;
			else if (cmp == "buff") reward.is_buff = true;
		});
		vana::util::str::run_flags(row.get<opt_string>("flags"), [&reward](const string &cmp) {
			if (cmp == "master_level_only") reward.master_level_only = true;
		});

		reward.id = row.get<int32_t>("rewardid");
		reward.count = row.get<int16_t>("quantity");
		reward.master_level = row.get<int16_t>("master_level");
		reward.gender = vana::util::game_logic::player::get_gender_id(row.get<string>("gender"));
		reward.prop = row.get<int32_t>("prop");

		bool found = false;
		for (auto &quest : m_quests) {
			if (quest.get_quest_id() == quest_id) {
				found = true;

				if (job != -1 || job_tracks.empty()) {
					quest.add_reward(start, reward, job);
					break;
				}

				vana::util::str::run_flags(job_tracks, [&quest, &reward, &start](const string &cmp) {
					auto add_reward_for_jobs = [&quest, &reward, &start](init_list<game_job_id> jobs) {
						for (const auto &job : jobs) {
							quest.add_reward(start, reward, job);
						}
					};
					if (cmp == "beginner") {
						add_reward_for_jobs({
							constant::job::id::beginner
						});
					}
					else if (cmp == "warrior") {
						add_reward_for_jobs({
							constant::job::id::swordsman,
							constant::job::id::fighter, constant::job::id::crusader, constant::job::id::hero,
							constant::job::id::page, constant::job::id::white_knight, constant::job::id::paladin,
							constant::job::id::spearman, constant::job::id::dragon_knight, constant::job::id::dark_knight,
						});
					}
					else if (cmp == "magician") {
						add_reward_for_jobs({
							constant::job::id::magician,
							constant::job::id::fp_wizard, constant::job::id::fp_mage, constant::job::id::fp_arch_mage,
							constant::job::id::il_wizard, constant::job::id::il_mage, constant::job::id::il_arch_mage,
							constant::job::id::cleric, constant::job::id::priest, constant::job::id::bishop,
						});
					}
					else if (cmp == "bowman") {
						add_reward_for_jobs({
							constant::job::id::archer,
							constant::job::id::hunter, constant::job::id::ranger, constant::job::id::bowmaster,
							constant::job::id::crossbowman, constant::job::id::sniper, constant::job::id::marksman,
						});
					}
					else if (cmp == "thief") {
						add_reward_for_jobs({
							constant::job::id::rogue,
							constant::job::id::assassin, constant::job::id::hermit, constant::job::id::night_lord,
							constant::job::id::bandit, constant::job::id::chief_bandit, constant::job::id::shadower,
						});
					}
					else if (cmp == "pirate") {
						add_reward_for_jobs({
							constant::job::id::pirate,
							constant::job::id::brawler, constant::job::id::marauder, constant::job::id::buccaneer,
							constant::job::id::gunslinger, constant::job::id::outlaw, constant::job::id::corsair,
						});
					}
					else if (cmp == "cygnus_beginner") {
						add_reward_for_jobs({
							constant::job::id::noblesse
						});
					}
					else if (cmp == "cygnus_warrior") {
						add_reward_for_jobs({
							constant::job::id::dawn_warrior1, constant::job::id::dawn_warrior2, constant::job::id::dawn_warrior3
						});
					}
					else if (cmp == "cygnus_magician") {
						add_reward_for_jobs({
							constant::job::id::blaze_wizard1, constant::job::id::blaze_wizard2, constant::job::id::blaze_wizard3
						});
					}
					else if (cmp == "cygnus_bowman") {
						add_reward_for_jobs({
							constant::job::id::wind_archer1, constant::job::id::wind_archer2, constant::job::id::wind_archer3
						});
					}
					else if (cmp == "cygnus_thief") {
						add_reward_for_jobs({
							constant::job::id::night_walker1, constant::job::id::night_walker2, constant::job::id::night_walker3
						});
					}
					else if (cmp == "cygnus_pirate") {
						add_reward_for_jobs({
							constant::job::id::thunder_breaker1, constant::job::id::thunder_breaker2, constant::job::id::thunder_breaker3
						});
					}
					else if (cmp == "episode2_beginner") {
						add_reward_for_jobs({
							constant::job::id::legend
						});
					}
					else if (cmp == "episode2_warrior") {
						add_reward_for_jobs({constant::job::id::aran1, constant::job::id::aran2, constant::job::id::aran3, constant::job::id::aran4});
					}
					else if (cmp == "episode2_magician") {
						add_reward_for_jobs({
							constant::job::id::evan1,
							constant::job::id::evan2, constant::job::id::evan3, constant::job::id::evan4,
							constant::job::id::evan5, constant::job::id::evan6, constant::job::id::evan7,
							constant::job::id::evan8, constant::job::id::evan9, constant::job::id::evan10,
						});
					}
					else {
						THROW_CODE_EXCEPTION(not_implemented_exception, "job_tracks");
					}
				});

				break;
			}
		}

		if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	}
}

auto quest::is_quest(game_quest_id quest_id) const -> bool {
	for (const auto &quest : m_quests) {
		if (quest.get_quest_id() == quest_id) {
			return true;
		}
	}

	return false;
}

auto quest::get_info(game_quest_id quest_id) const -> const vana::quest & {
	for (const auto &quest : m_quests) {
		if (quest.get_quest_id() == quest_id) {
			return quest;
		}
	}

	THROW_CODE_EXCEPTION(codepath_invalid_exception);
}

}
}
}