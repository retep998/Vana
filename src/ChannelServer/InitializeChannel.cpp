/*
Copyright (C) 2008 Vana Development Team

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
#include "InitializeCommon.h"
#include "Buffs.h"
#include "Mobs.h"
#include "Drops.h"
#include "Reactors.h"
#include "Shops.h"
#include "Quests.h"
#include "Skills.h"
#include "Inventory.h"
#include "Database.h"
#include "MiscUtilities.h"
#include "Pets.h"
#include <iostream>

using std::string;
using MiscUtilities::atob;

void Initializing::checkVEDBVersion() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM mcdb_info LIMIT 1");
	mysqlpp::StoreQueryResult res = query.store();

	int32_t version = (int32_t) res[0]["version"];
	int32_t subversion = (int32_t) res[0]["subversion"];

	if (version != mcdb_version || subversion < mcdb_subversion) {
		// VEDB too old
		std::cout << "ERROR: MCDB too old. Expected: " << mcdb_version << "." << mcdb_subversion << " ";
		std::cout << "Have: " << version << "." << subversion << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(4);
	}
}

// Mobs
void Initializing::initializeMobs() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Mobs... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT mobdata.mobid, mobdata.level, mobdata.hp, mobdata.mp, mobdata.elemAttr, mobdata.hprecovery, mobdata.mprecovery, mobdata.exp, mobdata.boss, mobdata.hpcolor, mobdata.hpbgcolor, mobsummondata.summonid FROM mobdata LEFT JOIN mobsummondata ON mobdata.mobid=mobsummondata.mobid ORDER BY mobdata.mobid ASC");
	mysqlpp::UseQueryResult res = query.use();

	int32_t currentid = 0;
	int32_t previousid = -1;
	MobInfo mob;
	MYSQL_ROW mobRow;
	while ((mobRow = res.fetch_raw_row())) {
		// Col0 : Mob ID
		//    1 : Level
		//    2 : HP
		//    3 : MP
		//    4 : Elemental Attributes
		//    5 : HP Recovery
		//    6 : MP Recovery
		//    7 : EXP
		//    8 : Boss
		//    9 : HP Color
		//   10 : HP BG Color
		//   11 : Mob Summon
		currentid = atoi(mobRow[0]);

		if (currentid != previousid && previousid != -1) {
			Mobs::addMob(previousid, mob);
			mob.summon.clear();
			mob.skills.clear();
		}
		mob.level = atoi(mobRow[1]);
		mob.hp = atoi(mobRow[2]);
		mob.mp = atoi(mobRow[3]);
		string elemattr(mobRow[4]);
		mob.hprecovery = atoi(mobRow[5]);
		mob.mprecovery = atoi(mobRow[6]);
		mob.exp = atoi(mobRow[7]);
		mob.boss = atob(mobRow[8]);
		mob.hpcolor = atoi(mobRow[9]);
		mob.hpbgcolor = atoi(mobRow[10]);

		mob.canfreeze = (!mob.boss && elemattr.find("I2") == string::npos && elemattr.find("I1") == string::npos);
		mob.canpoision = (!mob.boss && elemattr.find("S2") == string::npos && elemattr.find("S1") == string::npos);

		if (mobRow[11] != 0) {
			mob.summon.push_back(atoi(mobRow[11]));
		}

		previousid = currentid;
	}
	// Add final entry
	if (previousid != -1) {
		Mobs::addMob(previousid, mob);
	}

	query << "SELECT mobid, attackid, mpconsume, mpburn, disease, level, deadly FROM mobattackdata";
	res = query.use();

	while ((mobRow = res.fetch_raw_row())) {
		// Col0 : Mob ID
		//    1 : Attack ID
		//    2 : MP Consumption
		//    3 : MP Burn
		//    4 : Disease
		//    5 : Level
		//    6 : Deadly
		MobAttackInfo mobattack;
		mobattack.id = atoi(mobRow[1]);
		mobattack.mpconsume = atoi(mobRow[2]);
		mobattack.mpburn = atoi(mobRow[3]);
		mobattack.disease = atoi(mobRow[4]);
		mobattack.level = atoi(mobRow[5]);
		mobattack.deadlyattack = atob(mobRow[6]);
		Mobs::mobinfo[atoi(mobRow[0])].skills.push_back(mobattack);
	}

	std::cout << "DONE" << std::endl;
}
// Reactors
void Initializing::initializeReactors() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Reactors... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM reactoreventdata ORDER BY reactorid, state ASC");
	mysqlpp::UseQueryResult res = query.use();

	int32_t currentid = 0;
	int32_t previousid = -1;
	MYSQL_ROW reactorRow;
	while ((reactorRow = res.fetch_raw_row())) {
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
		revent.ltx = atoi(reactorRow[5]);
		revent.lty = atoi(reactorRow[6]);
		revent.rbx = atoi(reactorRow[7]);
		revent.rby = atoi(reactorRow[8]);
		revent.nextstate = atoi(reactorRow[9]);
		Reactors::setMaxstates(atoi(reactorRow[1]), revent.nextstate);
		Reactors::addEventInfo(atoi(reactorRow[1]), revent);
	}

	std::cout << "DONE" << std::endl;
}
// Items
void Initializing::initializeItems() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Items... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT itemdata.*, itemsummondata.mobid, itemsummondata.chance FROM itemdata LEFT JOIN itemsummondata ON itemdata.itemid=itemsummondata.itemid ORDER BY itemid ASC");
	mysqlpp::UseQueryResult res = query.use();

	int32_t currentid = 0;
	int32_t previousid = -1;
	ItemInfo item;
	MYSQL_ROW itemRow;
	while ((itemRow = res.fetch_raw_row())) {
		// Col0 : Item ID
		//    1 : Price
		//    2 : Max per slot
		//    3 : Not tradable?
		//    4 : Quest item?
		//    5 : HP
		//    6 : MP
		//    7 : HP Rate
		//    8 : MP Rate
		//    9 : Move to
		//   10 : Time
		//   11 : Weapon Attack
		//   12 : Magic Attack
		//   13 : Avoidability
		//   14 : Accuracy
		//   15 : Weapon Defense
		//   16 : Magic Defense
		//   17 : Speed
		//   18 : Jump
		//   19 : Morph
		//   20 : Success
		//   21 : Cursed
		//   22 : Item STR
		//   23 : Item DEX
		//   24 : Item INT
		//   25 : Item LUK
		//   26 : Item HP
		//   27 : Item MP
		//   28 : Item Weapon Attack
		//   29 : Item Magic Attack
		//   30 : Item Weapon Defense
		//   31 : Item Magic Defense
		//   32 : Item Accuracy
		//   33 : Item Avoid
		//   34 : Item Jump
		//   35 : Item Speed
		//   36 : Mob ID
		//   37 : Chance
		currentid = atoi(itemRow[0]);

		if (currentid != previousid && previousid != -1) { // Add the items into the cache
			Inventory::addItemInfo(previousid, item);
			item.cons.mobs.clear();
		}
		item.price = atoi(itemRow[1]);
		item.maxslot = atoi(itemRow[2]);
		item.notrade = atob(itemRow[3]);
		item.quest = atob(itemRow[4]);
		item.cons.hp = atoi(itemRow[5]);
		item.cons.mp = atoi(itemRow[6]);
		item.cons.hpr = atoi(itemRow[7]);
		item.cons.mpr = atoi(itemRow[8]);
		item.cons.moveTo = atoi(itemRow[9]);
		// Buffs
		item.cons.time = atoi(itemRow[10]);
		item.cons.watk = atoi(itemRow[11]);
		item.cons.matk = atoi(itemRow[12]);
		item.cons.avo = atoi(itemRow[13]);
		item.cons.acc = atoi(itemRow[14]);
		item.cons.wdef = atoi(itemRow[15]);
		item.cons.mdef = atoi(itemRow[16]);
		item.cons.speed = atoi(itemRow[17]);
		item.cons.jump = atoi(itemRow[18]);
		item.cons.morph = atoi(itemRow[19]);
		// Scrolling
		item.cons.success = atoi(itemRow[20]);
		item.cons.cursed = atoi(itemRow[21]);
		item.cons.istr = atoi(itemRow[22]);
		item.cons.idex = atoi(itemRow[23]);
		item.cons.iint = atoi(itemRow[24]);
		item.cons.iluk = atoi(itemRow[25]);
		item.cons.ihp = atoi(itemRow[26]);
		item.cons.imp = atoi(itemRow[27]);
		item.cons.iwatk = atoi(itemRow[28]);
		item.cons.imatk = atoi(itemRow[29]);
		item.cons.iwdef = atoi(itemRow[30]);
		item.cons.imdef = atoi(itemRow[31]);
		item.cons.iacc = atoi(itemRow[32]);
		item.cons.iavo = atoi(itemRow[33]);
		item.cons.ijump = atoi(itemRow[34]);
		item.cons.ispeed = atoi(itemRow[35]);
		item.cons.ihand = 0;
		// Summoning
		if (itemRow[36] != 0) {
			SummonBag summon;
			summon.mobid = atoi(itemRow[36]);
			summon.chance = atoi(itemRow[37]);
			item.cons.mobs.push_back(summon);
		}

		previousid = currentid;
	}
	// Add the final entry
	if (previousid != -1) {
		Inventory::addItemInfo(previousid, item);
		item.cons.mobs.clear();
	}

	// Item Skills
	query << "SELECT * FROM itemskilldata";
	res = query.use();

	MYSQL_ROW itemSkillRow;
	while ((itemSkillRow = res.fetch_raw_row())) {
		// Col0 : Item ID
		//    1 : Skill ID
		//    2 : Required Level
		//    3 : Master Level
		Skillbook skill;
		skill.skillid = atoi(itemSkillRow[1]);
		skill.reqlevel = atoi(itemSkillRow[2]);
		skill.maxlevel = atoi(itemSkillRow[3]);
		Inventory::items[atoi(itemSkillRow[0])].cons.skills.push_back(skill);
	}
	std::cout << "DONE" << std::endl;
}
// Drops
void Initializing::initializeDrops() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Drops... ";
	// Mob drops
	mysqlpp::Query query = Database::getDataDB().query("SELECT mobid, itemid, chance, quest, mesos, min, max FROM mobdropdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW dropRow;
	while ((dropRow = res.fetch_raw_row())) {
		// Col0 : Mob ID
		//    1 : Item ID
		//    2 : Chance
		//    3 : Quest
		//    4 : Mesos?
		//    5 : Min mesos
		//    6 : Max mesos

		DropInfo drop;
		drop.id = atoi(dropRow[1]);
		drop.chance = atoi(dropRow[2]);
		drop.quest = atoi(dropRow[3]);
		drop.ismesos = atob(dropRow[4]);
		drop.minmesos = atoi(dropRow[5]);
		drop.maxmesos = atoi(dropRow[6]);
		Drops::addDropData(atoi(dropRow[0]), drop);
	}

	// Reactor drops
	query << "SELECT reactorid, itemid, chance, quest, mesos, min, max FROM reactordropdata";
	res = query.use();

	while ((dropRow = res.fetch_raw_row())) {
		// Col0 : Reactor ID
		//    1 : Item ID
		//    2 : Chance
		//    3 : Quest
		//    4 : Mesos?
		//    5 : Min mesos
		//    6 : Max mesos

		DropInfo drop;
		drop.id = atoi(dropRow[1]);
		drop.chance = atoi(dropRow[2]);
		drop.quest = atoi(dropRow[3]);
		drop.ismesos = atob(dropRow[4]);
		drop.minmesos = atoi(dropRow[5]);
		drop.maxmesos = atoi(dropRow[6]);
		Drops::addDropData(atoi(dropRow[0]), drop);
	}

	std::cout << "DONE" << std::endl;
}
// Equips
void Initializing::initializeEquips() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Equips... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM equipdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW equipRow;
	while ((equipRow = res.fetch_raw_row())) {
		// Col0 : EquipID
		//    1 : Price
		//    2 : Slots
		//    3 : HP
		//    4 : MP
		//    5 : STR
		//    6 : DEX
		//    7 : INT
		//    8 : LUK
		//    9 : WAtk
		//   10 : WDef
		//   11 : MAtk
		//   12 : MDef
		//   13 : Acc
		//   14 : Avo
		//   15 : Jump
		//   16 : Speed
		//   17 : Taming Mob
		//   18 : Only one?
		//   19 : No trading?
		//   20 : Quest item?
		EquipInfo equip = EquipInfo();
		equip.price = atoi(equipRow[1]);
		equip.slots = atoi(equipRow[2]);
		equip.ihp = atoi(equipRow[3]);
		equip.imp = atoi(equipRow[4]);
		equip.istr = atoi(equipRow[5]);
		equip.idex = atoi(equipRow[6]);
		equip.iint = atoi(equipRow[7]);
		equip.iluk = atoi(equipRow[8]);
		equip.iwatk = atoi(equipRow[9]);
		equip.iwdef = atoi(equipRow[10]);
		equip.imatk = atoi(equipRow[11]);
		equip.imdef = atoi(equipRow[12]);
		equip.iacc = atoi(equipRow[13]);
		equip.iavo = atoi(equipRow[14]);
		equip.ijump = atoi(equipRow[15]);
		equip.ispeed = atoi(equipRow[16]);
		equip.tamingmob = atoi(equipRow[17]);
		equip.onlyone = atob(equipRow[18]);
		equip.notrade = atob(equipRow[19]);
		equip.quest = atob(equipRow[20]);
		equip.ihand = 0;
		// Add equip to the equip info table
		Inventory::addEquipInfo(atoi(equipRow[0]), equip);
	}
	std::cout << "DONE" << std::endl;
}
// Shops
void Initializing::initializeShops() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Shops... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT shopdata.shopid, shopdata.npcid, shopitemdata.itemid, shopitemdata.price FROM shopdata LEFT JOIN shopitemdata ON shopdata.shopid=shopitemdata.shopid ORDER BY shopdata.shopid ASC, shopitemdata.sort DESC");
	mysqlpp::UseQueryResult res = query.use();

	int32_t currentid = 0;
	int32_t previousid = -1;
	ShopInfo shop;
	MYSQL_ROW shopRow;
	while ((shopRow = res.fetch_raw_row())) {
		// Col0 : Shop ID
		//    1 : NPC ID
		//    2 : Item ID
		//    3 : Price
		currentid = atoi(shopRow[0]);

		if (currentid != previousid && previousid != -1) {
			Shops::addShop(previousid, shop);
			shop = ShopInfo();
		}
		shop.npc = atoi(shopRow[1]);
		if (shopRow[2] != 0) {
			shop.items.push_back(atoi(shopRow[2]));
			shop.prices[atoi(shopRow[2])] = atoi(shopRow[3]);
		}
		else std::cout << "Warning: Shop " << currentid << " does not have any shop items on record.";

		previousid = atoi(shopRow[0]);
	}
	// Add final entry
	if (previousid != -1) {
		Shops::addShop(previousid, shop);
		shop.items.clear();
	}
	std::cout << "DONE" << std::endl;
}
// Quests
void Initializing::initializeQuests() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Quests... ";
	// Quests
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM questdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW questRow;
	while ((questRow = res.fetch_raw_row())) {
		// Col0 : Quest ID
		//    1 : Next Quest ID
		Quests::setNextQuest(atoi(questRow[0]), atoi(questRow[1]));
	}

	// Quest Requests
	query << "SELECT * FROM questrequestdata ORDER BY questid ASC";
	res = query.use();

	int32_t currentid = 0;
	int32_t previousid = -1;
	QuestRequestsInfo reqs;
	MYSQL_ROW requestRow;
	while ((requestRow = res.fetch_raw_row())) {
		// Col0 : Row ID
		//    1 : Quest ID
		//    2 : Mob
		//    3 : Item
		//    4 : Quest
		//    5 : Object ID
		//    6 : Count
		currentid = atoi(requestRow[1]);

		if (currentid != previousid && previousid != -1) {
			Quests::addRequest(previousid, reqs);
			reqs.clear();
		}

		QuestRequestInfo req;
		req.ismob = atob(requestRow[2]);
		req.isitem = atob(requestRow[3]);
		req.isquest = atob(requestRow[4]);
		req.id = atoi(requestRow[5]);
		req.count = atoi(requestRow[6]);
		reqs.push_back(req);

		previousid = atoi(requestRow[1]);
	}
	if (previousid != -1) {
		Quests::addRequest(previousid, reqs);
		reqs.clear();
	}

	// Quest Rewards
	query << "SELECT * FROM questrewarddata ORDER BY questid ASC";
	res = query.use();

	currentid = 0;
	previousid = -1;
	QuestRewardsInfo rwas;
	MYSQL_ROW rewardRow;
	while ((rewardRow = res.fetch_raw_row())) {
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
		currentid = atoi(rewardRow[1]);

		if (currentid != previousid && previousid != -1) {
			Quests::addReward(previousid, rwas);
			rwas.clear();
		}

		QuestRewardInfo rwa;
		rwa.start = atob(rewardRow[2]);
		rwa.isitem = atob(rewardRow[3]);
		rwa.isexp = atob(rewardRow[4]);
		rwa.ismesos = atob(rewardRow[5]);
		rwa.isfame = atob(rewardRow[6]);
		rwa.isskill = atob(rewardRow[7]);
		rwa.id = atoi(rewardRow[8]);
		rwa.count = atoi(rewardRow[9]);
		rwa.gender = atoi(rewardRow[10]);
		rwa.job = atoi(rewardRow[11]);
		rwa.prop = atoi(rewardRow[12]);
		rwas.push_back(rwa);

		previousid = atoi(rewardRow[1]);
	}
	if (previousid != -1) {
		Quests::addReward(previousid, rwas);
		rwas.clear();
	}
	std::cout << "DONE" << std::endl;
}
// Skills
void Initializing::initializeSkills() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Skills... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM skilldata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW skillRow;
	SkillsLevelInfo skill;
	while ((skillRow = res.fetch_raw_row())) {
		// Col0 : Skill ID
		//    1 : Level
		//    2 : Time
		//    3 : MP
		//    4 : HP
		//    5 : Item
		//    6 : Item Count
		//    7 : Bullet Consume
		//    8 : Money Consume
		//    9 : Value X
		//	 10 : Value Y
		//   11 : Speed
		//   12 : Jump
		//   13 : Weapon Attack
		//   14 : Weapon Defense
		//   15 : Magic Attack
		//   16 : Magic Defense
		//   17 : Accuracy
		//   18 : Avoid
		//   19 : HPP
		//   20 : Prop(% chance)
		//   21 : Morph
		//   22 : Left Top X
		//   23 : Left Top Y
		//   24 : Right Bottom X
		//   25 : Right Bottom Y
		//   26 : Cooldown time
		SkillLevelInfo level;
		level.time = atoi(skillRow[2]);
		level.mp = atoi(skillRow[3]);
		level.hp = atoi(skillRow[4]);
		level.item = atoi(skillRow[5]);
		level.itemcount = atoi(skillRow[6]);
		level.bulletcon = atoi(skillRow[7]);
		level.moneycon = atoi(skillRow[8]);
		level.x = atoi(skillRow[9]);
		level.y = atoi(skillRow[10]);
		level.speed = atoi(skillRow[11]);
		level.jump = atoi(skillRow[12]);
		level.watk = atoi(skillRow[13]);
		level.wdef = atoi(skillRow[14]);
		level.matk = atoi(skillRow[15]);
		level.mdef = atoi(skillRow[16]);
		level.acc = atoi(skillRow[17]);
		level.avo = atoi(skillRow[18]);
		level.hpP = atoi(skillRow[19]);
		level.prop = atoi(skillRow[20]);
		level.morph = atoi(skillRow[21]);
		level.lt = Pos(atoi(skillRow[22]), atoi(skillRow[23]));
		level.rb = Pos(atoi(skillRow[24]), atoi(skillRow[25]));
		level.cooltime = atoi(skillRow[26]);
		Skills::addSkillLevelInfo(atoi(skillRow[0]), atoi(skillRow[1]), level);
	}
	Buffs::init();
	std::cout << "DONE" << std::endl;
}
// Pets
void Initializing::initializePets() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Pets... ";

	mysqlpp::Query query = Database::getDataDB().query("SELECT id, name, hunger FROM petdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW petRow;
	while ((petRow = res.fetch_raw_row())) {
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
	while ((petInteractRow = res.fetch_raw_row())) {
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
