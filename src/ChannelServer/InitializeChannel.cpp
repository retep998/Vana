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
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "ItemDataProvider.h"
#include "MapleVersion.h"
#include "MiscUtilities.h"
#include "MobDataProvider.h"
#include "Pets.h"
#include "Quests.h"
#include "Reactors.h"
#include "ScriptDataProvider.h"
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
	ScriptDataProvider::Instance()->loadData();
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
	ReactorEventInfo revent;

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
		//   10 : Repeat
		//   11 : Timeout

		int32_t id = atoi(reactorRow[1]);
		uint8_t state = atoi(reactorRow[2]);

		revent.type = atoi(reactorRow[3]);
		revent.itemid = atoi(reactorRow[4]);
		revent.lt.x = atoi(reactorRow[5]);
		revent.lt.y = atoi(reactorRow[6]);
		revent.rb.x = atoi(reactorRow[7]);
		revent.rb.y = atoi(reactorRow[8]);
		revent.nextstate = atoi(reactorRow[9]);
		revent.repeat = atob(reactorRow[10]);
		revent.timeout = atoi(reactorRow[11]);

		Reactors::setMaxStates(id, revent.nextstate);
		Reactors::addEventInfo(id, state, revent);
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
		"SELECT questid, oid, count FROM questrequestdata WHERE mob = 1 ORDER BY questid, id ASC",
		"SELECT questid, oid, count FROM questrequestdata WHERE item = 1 ORDER BY questid, id ASC",
		"SELECT questid, oid, count FROM questrequestdata WHERE quest = 1 ORDER BY questid, id ASC"
	};
	int32_t reqtypes[QuestRequestTypes::Count] = {
		QuestRequestTypes::Mob,
		QuestRequestTypes::Item,
		QuestRequestTypes::Quest
	};
	int32_t currentid = 0;
	int32_t previousid = -1;
	map<int32_t, int16_t, std::less<int32_t> > reqs;

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
	QuestRewardInfo rwa;
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
		//    8 : Only master level
		//    9 : Buff
		//   10 : Object ID
		//   11 : Count
		//   12 : Master level
		//   13 : Gender
		//   14 : Job
		//   15 : Prop

		currentid = atoi(Row[1]);

		if (currentid != previousid && previousid != -1) {
			Quests::addReward(previousid, rwas);
			rwas.rewards.clear();
			rwas.jobrewards.clear();
		}

		int32_t job = atoi(Row[14]);

		rwa.start = atob(Row[2]);
		rwa.isitem = atob(Row[3]);
		rwa.isexp = atob(Row[4]);
		rwa.ismesos = atob(Row[5]);
		rwa.isfame = atob(Row[6]);
		rwa.isskill = atob(Row[7]);
		rwa.ismasterlevelonly = atob(Row[8]);
		rwa.isbuff = atob(Row[9]);
		rwa.id = atoi(Row[10]);
		rwa.count = atoi(Row[11]);
		rwa.masterlevel = atoi(Row[11]); 
		rwa.gender = atoi(Row[13]);
		rwa.prop = atoi(Row[15]);

		if (job == -1) {
			rwas.rewards.push_back(rwa);
		}
		else if (GameLogicUtilities::isNonBitJob(job)) {
			rwas.jobrewards[job].push_back(rwa);
		}
		else { // Job tracks are indicated by series of bits between 1 and 32 and 1000+
			 // Beginners
			if ((job & 0x01) != 0) {
				rwas.jobrewards[Jobs::JobIds::Beginner].push_back(rwa);
			}
			if ((job & 0x400) != 0) { // Not sure if this is the right parsing, but I think it is
				rwas.jobrewards[Jobs::JobIds::Noblesse].push_back(rwa);
			}
			// Warriors
			if ((job & 0x02) != 0) {
				rwas.jobrewards[Jobs::JobIds::Swordsman].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Fighter].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Crusader].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Hero].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Page].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::WhiteKnight].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Paladin].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Spearman].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::DragonKnight].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::DarkKnight].push_back(rwa);
			}
			if ((job & 0x800) != 0) {
				rwas.jobrewards[Jobs::JobIds::DawnWarrior1].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::DawnWarrior2].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::DawnWarrior3].push_back(rwa);
			}
			// Magicians
			if ((job & 0x04) != 0) {
				rwas.jobrewards[Jobs::JobIds::Magician].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::FPWizard].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::FPMage].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::FPArchMage].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::ILWizard].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::ILMage].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::ILArchMage].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Cleric].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Priest].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Bishop].push_back(rwa);
			}
			if ((job & 0x1000) != 0) {
				rwas.jobrewards[Jobs::JobIds::BlazeWizard1].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::BlazeWizard2].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::BlazeWizard3].push_back(rwa);
			}
			// Bowmen
			if ((job & 0x08) != 0) {
				rwas.jobrewards[Jobs::JobIds::Archer].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Hunter].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Ranger].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Bowmaster].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Crossbowman].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Sniper].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Marksman].push_back(rwa);
			}
			if ((job & 0x2000) != 0) {
				rwas.jobrewards[Jobs::JobIds::WindArcher1].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::WindArcher2].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::WindArcher3].push_back(rwa);
			}
			// Thieves
			if ((job & 0x10) != 0) {
				rwas.jobrewards[Jobs::JobIds::Rogue].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Assassin].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Hermit].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::NightLord].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Bandit].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::ChiefBandit].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Shadower].push_back(rwa);
			}
			if ((job & 0x4000) != 0) {
				rwas.jobrewards[Jobs::JobIds::NightWalker1].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::NightWalker2].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::NightWalker3].push_back(rwa);
			}
			// Pirates
			if ((job & 0x20) != 0) {
				rwas.jobrewards[Jobs::JobIds::Pirate].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Brawler].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Marauder].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Buccaneer].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Gunslinger].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Outlaw].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::Corsair].push_back(rwa);
			}
			if ((job & 0x8000) != 0) {
				rwas.jobrewards[Jobs::JobIds::ThunderBreaker1].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::ThunderBreaker2].push_back(rwa);
				rwas.jobrewards[Jobs::JobIds::ThunderBreaker3].push_back(rwa);
			}
		}

		previousid = currentid;
	}
	if (previousid != -1) {
		Quests::addReward(previousid, rwas);
	}
	std::cout << "DONE" << std::endl;
}

void Initializing::initializeSkills() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Skills... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM skilldata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW skillRow;
	SkillLevelInfo level;

	enum SkillRows {
		SkillId = 0,
		Level, MobCount, HitCount, Time, Mp,
		Hp, Damage, Item, ItemCount, BulletCon,
		MoneyCon, X, Y, Speed, Jump,
		Watk, Wdef, Matk, Mdef, Acc,
		Avoid, HpP, MpP, Prop, Morph,
		LTX, LTY, RBX, RBY, CoolTime
	};

	while (skillRow = res.fetch_raw_row()) {
		level.mobcount = atoi(skillRow[MobCount]);
		level.hitcount = atoi(skillRow[HitCount]);
		level.time = atoi(skillRow[Time]);
		level.mp = atoi(skillRow[Mp]);
		level.hp = atoi(skillRow[Hp]);
		level.damage = atoi(skillRow[Damage]);
		level.item = atoi(skillRow[Item]);
		level.itemcount = atoi(skillRow[ItemCount]);
		level.bulletcon = atoi(skillRow[BulletCon]);
		level.moneycon = atoi(skillRow[MoneyCon]);
		level.x = atoi(skillRow[X]);
		level.y = atoi(skillRow[Y]);
		level.speed = atoi(skillRow[Speed]);
		level.jump = atoi(skillRow[Jump]);
		level.watk = atoi(skillRow[Watk]);
		level.wdef = atoi(skillRow[Wdef]);
		level.matk = atoi(skillRow[Matk]);
		level.mdef = atoi(skillRow[Mdef]);
		level.acc = atoi(skillRow[Acc]);
		level.avo = atoi(skillRow[Avoid]);
		level.hpP = atoi(skillRow[HpP]);
		level.mpP = atoi(skillRow[MpP]);
		level.prop = atoi(skillRow[Prop]);
		level.morph = atoi(skillRow[Morph]);
		level.lt = Pos(atoi(skillRow[LTX]), atoi(skillRow[LTY]));
		level.rb = Pos(atoi(skillRow[RBX]), atoi(skillRow[RBY]));
		level.cooltime = atoi(skillRow[CoolTime]);

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
	if (previousid != -1) {
		Skills::addMobSkillLevelInfo((uint8_t)(previousid), (uint8_t)(previouslevel), moblevel);
	}

	std::cout << "DONE" << std::endl;
}

void Initializing::initializePets() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Pets... ";

	mysqlpp::Query query = Database::getDataDB().query("SELECT id, name, hunger FROM petdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW Row;
	while (Row = res.fetch_raw_row()) {
		// 0 : Pet id
		// 1 : Pet breed name
		// 2 : Pet hunger level

		PetInfo pet;
		pet.name = Row[1];
		pet.hunger = atoi(Row[2]);
		Pets::petsInfo[atoi(Row[0])] = pet;
	}
	
	// Pet command info
	query << "SELECT * FROM petinteractdata";
	res = query.use();

	PetInteractInfo pet;
	while (Row = res.fetch_raw_row()) {
		// 0 : Id
		// 1 : Command
		// 2 : Increase
		// 3 : Prob

		pet.increase = atoi(Row[2]);
		pet.prob = atoi(Row[3]);
		Pets::petsInteractInfo[atoi(Row[0])][atoi(Row[1])] = pet;
	}
	std::cout << "DONE" << std::endl;
}

void Initializing::initializeChat() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Commands... ";
	ChatHandler::initializeCommands();
	std::cout << "DONE" << std::endl;
}
