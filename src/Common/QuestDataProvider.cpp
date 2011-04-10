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
#include "QuestDataProvider.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "Quest.h"
#include "StringUtilities.h"

using Initializing::outputWidth;
using StringUtilities::runFlags;

QuestDataProvider * QuestDataProvider::singleton = nullptr;

void QuestDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Quests... ";

	loadQuestData();
	loadRequests();
	loadRequiredJobs();
	loadRewards();

	std::cout << "DONE" << std::endl;
}

void QuestDataProvider::loadQuestData() {
	m_quests.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM quest_data");
	mysqlpp::UseQueryResult res = query.use();
	Quest curQuest;
	int16_t questId;

	enum QuestData {
		QuestId = 0,
		NextQuest, Area, MinLevel, MaxLevel, PetCloseness,
		TamingMobLevel, Interval, Fame, TimeLimit, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		questId = atoi(row[QuestId]);

		curQuest.setNextQuest(atoi(row[NextQuest]));
		curQuest.setQuestId(questId);

		m_quests[questId] = curQuest;
	}
}

namespace Functors {
	struct RequestTypeFlags {
		void operator() (const string &cmp) {
			if (cmp == "item") quest->addItemRequest(reqId, count);
			else if (cmp == "mob") quest->addMobRequest(reqId, count);
			else if (cmp == "quest") quest->addQuestRequest(static_cast<int16_t>(reqId), static_cast<int8_t>(count));
		}
		Quest *quest;
		int32_t reqId;
		int16_t count;
	};
}

void QuestDataProvider::loadRequests() {
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM quest_requests");
	mysqlpp::UseQueryResult res = query.use();
	int16_t questId;
	int32_t reward;
	int16_t count;
	Quest *cur;

	using namespace Functors;

	enum RequestData {
		QuestId = 0,
		State, Type, ObjectId, Count
	};

	// TODO: Process the state when you add quest requests

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		questId = atoi(row[QuestId]);
		cur = &m_quests[questId];

		reward = atoi(row[ObjectId]);
		count = atoi(row[Count]);

		RequestTypeFlags whoo = {cur, reward, count};
		runFlags(row[Type], whoo);
	}
}

void QuestDataProvider::loadRequiredJobs() {
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM quest_required_jobs");
	mysqlpp::UseQueryResult res = query.use();
	int16_t questId;
	Quest *cur;

	enum QuestData {
		QuestId = 0,
		JobId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		questId = atoi(row[QuestId]);
		cur = &m_quests[questId];

		cur->addValidJob(atoi(row[JobId]));
	}
}

namespace Functors {
	struct QuestJobFlags {
		void operator() (const string &cmp) {
			if (cmp == "beginner") {
				quest->addReward(start, *reward, Jobs::JobIds::Beginner);
			}
			else if (cmp == "warrior") {
				quest->addReward(start, *reward, Jobs::JobIds::Swordsman);
				quest->addReward(start, *reward, Jobs::JobIds::Fighter);
				quest->addReward(start, *reward, Jobs::JobIds::Crusader);
				quest->addReward(start, *reward, Jobs::JobIds::Hero);
				quest->addReward(start, *reward, Jobs::JobIds::Page);
				quest->addReward(start, *reward, Jobs::JobIds::WhiteKnight);
				quest->addReward(start, *reward, Jobs::JobIds::Paladin);
				quest->addReward(start, *reward, Jobs::JobIds::Spearman);
				quest->addReward(start, *reward, Jobs::JobIds::DragonKnight);
				quest->addReward(start, *reward, Jobs::JobIds::DarkKnight);
			}
			else if (cmp == "magician") {
				quest->addReward(start, *reward, Jobs::JobIds::Magician);
				quest->addReward(start, *reward, Jobs::JobIds::FPWizard);
				quest->addReward(start, *reward, Jobs::JobIds::FPMage);
				quest->addReward(start, *reward, Jobs::JobIds::FPArchMage);
				quest->addReward(start, *reward, Jobs::JobIds::ILWizard);
				quest->addReward(start, *reward, Jobs::JobIds::ILMage);
				quest->addReward(start, *reward, Jobs::JobIds::ILArchMage);
				quest->addReward(start, *reward, Jobs::JobIds::Cleric);
				quest->addReward(start, *reward, Jobs::JobIds::Priest);
				quest->addReward(start, *reward, Jobs::JobIds::Bishop);
			}
			else if (cmp == "bowman") {
				quest->addReward(start, *reward, Jobs::JobIds::Archer);
				quest->addReward(start, *reward, Jobs::JobIds::Hunter);
				quest->addReward(start, *reward, Jobs::JobIds::Ranger);
				quest->addReward(start, *reward, Jobs::JobIds::Bowmaster);
				quest->addReward(start, *reward, Jobs::JobIds::Crossbowman);
				quest->addReward(start, *reward, Jobs::JobIds::Sniper);
				quest->addReward(start, *reward, Jobs::JobIds::Marksman);
			}
			else if (cmp == "thief") {
				quest->addReward(start, *reward, Jobs::JobIds::Rogue);
				quest->addReward(start, *reward, Jobs::JobIds::Assassin);
				quest->addReward(start, *reward, Jobs::JobIds::Hermit);
				quest->addReward(start, *reward, Jobs::JobIds::NightLord);
				quest->addReward(start, *reward, Jobs::JobIds::Bandit);
				quest->addReward(start, *reward, Jobs::JobIds::ChiefBandit);
				quest->addReward(start, *reward, Jobs::JobIds::Shadower);
			}
			else if (cmp == "pirate") {
				quest->addReward(start, *reward, Jobs::JobIds::Pirate);
				quest->addReward(start, *reward, Jobs::JobIds::Brawler);
				quest->addReward(start, *reward, Jobs::JobIds::Marauder);
				quest->addReward(start, *reward, Jobs::JobIds::Buccaneer);
				quest->addReward(start, *reward, Jobs::JobIds::Gunslinger);
				quest->addReward(start, *reward, Jobs::JobIds::Outlaw);
				quest->addReward(start, *reward, Jobs::JobIds::Corsair);
			}
			else if (cmp == "cygnus_beginner") {
				quest->addReward(start, *reward, Jobs::JobIds::Noblesse);
			}
			else if (cmp == "cygnus_warrior") {
				quest->addReward(start, *reward, Jobs::JobIds::DawnWarrior1);
				quest->addReward(start, *reward, Jobs::JobIds::DawnWarrior2);
				quest->addReward(start, *reward, Jobs::JobIds::DawnWarrior3);
			}
			else if (cmp == "cygnus_magician") {
				quest->addReward(start, *reward, Jobs::JobIds::BlazeWizard1);
				quest->addReward(start, *reward, Jobs::JobIds::BlazeWizard2);
				quest->addReward(start, *reward, Jobs::JobIds::BlazeWizard3);
			}
			else if (cmp == "cygnus_bowman") {
				quest->addReward(start, *reward, Jobs::JobIds::WindArcher1);
				quest->addReward(start, *reward, Jobs::JobIds::WindArcher2);
				quest->addReward(start, *reward, Jobs::JobIds::WindArcher3);
			}
			else if (cmp == "cygnus_thief") {
				quest->addReward(start, *reward, Jobs::JobIds::NightWalker1);
				quest->addReward(start, *reward, Jobs::JobIds::NightWalker2);
				quest->addReward(start, *reward, Jobs::JobIds::NightWalker3);
			}
			else if (cmp == "cygnus_pirate") {
				quest->addReward(start, *reward, Jobs::JobIds::ThunderBreaker1);
				quest->addReward(start, *reward, Jobs::JobIds::ThunderBreaker2);
				quest->addReward(start, *reward, Jobs::JobIds::ThunderBreaker3);
			}
			else if (cmp == "episode2_beginner") {
				quest->addReward(start, *reward, Jobs::JobIds::Legend);
			}
			else if (cmp == "episode2_warrior") {
				quest->addReward(start, *reward, Jobs::JobIds::Aran1);
				quest->addReward(start, *reward, Jobs::JobIds::Aran2);
				quest->addReward(start, *reward, Jobs::JobIds::Aran3);
				quest->addReward(start, *reward, Jobs::JobIds::Aran4);
			}
			else if (cmp == "episode2_magician") {
				quest->addReward(start, *reward, Jobs::JobIds::Evan1);
				quest->addReward(start, *reward, Jobs::JobIds::Evan2);
				quest->addReward(start, *reward, Jobs::JobIds::Evan3);
				quest->addReward(start, *reward, Jobs::JobIds::Evan4);
				quest->addReward(start, *reward, Jobs::JobIds::Evan5);
				quest->addReward(start, *reward, Jobs::JobIds::Evan6);
				quest->addReward(start, *reward, Jobs::JobIds::Evan7);
				quest->addReward(start, *reward, Jobs::JobIds::Evan8);
				quest->addReward(start, *reward, Jobs::JobIds::Evan9);
				quest->addReward(start, *reward, Jobs::JobIds::Evan10);
			}
		}
		Quest *quest;
		QuestRewardInfo *reward;
		bool start;
	};
	struct RewardFlags {
		void operator() (const string &cmp) {
			if (cmp == "master_level_only") reward->masterLevelOnly = true;
		}
		QuestRewardInfo *reward;
	};
	struct RewardTypeFlags {
		void operator() (const string &cmp) {
			if (cmp == "item") reward->isItem = true;
			else if (cmp == "exp") reward->isExp = true;
			else if (cmp == "mesos") reward->isMesos = true;
			else if (cmp == "fame") reward->isFame = true;
			else if (cmp == "skill") reward->isSkill = true;
			else if (cmp == "buff") reward->isBuff = true;
		}
		QuestRewardInfo *reward;
	};
}

void QuestDataProvider::loadRewards() {
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM quest_rewards");
	mysqlpp::UseQueryResult res = query.use();
	string jobTracks;
	int16_t questId;
	int16_t job;
	bool start;
	Quest *cur;
	QuestRewardInfo rwa;

	using namespace Functors;

	enum RewardData {
		Id = 0,
		QuestId, State, Type, ObjectId, Count,
		MasterLevel, Gender, JobTrackFlags, Job, Prop,
		Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		questId = atoi(row[QuestId]);
		cur = &m_quests[questId];
		rwa = QuestRewardInfo();
		job = atoi(row[Job]);
		jobTracks = row[JobTrackFlags];
		start = (static_cast<string>(row[State]) == "start");

		RewardTypeFlags whoo = {&rwa};
		RewardFlags flags = {&rwa};
		runFlags(row[Type], whoo);
		runFlags(row[Flags], flags);

		rwa.id = atoi(row[ObjectId]);
		rwa.count = atoi(row[Count]);
		rwa.masterLevel = atoi(row[MasterLevel]);
		rwa.gender = GameLogicUtilities::getGenderId(row[Gender]);
		rwa.prop = atoi(row[Prop]);

		if (job != -1 || jobTracks.length() == 0) {
			cur->addReward(start, rwa, job);
		}
		else {
			QuestJobFlags ohyeah = {cur, &rwa, start};
			runFlags(jobTracks, ohyeah);
		}
	}
}

int16_t QuestDataProvider::getItemRequest(int16_t questId, int32_t itemId) {
	if (m_quests.find(questId) != m_quests.end()) {
		return m_quests[questId].getItemRequestQuantity(itemId);
	}
	return 0;
}