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
#include "Mobs.h"
#include "Drops.h"
#include "Reactors.h"
#include "Maps.h"
#include "NPCs.h"
#include "Shops.h"
#include "Quests.h"
#include "Skills.h"
#include "Inventory.h"
#include "MySQLM.h"
#include "MiscUtilities.h"
#include <iostream>
#include <string>

using std::string;
using MiscUtilities::atob;

void Initializing::checkVEDBVersion() {
	db.select_db("vedb");
	mysqlpp::Query query = db.query("SELECT * FROM vedb_info LIMIT 1");
	mysqlpp::StoreQueryResult res;

	if (!(res = query.store())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	int version = (int) res[0]["version"];
	int subversion = (int) res[0]["subversion"];

	if (version != vedb_version || subversion < vedb_subversion) {
		// VEDB too old
		std::cout << "ERROR: VEDB too old. Expected: " << vedb_version << "." << vedb_subversion << " ";
		std::cout << "Have: " << version << "." << subversion << std::endl;
		exit(4);
	}
}

// Mobs
void Initializing::initializeMobs() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Mobs... ";
	mysqlpp::Query query = db.query("SELECT mobdata.mobid, mobdata.hp, mobdata.mp, mobdata.exp, mobdata.boss, mobdata.hpcolor, mobdata.hpbgcolor, mobsummondata.summonid FROM mobdata LEFT JOIN mobsummondata ON mobdata.mobid=mobsummondata.mobid ORDER BY mobdata.mobid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	MobInfo mob;
	MYSQL_ROW mobRow;
	while ((mobRow = res.fetch_raw_row())) {
		// Col0 : Mob ID
		//    1 : HP
		//    2 : MP
		//    3 : EXP
		//    4 : Boss
		//    5 : HP Color
		//    6 : HP BG Color
		//    7 : Mob Summon
		currentid = atoi(mobRow[0]);

		if (currentid != previousid && previousid != -1) {
			Mobs::addMob(previousid, mob);
			mob.summon.clear();
		}
		mob.hp  = atoi(mobRow[1]);
		mob.mp  = atoi(mobRow[2]);
		mob.exp = atoi(mobRow[3]);
		mob.boss = atob(mobRow[4]);
		mob.hpcolor = atoi(mobRow[5]);
		mob.hpbgcolor = atoi(mobRow[6]);

		if (mobRow[7] != 0) {
			mob.summon.push_back(atoi(mobRow[7]));
		}
		previousid = atoi(mobRow[0]);
	}
	// Add final entry
	if (previousid != -1) {
		Mobs::addMob(previousid, mob);
	}

	std::cout << "DONE" << std::endl;
}
// Reactors
void Initializing::initializeReactors() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Reactors... ";
	mysqlpp::Query query = db.query("SELECT * FROM reactoreventdata ORDER BY reactorid, state ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
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
	mysqlpp::Query query = db.query("SELECT itemdata.*, itemsummondata.mobid, itemsummondata.chance FROM itemdata LEFT JOIN itemsummondata ON itemdata.itemid=itemsummondata.itemid ORDER BY itemid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	ConsumeInfo cons;
	ItemInfo item;
	SummonBag s;
	MYSQL_ROW itemRow;
	while ((itemRow = res.fetch_raw_row())) {
		// Col0 : Item ID
		//    1 : Type
		//    2 : Price
		//    3 : Slots
		//    4 : Quest
		//    5 : Consume
		//    6 : HP
		//    7 : MP
		//    8 : HP Rate
		//    9 : MP Rate
		//   10 : Move to
		//   11 : Time
		//   12 : Weapon Attack
		//   13 : Magic Attack
		//   14 : Avoidability
		//   15 : Accuracy
		//   16 : Weapon Defense
		//   17 : Magic Defense
		//   18 : Speed
		//   19 : Jump
		//   20 : Morph
		//   21 : Success
		//   22 : Cursed
		//   23 : Item STR
		//   24 : Item DEX
		//   25 : Item INT
		//   26 : Item LUK
		//   27 : Item HP
		//   28 : Item MP
		//   29 : Item Weapon Attack
		//   30 : Item Magic Attack
		//   31 : Item Weapon Defense
		//   32 : Item Magic Defense
		//   33 : Item Accuracy
		//   34 : Item Avoid
		//   35 : Item Jump
		//   36 : Item Speed
		//   37 : Mob ID
		//   38 : Chance
		currentid = atoi(itemRow[0]);

		if (currentid != previousid && previousid != -1) { // Add the items into the cache
			Drops::addConsume(previousid, cons);
			Drops::addItem(previousid, item);
			cons.mobs.clear();
		}
		item.type = atoi(itemRow[1]);
		item.price = atoi(itemRow[2]);
		item.maxslot = atoi(itemRow[3]);
		item.quest = atob(itemRow[4]);
		item.consume = atob(itemRow[5]);
		cons.hp = atoi(itemRow[6]);
		cons.mp = atoi(itemRow[7]);
		cons.hpr = atoi(itemRow[8]);
		cons.mpr = atoi(itemRow[9]);
		cons.moveTo = atoi(itemRow[10]);
		// Buffs
		cons.time = atoi(itemRow[11]);
		cons.watk = atoi(itemRow[12]);
		cons.matk = atoi(itemRow[13]);
		cons.avo = atoi(itemRow[14]);
		cons.acc = atoi(itemRow[15]);
		cons.wdef = atoi(itemRow[16]);
		cons.mdef = atoi(itemRow[17]);
		cons.speed = atoi(itemRow[18]);
		cons.jump = atoi(itemRow[19]);
		cons.morph = atoi(itemRow[20]);
		// Scrolling
		cons.success = atoi(itemRow[21]);
		cons.cursed = atoi(itemRow[22]);
		cons.istr = atoi(itemRow[23]);
		cons.idex = atoi(itemRow[24]);
		cons.iint = atoi(itemRow[25]);
		cons.iluk = atoi(itemRow[26]);
		cons.ihp = atoi(itemRow[27]);
		cons.imp = atoi(itemRow[28]);
		cons.iwatk = atoi(itemRow[29]);
		cons.imatk = atoi(itemRow[30]);
		cons.iwdef = atoi(itemRow[31]);
		cons.imdef = atoi(itemRow[32]);
		cons.iacc = atoi(itemRow[33]);
		cons.iavo = atoi(itemRow[34]);
		cons.ijump = atoi(itemRow[35]);
		cons.ispeed = atoi(itemRow[36]);
		cons.ihand = 0;
		// Summoning
		if (itemRow[37] != 0) {
			s.mobid = atoi(itemRow[37]);
			s.chance = atoi(itemRow[38]);
			cons.mobs.push_back(s);
		}

		previousid = atoi(itemRow[0]);
	}
	// Add the final entry
	if (previousid != -1) {
		Drops::addConsume(previousid, cons);
		Drops::addItem(previousid, item);
		cons.mobs.clear();
	}

	// Item Skills
	query << "SELECT * FROM itemskilldata ORDER BY itemid ASC";
	
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

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
		Drops::consumes[atoi(itemSkillRow[0])].skills.push_back(skill);
	}
	std::cout << "DONE" << std::endl;
}
// Drops
void Initializing::initializeDrops() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Drops... ";
	// Mob drops
	mysqlpp::Query query = db.query("SELECT * FROM mobdropdata ORDER BY mobid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	MYSQL_ROW dropRow;
	while ((dropRow = res.fetch_raw_row())) {
		// Col0 : Row ID
		//    1 : Mob ID
		//    2 : Item ID
		//    3 : Chance
		//    4 : Quest
		//    5 : Mesos?
		//    6 : Min mesos
		//    7 : Max mesos

		if (atob(dropRow[5])) {
			Mesos mesos;
			mesos.min = atoi(dropRow[6]);
			mesos.max = atoi(dropRow[7]);
			Drops::addMesos(atoi(dropRow[1]), mesos);
		}
		else {
			MobDropInfo drop;
			drop.id = atoi(dropRow[2]);
			drop.chance = atoi(dropRow[3]);
			drop.quest = atoi(dropRow[4]);
			Drops::addDrop(atoi(dropRow[1]), drop);
		}
	}

	std::cout << "DONE" << std::endl;
}
// Equips
void Initializing::initializeEquips() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Equips... ";
	mysqlpp::Query query = db.query("SELECT * FROM equipdata ORDER BY type ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	MYSQL_ROW equipRow;
	while ((equipRow = res.fetch_raw_row())) {
		EquipInfo equip = EquipInfo();
		// Col0 : EquipID
		//    1 : Type
		//    2 : Price
		//    3 : Slots
		//    4 : HP
		//    5 : MP
		//    6 : STR
		//    7 : DEX
		//    8 : INT
		//    9 : LUK
		//   10 : WAtk
		//   11 : WDef
		//   12 : MAtk
		//   13 : MDef
		//   14 : Acc
		//   15 : Avo
		//   16 : Jump
		//   17 : Speed
		//   18 : Taming Mob
		//   19 : Cash
		//   20 : Quest
		int equipID = atoi(equipRow[0]); // This is the Equip ID
		equip.type = atoi(equipRow[1]);
		equip.price = atoi(equipRow[2]);
		equip.slots = atoi(equipRow[3]);
		equip.ihp = atoi(equipRow[4]);
		equip.imp = atoi(equipRow[5]);
		equip.istr = atoi(equipRow[6]);
		equip.idex = atoi(equipRow[7]);
		equip.iint = atoi(equipRow[8]);
		equip.iluk = atoi(equipRow[9]);
		equip.iwatk = atoi(equipRow[10]);
		equip.iwdef = atoi(equipRow[11]);
		equip.imatk = atoi(equipRow[12]);
		equip.imdef = atoi(equipRow[13]);
		equip.iacc = atoi(equipRow[14]);
		equip.iavo = atoi(equipRow[15]);
		equip.ijump = atoi(equipRow[16]);
		equip.ispeed = atoi(equipRow[17]);
		equip.tamingmob = atoi(equipRow[18]);
		equip.cash = atob(equipRow[19]);
		equip.quest = atob(equipRow[20]);
		equip.ihand = 0;
		// Add equip to the drops table
		Drops::addEquip(equipID, equip);
	}
	std::cout << "DONE" << std::endl;
}
// Shops
void Initializing::initializeShops() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Shops... ";
	mysqlpp::Query query = db.query("SELECT shopdata.*, shopitemdata.itemid, shopitemdata.price FROM shopdata LEFT JOIN shopitemdata ON shopdata.shopid=shopitemdata.shopid ORDER BY shopdata.shopid ASC, shopitemdata.sort DESC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
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
			shop.items.clear();
		}
		shop.npc = atoi(shopRow[1]);
		if (shopRow[2] != 0) {
			ShopItemInfo item;
			item.id = atoi(shopRow[2]);
			item.price = atoi(shopRow[3]);
			shop.items.push_back(item);
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
	mysqlpp::Query query = db.query("SELECT * FROM questdata");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	MYSQL_ROW questRow;
	while ((questRow = res.fetch_raw_row())) {
		// Col0 : Quest ID
		//    1 : Next Quest ID
		Quests::setNextQuest(atoi(questRow[0]), atoi(questRow[1]));
	}

	// Quest Requests
	query << "SELECT * FROM questrequestdata ORDER BY questid ASC";

	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
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

	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

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
	mysqlpp::Query query = db.query("SELECT * FROM skilldata ORDER BY skillid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
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
		//    7 : Value X
		//	  8 : Value Y
		//    9 : Speed
		//   10 : Jump
		//   11 : Weapon Attack
		//   12 : Weapon Defense
		//   13 : Magic Attack
		//   14 : Magic Defense
		//   15 : Accuracy
		//   16 : Avoid
		//   17 : HPP
		//   18 : Prop(% chance)
		//   19 : Cooldown time
		currentid = atoi(skillRow[0]);

		if (currentid != previousid && previousid != -1) {
			Skills::addSkill(previousid, skill);
			skill.clear();
		}

		SkillLevelInfo level;
		level.time = atoi(skillRow[2]);
		level.mp = atoi(skillRow[3]);
		level.hp = atoi(skillRow[4]);
		level.item = atoi(skillRow[5]);
		level.itemcount = atoi(skillRow[6]);
		level.x = atoi(skillRow[7]);
		level.y = atoi(skillRow[8]);
		level.speed = atoi(skillRow[9]);
		level.jump = atoi(skillRow[10]);
		level.watk = atoi(skillRow[11]);
		level.wdef = atoi(skillRow[12]);
		level.matk = atoi(skillRow[13]);
		level.mdef = atoi(skillRow[14]);
		level.acc = atoi(skillRow[15]);
		level.avo = atoi(skillRow[16]);
		level.hpP = atoi(skillRow[17]);
		level.prop = atoi(skillRow[18]);
		level.cooltime = atoi(skillRow[19]);
		skill[atoi(skillRow[1])] = level;

		previousid = atoi(skillRow[0]);
	}
	if (previousid != -1) {
		Skills::addSkill(previousid, skill);
		skill.clear();
	}
	Skills::init();
	std::cout << "DONE" << std::endl;
}
// Maps
void Initializing::initializeMaps() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Maps... ";
	// Maps
	mysqlpp::Query query = db.query("SELECT mapid, returnmap, forcedreturn, mobrate, clock, ship FROM mapdata ORDER BY mapid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	MYSQL_ROW mapRow;
	while ((mapRow = res.fetch_raw_row())) {
		// Col0 : Map ID
		//    1 : Return Map
		//    2 : Forced Return Map
		//    3 : Mob Spawn Rate
		//    4 : Clock
		//    5 : Ship Interval
		int mapid = atoi(mapRow[0]);
		Maps::addMap(mapid);
		MapInfo map;
		map.rm = atoi(mapRow[1]);
		map.forcedReturn = atoi(mapRow[2]);
		map.spawnrate = atof(mapRow[3]);
		map.clock = atob(mapRow[4]);
		map.shipInterval = atoi(mapRow[2]);
		Maps::maps[mapid]->setInfo(map);
	}

	// Portals
	query << "SELECT mapid, portalid, pfrom, pto, toid, type, x, y, script FROM mapportaldata";

	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	MYSQL_ROW portalRow;
	while ((portalRow = res.fetch_raw_row())) {
		// Col0 : Map ID
		//    1 : Portal ID
		//    2 : From
		//    3 : To
		//    4 : To ID
		//    5 : Type
		//    6 : x
		//    7 : y
		//    8 : Script
		PortalInfo portal;
		portal.id = atoi(portalRow[1]);
		strcpy_s(portal.from, portalRow[2]);
		strcpy_s(portal.to, portalRow[3]);
		portal.toid = atoi(portalRow[4]);
		portal.type = atoi(portalRow[5]);
		portal.pos = Pos(atoi(portalRow[6]), atoi(portalRow[7]));
		strcpy_s(portal.script, portalRow[8]);
		Maps::maps[atoi(portalRow[0])]->addPortal(portal);
	}

	// NPCs
	query << "SELECT mapid, npcid, x, cy, fh, rx0, rx1 FROM mapnpcdata";

	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	MYSQL_ROW npcRow;
	while ((npcRow = res.fetch_raw_row())) {
		//    0 : Map ID
		//    1 : NPC ID
		//    2 : x
		//    3 : cy
		//    4 : fh
		//    5 : rx0
		//    6 : rx1
		NPCInfo npc;
		npc.id = atoi(npcRow[1]);
		npc.x = atoi(npcRow[2]);
		npc.cy = atoi(npcRow[3]);
		npc.fh = atoi(npcRow[4]);
		npc.rx0 = atoi(npcRow[5]);
		npc.rx1 = atoi(npcRow[6]);
		Maps::maps[atoi(npcRow[0])]->addNPC(npc);
	}

	// Mobs
	query << "SELECT mapid, mobid, x, cy, fh FROM mapmobdata";

	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	MYSQL_ROW mobRow;
	while ((mobRow = res.fetch_raw_row())) {
		//    0 : Map ID
		//    1 : Mob ID
		//    2 : x
		//    3 : cy
		//    4 : fh
		SpawnInfo spawn;
		spawn.id = atoi(mobRow[1]);
		spawn.pos = Pos(atoi(mobRow[2]), atoi(mobRow[3]));
		spawn.fh = atoi(mobRow[4]);
		Mobs::addSpawn(atoi(mobRow[0]), spawn);
	}

	// Reactors
	query << "SELECT mapid, reactorid, x, y FROM mapreactordata";

	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	MYSQL_ROW reactorRow;
	while ((reactorRow = res.fetch_raw_row())) {
		//    0 : Map ID
		//    1 : Reactor ID
		//    2 : x
		//    3 : y
		ReactorSpawnInfo reactor;
		reactor.id = atoi(reactorRow[1]);
		reactor.pos = Pos(atoi(reactorRow[2]), atoi(reactorRow[3]));
		Reactors::addSpawn(atoi(reactorRow[0]), reactor);
	}

	// Footholds
	query << "SELECT mapid, x1, y1, x2, y2 FROM mapfootholddata";

	if (!(res = query.use())) {
		std::cout << "FAILED: " << db.error() << std::endl;
		exit(1);
	}

	MYSQL_ROW footsRow;
	while ((footsRow = res.fetch_raw_row())) {
		//    0 : Map ID
		//    1 : x1
		//    2 : y1
		//    3 : x2
		//    4 : y2
		//    5 : Previous
		//    6 : Next
		FootholdInfo foot;
		foot.pos1 = Pos(atoi(footsRow[1]), atoi(footsRow[2]));
		foot.pos2 = Pos(atoi(footsRow[3]), atoi(footsRow[4]));
		Maps::maps[atoi(footsRow[0])]->addFoothold(foot);
	}
	db.select_db("maplestory");
	std::cout << "DONE" << std::endl;
}
