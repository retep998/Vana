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
#include "QuestDataProvider.hpp"
#include "Algorithm.hpp"
#include "Database.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "Quest.hpp"
#include "StringUtilities.hpp"
#include <initializer_list>
#include <iomanip>
#include <iostream>

namespace vana {

auto quest_data_provider::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Quests... ";

	load_quest_data();
	load_requests();
	load_required_jobs();
	load_rewards();

	std::cout << "DONE" << std::endl;
}

auto quest_data_provider::load_quest_data() -> void {
	m_quests.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("quest_data"));

	for (const auto &row : rs) {
		quest quest;
		game_quest_id quest_id = row.get<game_quest_id>("questid");

		quest.set_next_quest(row.get<game_quest_id>("next_quest"));
		quest.set_quest_id(quest_id);

		m_quests.emplace(quest_id, quest);
	}
}

auto quest_data_provider::load_requests() -> void {
	// TODO FIXME quest
	// Process the state when you add quest requests

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("quest_requests"));

	for (const auto &row : rs) {
		game_quest_id questId = row.get<game_quest_id>("questid");
		auto &quest = m_quests[questId];
		quest_request_info quest_request;

		int32_t request = row.get<int32_t>("objectid");
		int16_t count = row.get<int16_t>("quantity");

		utilities::str::run_enum(row.get<string>("request_type"), [&quest_request, request, count](const string &cmp) {
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
				throw not_implemented_exception{"request_type"};
			}
		});

		quest.add_request(false, quest_request);
	}
}

auto quest_data_provider::load_required_jobs() -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("quest_required_jobs"));

	for (const auto &row : rs) {
		game_quest_id questId = row.get<game_quest_id>("questid");
		auto &quest = m_quests[questId];
		quest_request_info request;
		request.is_job = true;
		request.id = row.get<game_job_id>("valid_jobid");
		quest.add_request(true, request);
	}
}

auto quest_data_provider::load_rewards() -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("quest_rewards"));

	for (const auto &row : rs) {
		game_quest_id quest_id = row.get<game_quest_id>("questid");
		quest &quest = m_quests[quest_id];
		quest_reward_info reward;
		game_job_id job = row.get<game_job_id>("job");
		string job_tracks = row.get<string>("job_tracks");
		bool start = (row.get<string>("quest_state") == "start");

		utilities::str::run_enum(row.get<string>("reward_type"), [&reward](const string &cmp) {
			if (cmp == "item") reward.is_item = true;
			else if (cmp == "exp") reward.is_exp = true;
			else if (cmp == "mesos") reward.is_mesos = true;
			else if (cmp == "fame") reward.is_fame = true;
			else if (cmp == "skill") reward.is_skill = true;
			else if (cmp == "buff") reward.is_buff = true;
		});
		utilities::str::run_flags(row.get<opt_string>("flags"), [&reward](const string &cmp) {
			if (cmp == "master_level_only") reward.master_level_only = true;
		});

		reward.id = row.get<int32_t>("rewardid");
		reward.count = row.get<int16_t>("quantity");
		reward.master_level = row.get<int16_t>("master_level");
		reward.gender = game_logic_utilities::get_gender_id(row.get<string>("gender"));
		reward.prop = row.get<int32_t>("prop");

		if (job != -1 || job_tracks.empty()) {
			quest.add_reward(start, reward, job);
		}
		else {
			utilities::str::run_flags(job_tracks, [&quest, &reward, &start](const string &cmp) {
				auto add_reward_for_jobs = [&quest, &reward, &start](init_list<game_job_id> jobs) {
					for (const auto &job : jobs) {
						quest.add_reward(start, reward, job);
					}
				};
				if (cmp == "beginner") {
					add_reward_for_jobs({jobs::job_ids::beginner});
				}
				else if (cmp == "warrior") {
					add_reward_for_jobs({
						jobs::job_ids::swordsman,
						jobs::job_ids::fighter, jobs::job_ids::crusader, jobs::job_ids::hero,
						jobs::job_ids::page, jobs::job_ids::white_knight, jobs::job_ids::paladin,
						jobs::job_ids::spearman, jobs::job_ids::dragon_knight, jobs::job_ids::dark_knight
					});
				}
				else if (cmp == "magician") {
					add_reward_for_jobs({
						jobs::job_ids::magician,
						jobs::job_ids::fp_wizard, jobs::job_ids::fp_mage, jobs::job_ids::fp_arch_mage,
						jobs::job_ids::il_wizard, jobs::job_ids::il_mage, jobs::job_ids::il_arch_mage,
						jobs::job_ids::cleric, jobs::job_ids::priest, jobs::job_ids::bishop
					});
				}
				else if (cmp == "bowman") {
					add_reward_for_jobs({
						jobs::job_ids::archer,
						jobs::job_ids::hunter, jobs::job_ids::ranger, jobs::job_ids::bowmaster,
						jobs::job_ids::crossbowman, jobs::job_ids::sniper, jobs::job_ids::marksman
					});
				}
				else if (cmp == "thief") {
					add_reward_for_jobs({
						jobs::job_ids::rogue,
						jobs::job_ids::assassin, jobs::job_ids::hermit, jobs::job_ids::night_lord,
						jobs::job_ids::bandit, jobs::job_ids::chief_bandit, jobs::job_ids::shadower
					});
				}
				else if (cmp == "pirate") {
					add_reward_for_jobs({
						jobs::job_ids::pirate,
						jobs::job_ids::brawler, jobs::job_ids::marauder, jobs::job_ids::buccaneer,
						jobs::job_ids::gunslinger, jobs::job_ids::outlaw, jobs::job_ids::corsair
					});
				}
				else if (cmp == "cygnus_beginner") {
					add_reward_for_jobs({jobs::job_ids::noblesse});
				}
				else if (cmp == "cygnus_warrior") {
					add_reward_for_jobs({jobs::job_ids::dawn_warrior1, jobs::job_ids::dawn_warrior2, jobs::job_ids::dawn_warrior3});
				}
				else if (cmp == "cygnus_magician") {
					add_reward_for_jobs({jobs::job_ids::blaze_wizard1, jobs::job_ids::blaze_wizard2, jobs::job_ids::blaze_wizard3});
				}
				else if (cmp == "cygnus_bowman") {
					add_reward_for_jobs({jobs::job_ids::wind_archer1, jobs::job_ids::wind_archer2, jobs::job_ids::wind_archer3});
				}
				else if (cmp == "cygnus_thief") {
					add_reward_for_jobs({jobs::job_ids::night_walker1, jobs::job_ids::night_walker2, jobs::job_ids::night_walker3});
				}
				else if (cmp == "cygnus_pirate") {
					add_reward_for_jobs({jobs::job_ids::thunder_breaker1, jobs::job_ids::thunder_breaker2, jobs::job_ids::thunder_breaker3});
				}
				else if (cmp == "episode2_beginner") {
					add_reward_for_jobs({jobs::job_ids::legend});
				}
				else if (cmp == "episode2_warrior") {
					add_reward_for_jobs({jobs::job_ids::aran1, jobs::job_ids::aran2, jobs::job_ids::aran3, jobs::job_ids::aran4});
				}
				else if (cmp == "episode2_magician") {
					add_reward_for_jobs({
						jobs::job_ids::evan1,
						jobs::job_ids::evan2, jobs::job_ids::evan3, jobs::job_ids::evan4,
						jobs::job_ids::evan5, jobs::job_ids::evan6, jobs::job_ids::evan7,
						jobs::job_ids::evan8, jobs::job_ids::evan9, jobs::job_ids::evan10
					});
				}
				else {
					throw not_implemented_exception{"job_tracks"};
				}
			});
		}
	}
}

auto quest_data_provider::is_quest(game_quest_id quest_id) const -> bool {
	return ext::is_element(m_quests, quest_id);
}

auto quest_data_provider::get_info(game_quest_id quest_id) const -> const quest & {
	return m_quests.find(quest_id)->second;
}

}