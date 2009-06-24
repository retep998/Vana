/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "InitializeChannel.h"
#include "BeautyDataProvider.h"
#include "ChatHandler.h"
#include "Database.h"
#include "DropDataProvider.h"
#include "EventDataProvider.h"
#include "InitializeCommon.h"
#include "ItemDataProvider.h"
#include "MapleVersion.h"
#include "MiscUtilities.h"
#include "MobDataProvider.h"
#include "Pets.h"
#include "Quests.h"
#include "Reactors.h"
#include "ShopDataProvider.h"
#include "Skills.h"
#include <cstdio>
#include <iostream>

using std::string;
using MiscUtilities::atob;

void Initializing::checkMcdbVersion() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM mcdb_info LIMIT 1");
	mysqlpp::StoreQueryResult res = query.store();

	int32_t version = (int32_t) res[0]["version"];
	int32_t subversion = (int32_t) res[0]["subversion"];
	int32_t maple_version = (int32_t) res[0]["maple_version"];

	if (version != McdbVersion || subversion != McdbSubVersion) {
		// MCDB incompatible
		std::cout << "ERROR: MCDB version imcompatible. Expected: " << McdbVersion << "." << McdbSubVersion << " ";
		std::cout << "Have: " << version << "." << subversion << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(4);
	}

	if (maple_version != MAPLE_VERSION) {
		std::cout << "WARNING: Your copy of MCDB is based on an incongruent version of the WZ files. Vana: " << MAPLE_VERSION << " MCDB: " << maple_version << std::endl;
	}
}

void Initializing::loadData() {
	ItemDataProvider::Instance()->loadData();
	ShopDataProvider::Instance()->loadData();
	MobDataProvider::Instance()->loadData();
	DropDataProvider::Instance()->loadData();
	BeautyDataProvider::Instance()->loadData();
	EventDataProvider::Instance()->loadEvents();
	initializeReactors();
	initializeQuests();
	initializeSkills();
	initializePets();
};

void Initializing::initializeReactors() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Reactors... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM reactoreventdata ORDER BY reactorid, state ASC");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW reactorRow;
	while (reactorRow = res.fetch_raw_row()) {
		// Col0 : Row ID
		//    1 : Reactor ID
		//    2 : State
		//    3 : Type
		//    4 : Item ID
		//    5 : LT X
		//    6 : LT Y
		//    7 : RB X
		//    8 : RB Y
		//    9 : Next State

		ReactorEventInfo revent;
		revent.state = atoi(reactorRow[2]);
		revent.type = atoi(reactorRow[3]);
		revent.itemid = atoi(reactorRow[4]);
		revent.lt.x = atoi(reactorRow[5]);
		revent.lt.y = atoi(reactorRow[6]);
		revent.rb.x = atoi(reactorRow[7]);
		revent.rb.y = atoi(reactorRow[8]);
		revent.nextstate = atoi(reactorRow[9]);
		Reactors::setMaxstates(atoi(reactorRow[1]), revent.nextstate);
		Reactors::addEventInfo(atoi(reactorRow[1]), revent);
	}

	std::cout << "DONE" << std::endl;
}

void Initializing::initializeQuests() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Quests... ";
	// Quests
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM questdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW Row;
	while (Row = res.fetch_raw_row()) {
		// Col0 : Quest ID
		//    1 : Next Quest ID
		Quests::setNextQuest(atoi(Row[0]), atoi(Row[1]));
	}

	// Quest requests
	string queries[QuestRequestTypes::Count] = {
		"SELECT questid, oid, count FROM questrequestdata WHERE mob = 1 ORDER BY questid ASC",
		"SELECT questid, oid, count FROM questrequestdata WHERE item = 1 ORDER BY questid ASC",
		"SELECT questid, oid, count FROM questrequestdata WHERE quest = 1 ORDER BY questid ASC"
	};
	int32_t reqtypes[QuestRequestTypes::Count] = {
		QuestRequestTypes::Mob,
		QuestRequestTypes::Item,
		QuestRequestTypes::Quest
	};
	int32_t currentid = 0;
	int32_t previousid = -1;
	unordered_map<int32_t, int16_t> reqs;

	for (int32_t i = 0; i < QuestRequestTypes::Count; i++) {
		query << queries[i];
		res = query.use();

		currentid = 0;
		previousid = -1;

		while (Row = res.fetch_raw_row()) {
			// Col0 : Quest ID
			//    1 : Object ID
			//    2 : Count
			currentid = atoi(Row[0]);

			if (currentid != previousid && previousid != -1) {
				Quests::addRequest(previousid, reqtypes[i], reqs);
				reqs.clear();
			}

			int32_t id = atoi(Row[1]);
			int16_t count = atoi(Row[2]);
			reqs[id] = count;

			previousid = currentid;
		}
		if (previousid != -1) {
			Quests::addRequest(previousid, reqtypes[i], reqs);
			reqs.clear();
		}
	}

	// Quest Rewards
	query << "SELECT * FROM questrewarddata ORDER BY questid ASC";
	res = query.use();

	currentid = 0;
	previousid = -1;
	QuestRewardsInfo rwas;
	while (Row = res.fetch_raw_row()) {
		// Col0 : Row ID
		//    1 : Quest ID
		//    2 : Start
		//    3 : Item
		//    4 : EXP
		//    5 : Mesos
		//    6 : Fame
		//    7 : Skill
		//    8 : Object ID
		//    9 : Count
		//   10 : Gender
		//   11 : Job
		//   12 : Prop
		currentid = atoi(Row[1]);

		if (currentid != previousid && previousid != -1) {
			Quests::addReward(previousid, rwas);
			rwas.clear();
		}

		QuestRewardInfo rwa;
		rwa.start = atob(Row[2]);
		rwa.isitem = atob(Row[3]);
		rwa.isexp = atob(Row[4]);
		rwa.ismesos = atob(Row[5]);
		rwa.isfame = atob(Row[6]);
		rwa.isskill = atob(Row[7]);
		rwa.id = atoi(Row[8]);
		rwa.count = atoi(Row[9]);
		rwa.gender = atoi(Row[10]);
		rwa.job = atoi(Row[11]);
		rwa.prop = atoi(Row[12]);
		rwas.push_back(rwa);

		previousid = currentid;
	}
	if (previousid != -1) {
		Quests::addReward(previousid, rwas);
		rwas.clear();
	}
	std::cout << "DONE" << std::endl;
}

void Initializing::initializeSkills() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Skills... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM skilldata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW skillRow;
	while (skillRow = res.fetch_raw_row()) {
		// Col0 : Skill ID
		//    1 : Level
		//    2 : Time
		//    3 : MP
		//    4 : HP
		//    5 : Damage
		//    6 : Item
		//    7 : Item Count
		//    8 : Bullet Consume
		//    9 : Money Consume
		//   10 : Value X
		//	 11 : Value Y
		//   12 : Speed
		//   13 : Jump
		//   14 : Weapon Attack
		//   15 : Weapon Defense
		//   16 : Magic Attack
		//   17 : Magic Defense
		//   18 : Accuracy
		//   19 : Avoid
		//   20 : HPP
		//   21 : Prop(% chance)
		//   22 : Morph
		//   23 : Left Top X
		//   24 : Left Top Y
		//   25 : Right Bottom X
		//   26 : Right Bottom Y
		//   27 : Cooldown time
		SkillLevelInfo level;
		level.time = atoi(skillRow[2]);
		level.mp = atoi(skillRow[3]);
		level.hp = atoi(skillRow[4]);
		level.damage = atoi(skillRow[5]);
		level.item = atoi(skillRow[6]);
		level.itemcount = atoi(skillRow[7]);
		level.bulletcon = atoi(skillRow[8]);
		level.moneycon = atoi(skillRow[9]);
		level.x = atoi(skillRow[10]);
		level.y = atoi(skillRow[11]);
		level.speed = atoi(skillRow[12]);
		level.jump = atoi(skillRow[13]);
		level.watk = atoi(skillRow[14]);
		level.wdef = atoi(skillRow[15]);
		level.matk = atoi(skillRow[16]);
		level.mdef = atoi(skillRow[17]);
		level.acc = atoi(skillRow[18]);
		level.avo = atoi(skillRow[19]);
		level.hpP = atoi(skillRow[20]);
		level.prop = atoi(skillRow[21]);
		level.morph = atoi(skillRow[22]);
		level.lt = Pos(atoi(skillRow[23]), atoi(skillRow[24]));
		level.rb = Pos(atoi(skillRow[25]), atoi(skillRow[26]));
		level.cooltime = atoi(skillRow[27]);
		Skills::addSkillLevelInfo(atoi(skillRow[0]), atoi(skillRow[1]), level);
	}

	query << "SELECT mobskills.*, mobskillsummons.mobid FROM mobskills LEFT JOIN mobskillsummons ON mobskills.level = mobskillsummons.level AND mobskills.skillid = 200 ORDER BY skillid ASC";
	res = query.use();

	int16_t currentid = 0;
	int16_t previousid = -1;
	int16_t currentlevel = 0;
	int16_t previouslevel = -1;

	MobSkillLevelInfo moblevel;

	while (skillRow = res.fetch_raw_row()) {
		// Col0 : Skill ID
		//    1 : Level
		//    2 : Time
		//    3 : MP
		//    4 : X
		//    5 : Y
		//    6 : Prop
		//    7 : Count
		//    8 : Interval
		//    9 : LTX
		//   10 : RBX
		//   11 : LTY
		//   12 : RBY
		//   13 : HP
		//   14 : Limit
		//   15 : Summon Effect
		//   16 : Mob IDs for summons
		currentid = atoi(skillRow[0]);
		currentlevel = atoi(skillRow[1]);

		if ((currentid != previousid && previousid != -1) || (currentlevel != previouslevel && previouslevel != -1)) { // Add the items into the cache
			Skills::addMobSkillLevelInfo((uint8_t)(previousid), (uint8_t)(previouslevel), moblevel);
			moblevel.summons.clear();
		}
		moblevel.time = atoi(skillRow[2]);
		moblevel.mp = atoi(skillRow[3]);
		moblevel.x = atoi(skillRow[4]);
		moblevel.y = atoi(skillRow[5]);
		moblevel.prop = atoi(skillRow[6]);
		moblevel.count = atoi(skillRow[7]);
		moblevel.interval = atoi(skillRow[8]);
		moblevel.lt.x = atoi(skillRow[9]);
		moblevel.rb.x = atoi(skillRow[10]);
		moblevel.lt.y = atoi(skillRow[11]);
		moblevel.rb.y = atoi(skillRow[12]);
		moblevel.hp = atoi(skillRow[13]);
		moblevel.limit = atoi(skillRow[14]);
		moblevel.summoneffect = atoi(skillRow[15]);

		if (skillRow[16] != 0) {
			moblevel.summons.push_back(atoi(skillRow[16]));
		}

		previousid = currentid;
		previouslevel = currentlevel;
	}
	std::cout << "DONE" << std::endl;
}

void Initializing::initializePets() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Pets... ";

	mysqlpp::Query query = Database::getDataDB().query("SELECT id, name, hunger FROM petdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW petRow;
	while (petRow = res.fetch_raw_row()) {
		// 0 : Pet id
		// 1 : Pet breed name
		// 2 : Pet hunger level
		PetInfo pet;
		pet.name = petRow[1];
		pet.hunger = atoi(petRow[2]);
		Pets::petsInfo[atoi(petRow[0])] = pet;
	}
	
	// Pet command info
	query << "SELECT * FROM petinteractdata";
	res = query.use();

	MYSQL_ROW petInteractRow;
	while (petInteractRow = res.fetch_raw_row()) {
		// 0 : Id
		// 1 : Command
		// 2 : Increase
		// 3 : Prob
		PetInteractInfo pet;
		pet.increase = atoi(petInteractRow[2]);
		pet.prob = atoi(petInteractRow[3]);
		Pets::petsInteractInfo[atoi(petInteractRow[0])][atoi(petInteractRow[1])] = pet;
	}
	std::cout << "DONE" << std::endl;
}

void Initializing::initializeChat() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Commands... ";
	ChatHandler::initializeCommands();
	std::cout << "DONE" << std::endl;
}
