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
#include "QuestDataProvider.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "Quest.h"
#include "StringUtilities.h"
#include <initializer_list>

using Initializing::OutputWidth;
using StringUtilities::runFlags;

QuestDataProvider * QuestDataProvider::singleton = nullptr;

void QuestDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Quests... ";

	loadQuestData();
	loadRequests();
	loadRequiredJobs();
	loadRewards();

	std::cout << "DONE" << std::endl;
}

void QuestDataProvider::loadQuestData() {
	m_quests.clear();
	Quest curQuest;
	int16_t questId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM quest_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		questId = row.get<int16_t>("questid");
		curQuest.setNextQuest(row.get<int16_t>("next_quest"));
		curQuest.setQuestId(questId);

		m_quests[questId] = curQuest;
	}
}

void QuestDataProvider::loadRequests() {
	int16_t questId;
	int32_t reward;
	int16_t count;
	Quest *cur;

	// TODO: Process the state when you add quest requests

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM quest_requests");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		questId = row.get<int16_t>("questid");
		cur = &m_quests[questId];

		reward = row.get<int32_t>("objectid");
		count = row.get<int16_t>("quantity");

		runFlags(row.get<opt_string>("request_type"), [&cur, reward, count](const string &cmp) {
			if (cmp == "item") cur->addItemRequest(reward, count);
			else if (cmp == "mob") cur->addMobRequest(reward, count);
			else if (cmp == "quest") cur->addQuestRequest(static_cast<int16_t>(reward), static_cast<int8_t>(count));
		});
	}
}

void QuestDataProvider::loadRequiredJobs() {
	int16_t questId;
	Quest *cur;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM quest_required_jobs");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		questId = row.get<int16_t>("questid");
		cur = &m_quests[questId];

		cur->addValidJob(row.get<int16_t>("valid_jobid"));
	}
}

void QuestDataProvider::loadRewards() {
	string jobTracks;
	int16_t questId;
	int16_t job;
	bool start;
	Quest *cur;
	QuestRewardInfo reward;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM quest_rewards");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		questId = row.get<int16_t>("questid");
		cur = &m_quests[questId];
		reward = QuestRewardInfo();
		job = row.get<int16_t>("job");
		jobTracks = row.get<string>("job_tracks");
		start = (row.get<string>("quest_state") == "start");

		runFlags(row.get<opt_string>("reward_type"), [&reward](const string &cmp) {
			if (cmp == "item") reward.isItem = true;
			else if (cmp == "exp") reward.isExp = true;
			else if (cmp == "mesos") reward.isMesos = true;
			else if (cmp == "fame") reward.isFame = true;
			else if (cmp == "skill") reward.isSkill = true;
			else if (cmp == "buff") reward.isBuff = true;
		});
		runFlags(row.get<opt_string>("flags"), [&reward](const string &cmp) {
			if (cmp == "master_level_only") reward.masterLevelOnly = true;
		});

		reward.id = row.get<int32_t>("rewardid");
		reward.count = row.get<int16_t>("quantity");
		reward.masterLevel = row.get<int16_t>("master_level");
		reward.gender = GameLogicUtilities::getGenderId(row.get<string>("gender"));
		reward.prop = row.get<int32_t>("prop");

		if (job != -1 || jobTracks.length() == 0) {
			cur->addReward(start, reward, job);
		}
		else {
			runFlags(jobTracks, [&cur, &reward, &start](const string &cmp) {
				// Workaround for VS2010 in two ways:
				// - There's a bug in the compiler where it won't properly capture variables on inner lambdas
				// - Secondly, it doesn't support std::initializer_list semantics so I have to do something ugly... very ugly
				Quest *&q = cur;
				QuestRewardInfo &r = reward;
				bool s = start;
				auto func = [&](std::initializer_list<int16_t> jobArr) {
					for (std::initializer_list<int16_t>::iterator i = jobArr.begin(); i != jobArr.end(); ++i) {
						q->addReward(s, r, *i);
					}
				};
				if (cmp == "beginner") {
					int16_t arr[] = {Jobs::JobIds::Beginner};
					func(std::initializer_list<int16_t>(arr, arr + 1));
				}
				else if (cmp == "warrior") {
					int16_t arr[] = {
						Jobs::JobIds::Swordsman,
						Jobs::JobIds::Fighter, Jobs::JobIds::Crusader, Jobs::JobIds::Hero,
						Jobs::JobIds::Page, Jobs::JobIds::WhiteKnight, Jobs::JobIds::Paladin,
						Jobs::JobIds::Spearman, Jobs::JobIds::DragonKnight, Jobs::JobIds::DarkKnight
					};
					func(std::initializer_list<int16_t>(arr, arr + 10));
				}
				else if (cmp == "magician") {
					int16_t arr[] = {
						Jobs::JobIds::Magician,
						Jobs::JobIds::FpWizard, Jobs::JobIds::FpMage, Jobs::JobIds::FpArchMage,
						Jobs::JobIds::IlWizard, Jobs::JobIds::IlMage, Jobs::JobIds::IlArchMage,
						Jobs::JobIds::Cleric, Jobs::JobIds::Priest, Jobs::JobIds::Bishop
					};
					func(std::initializer_list<int16_t>(arr, arr + 10));
				}
				else if (cmp == "bowman") {
					int16_t arr[] = {
						Jobs::JobIds::Archer,
						Jobs::JobIds::Hunter, Jobs::JobIds::Ranger, Jobs::JobIds::Bowmaster,
						Jobs::JobIds::Crossbowman, Jobs::JobIds::Sniper, Jobs::JobIds::Marksman
					};
					func(std::initializer_list<int16_t>(arr, arr + 7));
				}
				else if (cmp == "thief") {
					int16_t arr[] = {
						Jobs::JobIds::Rogue,
						Jobs::JobIds::Assassin, Jobs::JobIds::Hermit, Jobs::JobIds::NightLord,
						Jobs::JobIds::Bandit, Jobs::JobIds::ChiefBandit, Jobs::JobIds::Shadower
					};
					func(std::initializer_list<int16_t>(arr, arr + 7));
				}
				else if (cmp == "pirate") {
					int16_t arr[] = {
						Jobs::JobIds::Pirate,
						Jobs::JobIds::Brawler, Jobs::JobIds::Marauder, Jobs::JobIds::Buccaneer,
						Jobs::JobIds::Gunslinger, Jobs::JobIds::Outlaw, Jobs::JobIds::Corsair
					};
					func(std::initializer_list<int16_t>(arr, arr + 7));
				}
				else if (cmp == "cygnus_beginner") {
					int16_t arr[] = {Jobs::JobIds::Noblesse};
					func(std::initializer_list<int16_t>(arr, arr + 1));
				}
				else if (cmp == "cygnus_warrior") {
					int16_t arr[] = {Jobs::JobIds::DawnWarrior1, Jobs::JobIds::DawnWarrior2, Jobs::JobIds::DawnWarrior3};
					func(std::initializer_list<int16_t>(arr, arr + 3));
				}
				else if (cmp == "cygnus_magician") {
					int16_t arr[] = {Jobs::JobIds::BlazeWizard1, Jobs::JobIds::BlazeWizard2, Jobs::JobIds::BlazeWizard3};
					func(std::initializer_list<int16_t>(arr, arr + 3));
				}
				else if (cmp == "cygnus_bowman") {
					int16_t arr[] = {Jobs::JobIds::WindArcher1, Jobs::JobIds::WindArcher2, Jobs::JobIds::WindArcher3};
					func(std::initializer_list<int16_t>(arr, arr + 3));
				}
				else if (cmp == "cygnus_thief") {
					int16_t arr[] = {Jobs::JobIds::NightWalker1, Jobs::JobIds::NightWalker2, Jobs::JobIds::NightWalker3};
					func(std::initializer_list<int16_t>(arr, arr + 3));
				}
				else if (cmp == "cygnus_pirate") {
					int16_t arr[] = {Jobs::JobIds::ThunderBreaker1, Jobs::JobIds::ThunderBreaker2, Jobs::JobIds::ThunderBreaker3};
					func(std::initializer_list<int16_t>(arr, arr + 3));
				}
				else if (cmp == "episode2_beginner") {
					int16_t arr[] = {Jobs::JobIds::Legend};
					func(std::initializer_list<int16_t>(arr, arr + 1));
				}
				else if (cmp == "episode2_warrior") {
					int16_t arr[] = {Jobs::JobIds::Aran1, Jobs::JobIds::Aran2, Jobs::JobIds::Aran3, Jobs::JobIds::Aran4};
					func(std::initializer_list<int16_t>(arr, arr + 4));
				}
				else if (cmp == "episode2_magician") {
					int16_t arr[] = {
						Jobs::JobIds::Evan1,
						Jobs::JobIds::Evan2, Jobs::JobIds::Evan3, Jobs::JobIds::Evan4,
						Jobs::JobIds::Evan5, Jobs::JobIds::Evan6, Jobs::JobIds::Evan7,
						Jobs::JobIds::Evan8, Jobs::JobIds::Evan9, Jobs::JobIds::Evan10
					};
					func(std::initializer_list<int16_t>(arr, arr + 10));
				}
			});
		}
	}
}

int16_t QuestDataProvider::getItemRequest(int16_t questId, int32_t itemId) {
	if (m_quests.find(questId) != m_quests.end()) {
		return m_quests[questId].getItemRequestQuantity(itemId);
	}
	return 0;
}