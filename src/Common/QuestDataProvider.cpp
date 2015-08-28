/*
Copyright (C) 2008-2015 Vana Development Team

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

namespace Vana {

auto QuestDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Quests... ";

	loadQuestData();
	loadRequests();
	loadRequiredJobs();
	loadRewards();

	std::cout << "DONE" << std::endl;
}

auto QuestDataProvider::loadQuestData() -> void {
	m_quests.clear();

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("quest_data"));

	for (const auto &row : rs) {
		Quest quest;
		quest_id_t questId = row.get<quest_id_t>("questid");

		quest.setNextQuest(row.get<quest_id_t>("next_quest"));
		quest.setQuestId(questId);

		m_quests.emplace(questId, quest);
	}
}

auto QuestDataProvider::loadRequests() -> void {
	// TODO FIXME quest
	// Process the state when you add quest requests

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("quest_requests"));

	for (const auto &row : rs) {
		quest_id_t questId = row.get<quest_id_t>("questid");
		auto &quest = m_quests[questId];
		QuestRequestInfo questRequest;

		int32_t request = row.get<int32_t>("objectid");
		int16_t count = row.get<int16_t>("quantity");

		StringUtilities::runEnum(row.get<string_t>("request_type"), [&questRequest, request, count](const string_t &cmp) {
			if (cmp == "item") {
				questRequest.isItem = true;
				questRequest.id = request;
				questRequest.count = count;
			}
			else if (cmp == "mob") {
				questRequest.isMob = true;
				questRequest.id = request;
				questRequest.count = count;
			}
			else if (cmp == "quest") {
				questRequest.isQuest = true;
				questRequest.id = request;
				questRequest.questState = static_cast<int8_t>(count);
			}
		});

		quest.addRequest(false, questRequest);
	}
}

auto QuestDataProvider::loadRequiredJobs() -> void {
	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("quest_required_jobs"));

	for (const auto &row : rs) {
		quest_id_t questId = row.get<quest_id_t>("questid");
		auto &quest = m_quests[questId];
		QuestRequestInfo request;
		request.isJob = true;
		request.id = row.get<job_id_t>("valid_jobid");
		quest.addRequest(true, request);
	}
}

auto QuestDataProvider::loadRewards() -> void {
	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("quest_rewards"));

	for (const auto &row : rs) {
		quest_id_t questId = row.get<quest_id_t>("questid");
		Quest &quest = m_quests[questId];
		QuestRewardInfo reward;
		job_id_t job = row.get<job_id_t>("job");
		string_t jobTracks = row.get<string_t>("job_tracks");
		bool start = (row.get<string_t>("quest_state") == "start");

		StringUtilities::runEnum(row.get<string_t>("reward_type"), [&reward](const string_t &cmp) {
			if (cmp == "item") reward.isItem = true;
			else if (cmp == "exp") reward.isExp = true;
			else if (cmp == "mesos") reward.isMesos = true;
			else if (cmp == "fame") reward.isFame = true;
			else if (cmp == "skill") reward.isSkill = true;
			else if (cmp == "buff") reward.isBuff = true;
		});
		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&reward](const string_t &cmp) {
			if (cmp == "master_level_only") reward.masterLevelOnly = true;
		});

		reward.id = row.get<int32_t>("rewardid");
		reward.count = row.get<int16_t>("quantity");
		reward.masterLevel = row.get<int16_t>("master_level");
		reward.gender = GameLogicUtilities::getGenderId(row.get<string_t>("gender"));
		reward.prop = row.get<int32_t>("prop");

		if (job != -1 || jobTracks.empty()) {
			quest.addReward(start, reward, job);
		}
		else {
			StringUtilities::runFlags(jobTracks, [&quest, &reward, &start](const string_t &cmp) {
				auto addRewardForJobs = [&quest, &reward, &start](init_list_t<job_id_t> jobs) {
					for (const auto &job : jobs) {
						quest.addReward(start, reward, job);
					}
				};
				if (cmp == "beginner") {
					addRewardForJobs({Jobs::JobIds::Beginner});
				}
				else if (cmp == "warrior") {
					addRewardForJobs({
						Jobs::JobIds::Swordsman,
						Jobs::JobIds::Fighter, Jobs::JobIds::Crusader, Jobs::JobIds::Hero,
						Jobs::JobIds::Page, Jobs::JobIds::WhiteKnight, Jobs::JobIds::Paladin,
						Jobs::JobIds::Spearman, Jobs::JobIds::DragonKnight, Jobs::JobIds::DarkKnight
					});
				}
				else if (cmp == "magician") {
					addRewardForJobs({
						Jobs::JobIds::Magician,
						Jobs::JobIds::FpWizard, Jobs::JobIds::FpMage, Jobs::JobIds::FpArchMage,
						Jobs::JobIds::IlWizard, Jobs::JobIds::IlMage, Jobs::JobIds::IlArchMage,
						Jobs::JobIds::Cleric, Jobs::JobIds::Priest, Jobs::JobIds::Bishop
					});
				}
				else if (cmp == "bowman") {
					addRewardForJobs({
						Jobs::JobIds::Archer,
						Jobs::JobIds::Hunter, Jobs::JobIds::Ranger, Jobs::JobIds::Bowmaster,
						Jobs::JobIds::Crossbowman, Jobs::JobIds::Sniper, Jobs::JobIds::Marksman
					});
				}
				else if (cmp == "thief") {
					addRewardForJobs({
						Jobs::JobIds::Rogue,
						Jobs::JobIds::Assassin, Jobs::JobIds::Hermit, Jobs::JobIds::NightLord,
						Jobs::JobIds::Bandit, Jobs::JobIds::ChiefBandit, Jobs::JobIds::Shadower
					});
				}
				else if (cmp == "pirate") {
					addRewardForJobs({
						Jobs::JobIds::Pirate,
						Jobs::JobIds::Brawler, Jobs::JobIds::Marauder, Jobs::JobIds::Buccaneer,
						Jobs::JobIds::Gunslinger, Jobs::JobIds::Outlaw, Jobs::JobIds::Corsair
					});
				}
				else if (cmp == "cygnus_beginner") {
					addRewardForJobs({Jobs::JobIds::Noblesse});
				}
				else if (cmp == "cygnus_warrior") {
					addRewardForJobs({Jobs::JobIds::DawnWarrior1, Jobs::JobIds::DawnWarrior2, Jobs::JobIds::DawnWarrior3});
				}
				else if (cmp == "cygnus_magician") {
					addRewardForJobs({Jobs::JobIds::BlazeWizard1, Jobs::JobIds::BlazeWizard2, Jobs::JobIds::BlazeWizard3});
				}
				else if (cmp == "cygnus_bowman") {
					addRewardForJobs({Jobs::JobIds::WindArcher1, Jobs::JobIds::WindArcher2, Jobs::JobIds::WindArcher3});
				}
				else if (cmp == "cygnus_thief") {
					addRewardForJobs({Jobs::JobIds::NightWalker1, Jobs::JobIds::NightWalker2, Jobs::JobIds::NightWalker3});
				}
				else if (cmp == "cygnus_pirate") {
					addRewardForJobs({Jobs::JobIds::ThunderBreaker1, Jobs::JobIds::ThunderBreaker2, Jobs::JobIds::ThunderBreaker3});
				}
				else if (cmp == "episode2_beginner") {
					addRewardForJobs({Jobs::JobIds::Legend});
				}
				else if (cmp == "episode2_warrior") {
					addRewardForJobs({Jobs::JobIds::Aran1, Jobs::JobIds::Aran2, Jobs::JobIds::Aran3, Jobs::JobIds::Aran4});
				}
				else if (cmp == "episode2_magician") {
					addRewardForJobs({
						Jobs::JobIds::Evan1,
						Jobs::JobIds::Evan2, Jobs::JobIds::Evan3, Jobs::JobIds::Evan4,
						Jobs::JobIds::Evan5, Jobs::JobIds::Evan6, Jobs::JobIds::Evan7,
						Jobs::JobIds::Evan8, Jobs::JobIds::Evan9, Jobs::JobIds::Evan10
					});
				}
			});
		}
	}
}

auto QuestDataProvider::isQuest(quest_id_t questId) const -> bool {
	return ext::is_element(m_quests, questId);
}

auto QuestDataProvider::getInfo(quest_id_t questId) const -> const Quest & {
	return m_quests.find(questId)->second;
}

}