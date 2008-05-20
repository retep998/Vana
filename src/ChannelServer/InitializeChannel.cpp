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

// Mobs
void Initializing::initializeMobs(){
	printf("Initializing Mobs... ");
	mysqlpp::Query query = db.query("SELECT mobdata.*, mobsummondata.summonid FROM mobdata LEFT JOIN mobsummondata ON mobdata.mobid=mobsummondata.mobid ORDER BY mobdata.mobid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	MobInfo mob;
	while (mysqlpp::Row mobRow = res.fetch_row()) {
		currentid = atoi(mobRow[1]);

		if(currentid != previousid){
			// Col0 : Row ID
			//    1 : Mob ID
			//    2 : HP
			//    3 : MP
			//    4 : EXP
			//    5 : Boss
			//    6 : HP Color
			//    7 : HP BG Color
			//    8 : Mob Summon
			if(previousid != -1){
				Mobs::addMob(previousid, mob);
				mob.summon.clear();
			}
			mob.hp  = atoi(mobRow[2]);
			mob.mp  = atoi(mobRow[3]);
			mob.exp = atoi(mobRow[4]);
			mob.boss = (bool) mobRow[5];
			mob.hpcolor = atoi(mobRow[6]);
			mob.hpbgcolor = atoi(mobRow[7]);
		}

		if(!mobRow[8].is_null()){
			mob.summon.push_back(atoi(mobRow[8]));
		}
		previousid = atoi(mobRow[1]);
	}
	// Add final entry
	if(previousid != -1){
		Mobs::addMob(previousid, mob);
	}

	printf("DONE\n");
}
// Items
void Initializing::initializeItems(){
	printf("Initializing Items... ");
	mysqlpp::Query query = db.query("SELECT itemdata.*, itemsummondata.mobid, itemsummondata.chance FROM itemdata LEFT JOIN itemsummondata ON itemdata.itemid=itemsummondata.itemid ORDER BY itemid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	ConsumeInfo cons;
	ItemInfo item;
	SummonBag s;
	while (mysqlpp::Row itemRow = res.fetch_row()) {
		// Col0 : Row ID
		//    1 : Item ID
		//    2 : Type
		//    3 : Price
		//    4 : Slots
		//    5 : Quest
		//    6 : Consume
		//    7 : HP
		//    8 : MP
		//    9 : HP Rate
		//   10 : MP Rate
		//   11 : Move to
		//   12 : Time
		//   13 : Weapon Attack
		//   14 : Magic Attack
		//   15 : Avoidability
		//   16 : Accuracy
		//   17 : Weapon Defense
		//   18 : Magic Defense
		//   19 : Speed
		//   20 : Jump
		//   21 : Morph
		//   22 : Success
		//   23 : Cursed
		//   24 : Item STR
		//   25 : Item DEX
		//   26 : Item INT
		//   27 : Item LUK
		//   28 : Item HP
		//   29 : Item MP
		//   30 : Item Weapon Attack
		//   31 : Item Magic Attack
		//   32 : Item Weapon Defense
		//   33 : Item Magic Defense
		//   34 : Item Accuracy
		//   35 : Item Avoid
		//   36 : Item Jump
		//   37 : Item Speed
		//   38 : Mob ID
		//   39 : Chance
		currentid = atoi(itemRow[1]);
		if(currentid != previousid){
			if(previousid != -1){ // Add the items into the cache
				Drops::addConsume(previousid, cons);
				Drops::addItem(previousid, item);
				cons.mobs.clear();
			}
			item.type = atoi(itemRow[2]);
			item.price = atoi(itemRow[3]);
			item.maxslot = atoi(itemRow[4]);
			item.quest = (bool) itemRow[5];
			item.consume = (bool) itemRow[6];
			cons.hp = atoi(itemRow[7]);
			cons.mp = atoi(itemRow[8]);
			cons.hpr = atoi(itemRow[9]);
			cons.mpr = atoi(itemRow[10]);
			cons.moveTo = atoi(itemRow[11]);
			// Buffs
			cons.time = atoi(itemRow[12]);
			cons.watk = atoi(itemRow[13]);
			cons.matk = atoi(itemRow[14]);
			cons.avo = atoi(itemRow[15]);
			cons.acc = atoi(itemRow[16]);
			cons.wdef = atoi(itemRow[17]);
			cons.mdef = atoi(itemRow[18]);
			cons.speed = atoi(itemRow[19]);
			cons.jump = atoi(itemRow[20]);
			cons.morph = atoi(itemRow[21]);
			// Scrolling
			cons.success = atoi(itemRow[22]);
			cons.cursed = atoi(itemRow[23]);
			cons.istr = atoi(itemRow[24]);
			cons.idex = atoi(itemRow[25]);
			cons.iint = atoi(itemRow[26]);
			cons.iluk = atoi(itemRow[27]);
			cons.ihp = atoi(itemRow[28]);
			cons.imp = atoi(itemRow[29]);
			cons.iwatk = atoi(itemRow[30]);
			cons.imatk = atoi(itemRow[31]);
			cons.iwdef = atoi(itemRow[32]);
			cons.imdef = atoi(itemRow[33]);
			cons.iacc = atoi(itemRow[34]);
			cons.iavo = atoi(itemRow[35]);
			cons.ijump = atoi(itemRow[36]);
			cons.ispeed = atoi(itemRow[37]);
			cons.ihand = 0;
		}
		// Summoning
		if(!itemRow[38].is_null()){
			s.mobid = atoi(itemRow[38]);
			s.chance = atoi(itemRow[39]);
			cons.mobs.push_back(s);
		}
		previousid = atoi(itemRow[1]);
	}
	// Add the final entry
	if(previousid != -1){
		Drops::addConsume(previousid, cons);
		Drops::addItem(previousid, item);
		cons.mobs.clear();
	}

	// Item Skills

	query << "SELECT * FROM itemskilldata ORDER BY itemid ASC";
	
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	while(mysqlpp::Row itemSkillRow = res.fetch_row()){
		// Col0 : Row ID
		//    1 : Item ID
		//    2 : Skill ID
		//    3 : Required Level
		//    4 : Master Level

		Skillbook skill;
		skill.skillid = atoi(itemSkillRow[2]);
		skill.reqlevel = atoi(itemSkillRow[3]);
		skill.maxlevel = atoi(itemSkillRow[4]);
		Drops::consumes[atoi(itemSkillRow[1])].skills.push_back(skill);
	}
	printf("DONE\n");
}
// Drops
void Initializing::initializeDrops(){
	printf("Initializing Drops... ");
	// Get all the drops
	mysqlpp::Query query = db.query("SELECT * FROM itemdropdata ORDER BY mobid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	MobDropsInfo drops;
	while (mysqlpp::Row dropRow = res.fetch_row()) {
		currentid = atoi(dropRow[1]);

		if(currentid != previousid && previousid != -1){
			Drops::addDrop(previousid, drops);
			drops.clear();
		}
		MobDropInfo drop;
		drop.id = atoi(dropRow[2]);
		drop.chance = atoi(dropRow[3]);
		drop.quest = atoi(dropRow[4]);
		drops.push_back(drop);

		previousid = atoi(dropRow[1]);
	}
	if(previousid != -1){
		Drops::addDrop(previousid, drops);
		drops.clear();
	}

	// Mesos
	query << "SELECT * FROM mesodropdata ORDER BY mobid ASC";
	
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}
	
	Mesos mesos = Mesos();
	while (mysqlpp::Row mesoRow = res.fetch_row()) {
		mesos.min = atoi(mesoRow[2]);
		mesos.max = atoi(mesoRow[3]);
		Drops::addMesos(atoi(mesoRow[1]), mesos);
	}
	printf("DONE\n");
}
// Equips
void Initializing::initializeEquips(){
	printf("Initializing Equips... ");
	mysqlpp::Query query = db.query("SELECT * FROM equipdata ORDER BY type ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	while (mysqlpp::Row equipRow = res.fetch_row()) {
		EquipInfo equip = EquipInfo();
		// Col0 : RowID
		//    1 : EquipID
		//    2 : Type
		//    3 : Price
		//    4 : Slots
		//    5 : HP
		//    6 : MP
		//    7 : STR
		//    8 : DEX
		//    9 : INT
		//   10 : LUK
		//   11 : WAtk
		//   12 : WDef
		//   13 : MAtk
		//   14 : MDef
		//   15 : Acc
		//   16 : Avo
		//   17 : Jump
		//   18 : Speed
		//   19 : Taming Mob
		//   20 : Cash
		//   21 : Quest
		int equipID = atoi(equipRow[1]); // This is the Equip ID
		equip.type = atoi(equipRow[2]);
		equip.price = atoi(equipRow[3]);
		equip.slots = atoi(equipRow[4]);
		equip.ihp = atoi(equipRow[5]);
		equip.imp = atoi(equipRow[6]);
		equip.istr = atoi(equipRow[7]);
		equip.idex = atoi(equipRow[8]);
		equip.iint = atoi(equipRow[9]);
		equip.iluk = atoi(equipRow[10]);
		equip.iwatk = atoi(equipRow[11]);
		equip.iwdef = atoi(equipRow[12]);
		equip.imatk = atoi(equipRow[13]);
		equip.imdef = atoi(equipRow[14]);
		equip.iacc = atoi(equipRow[15]);
		equip.iavo = atoi(equipRow[16]);
		equip.ijump = atoi(equipRow[17]);
		equip.ispeed = atoi(equipRow[18]);
		equip.tamingmob = atoi(equipRow[19]);
		equip.cash = (bool) equipRow[20];
		equip.quest = (bool) equipRow[21];
		equip.ihand = 0;
		// Add equip to the drops table
		Drops::addEquip(equipID,equip);
	}
	printf("DONE\n");
}
// Shops
void Initializing::initializeShops(){
	printf("Initializing Shops... ");
	mysqlpp::Query query = db.query("SELECT shopdata.*, shopitemdata.itemid, shopitemdata.price FROM shopdata LEFT JOIN shopitemdata ON shopdata.shopid=shopitemdata.shopid ORDER BY shopdata.shopid, shopitemdata.id ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	ShopInfo shop;
	while (mysqlpp::Row shopRow = res.fetch_row()) {
		// Col0 : RowID
		//    1 : shopid
		//    2 : NPC ID
		//    3 : Item ID
		//    4 : Price
		currentid = atoi(shopRow[1]);

		if(currentid != previousid){
			if(previousid != -1){
				Shops::addShop(previousid, shop);
				shop.items.clear();
			}
			shop.npc = atoi(shopRow[2]);
		}
		if(!shopRow[3].is_null()){
			ShopItemInfo item;
			item.id = atoi(shopRow[3]);
			item.price = atoi(shopRow[4]);
			shop.items.push_back(item);
		}
		else printf("Warning: Shop %d does not have any shop items on record.", currentid);

		previousid = atoi(shopRow[1]);
	}
	// Add final entry
	if(previousid != -1){
		Shops::addShop(previousid, shop);
		shop.items.clear();
	}
	printf("DONE\n");
}
// Quests
void Initializing::initializeQuests(){
	printf("Initializing Quests... ");
	// Quests
	mysqlpp::Query query = db.query("SELECT * FROM questdata");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	while (mysqlpp::Row questRow = res.fetch_row()) {
		// Col0 : Row ID
		//    1 : Quest ID
		//    2 : Next Quest ID
		Quests::setNextQuest(atoi(questRow[1]), atoi(questRow[2]));
	}

	// Quest Requests
	query << "SELECT * FROM questrequestdata ORDER BY questid ASC";

	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	QuestRequestsInfo reqs;
	while (mysqlpp::Row requestRow = res.fetch_row()) {
		// Col0 : Row ID
		//    1 : Quest ID
		//    2 : Mob
		//    3 : Item
		//    4 : Quest
		//    5 : Object ID
		//    6 : Count
		currentid = atoi(requestRow[1]);

		if(currentid != previousid && previousid != -1){
			Quests::addRequest(previousid, reqs);
			reqs.clear();
		}

		QuestRequestInfo req;
		req.ismob = (bool) requestRow[2];
		req.isitem = (bool) requestRow[3];
		req.isquest = (bool) requestRow[4];
		req.id = atoi(requestRow[5]);
		req.count = atoi(requestRow[6]);
		reqs.push_back(req);

		previousid = atoi(requestRow[1]);
	}
	if(previousid != -1){
		Quests::addRequest(previousid, reqs);
		reqs.clear();
	}

	// Quest Rewards
	query << "SELECT * FROM questrewarddata ORDER BY questid ASC";

	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	currentid = 0;
	previousid = -1;
	QuestRewardsInfo rwas;
	while (mysqlpp::Row rewardRow = res.fetch_row()) {
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

		if(currentid != previousid && previousid != -1){
			Quests::addReward(previousid, rwas);
			rwas.clear();
		}

		QuestRewardInfo rwa;
		rwa.start = (bool) rewardRow[2];
		rwa.isitem = (bool) rewardRow[3];
		rwa.isexp = (bool) rewardRow[4];
		rwa.ismesos = (bool) rewardRow[5];
		rwa.isfame = (bool) rewardRow[6];
		rwa.isskill = (bool) rewardRow[7];
		rwa.id = atoi(rewardRow[8]);
		rwa.count = atoi(rewardRow[9]);
		rwa.gender = atoi(rewardRow[10]);
		rwa.job = atoi(rewardRow[11]);
		rwa.prop = atoi(rewardRow[12]);
		rwas.push_back(rwa);

		previousid = atoi(rewardRow[1]);
	}
	if(previousid != -1){
		Quests::addReward(previousid, rwas);
		rwas.clear();
	}
	printf("DONE\n");
}
// Skills
void Initializing::initializeSkills(){
	printf("Initializing Skills... ");
	mysqlpp::Query query = db.query("SELECT * FROM skilldata ORDER BY skillid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	SkillsLevelInfo skill;
	while (mysqlpp::Row skillRow = res.fetch_row()) {
		// Col0 : Row ID
		//    1 : Skill ID
		//    2 : Level
		//    3 : Time
		//    4 : MP
		//    5 : HP
		//    6 : Item
		//    7 : Item Count
		//    8 : Value X
		//	  9 : Value Y
		//   10 : Speed
		//   11 : Jump
		//   12 : Weapon Attack
		//   13 : Weapon Defense
		//   14 : Magic Attack
		//   15 : Magic Defense
		//   16 : Accuracy
		//   17 : Avoid
		//   18 : HPP
		currentid = atoi(skillRow[1]);
		if(currentid != previousid && previousid != -1){
			Skills::addSkill(previousid, skill);
			skill.clear();
		}

		SkillLevelInfo level;
		level.time = atoi(skillRow[3]);
		level.mp = atoi(skillRow[4]);
		level.hp = atoi(skillRow[5]);
		level.item = atoi(skillRow[6]);
		level.itemcount = atoi(skillRow[7]);
		level.x = atoi(skillRow[8]);
		level.y = atoi(skillRow[9]);
		level.speed = atoi(skillRow[10]);
		level.jump = atoi(skillRow[11]);
		level.watk = atoi(skillRow[12]);
		level.wdef = atoi(skillRow[13]);
		level.matk = atoi(skillRow[14]);
		level.mdef = atoi(skillRow[15]);
		level.acc = atoi(skillRow[16]);
		level.avo = atoi(skillRow[17]);
		level.hpP = atoi(skillRow[18]);
		skill[atoi(skillRow[2])] = level;

		previousid = atoi(skillRow[1]);
	}
	if(previousid != -1){
		Skills::addSkill(previousid, skill);
		skill.clear();
	}
	Skills::init();
	printf("DONE\n");
}
// Maps
void Initializing::initializeMaps(){
	printf("Initializing Maps... ");
	// Maps and portals
	mysqlpp::Query query = db.query("SELECT mapdata.*, mapportaldata.portalid, mapportaldata.pfrom, mapportaldata.pto, mapportaldata.toid, mapportaldata.type, mapportaldata.x, mapportaldata.y, mapportaldata.script FROM mapdata LEFT JOIN mapportaldata ON mapdata.mapid=mapportaldata.mapid ORDER BY mapdata.mapid ASC");

	mysqlpp::UseQueryResult res;
	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	int currentid = 0;
	int previousid = -1;
	MapInfo map;
	while (mysqlpp::Row mapRow = res.fetch_row()) {
		// Col0 : Row ID
		//    1 : Map ID
		//    2 : Return Map
		//    3 : Forced Return Map
		//    4 : Mob Spawn Rate
		//    5 : Clock
		//    6 : Ship Interval
		//    7 : Portal ID
		//    8 : From
		//    9 : To
		//   10 : To ID
		//   11 : Type
		//   12 : x
		//   13 : y
		//   14 : Script
		currentid = atoi(mapRow[1]);

		if(currentid != previousid){
			if(previousid != -1){
				Maps::addMap(previousid, map);
				map.Portals.clear();
			}
			map.rm = atoi(mapRow[2]);
			map.forcedReturn = atoi(mapRow[3]);
			map.spawnrate = atof(mapRow[4]);
			map.clock = (bool) mapRow[5];
			map.shipInterval = atoi(mapRow[6]);
		}
		PortalInfo portal;
		if(!mapRow[7].is_null()){
			portal.id = atoi(mapRow[7]);
			strcpy_s(portal.from, mapRow[8]);
			strcpy_s(portal.to, mapRow[9]);
			portal.toid = atoi(mapRow[10]);
			portal.type = atoi(mapRow[11]);
			portal.x = atoi(mapRow[12]);
			portal.y = atoi(mapRow[13]);
			strcpy_s(portal.script, mapRow[14]);
			map.Portals.push_back(portal);
		}
		else printf("Warning: Map %d has no portal data on record.", currentid);

		previousid = atoi(mapRow[1]);
	}
	if(previousid != -1){
		Maps::addMap(previousid, map);
		map.Portals.clear();
	}

	// NPCs
	query << "SELECT * FROM mapnpcdata ORDER BY mapid ASC";

	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	currentid = 0;
	previousid = -1;
	NPCsInfo npcs;
	while (mysqlpp::Row npcRow = res.fetch_row()) {
		// Col0 : Row ID
		//    1 : Map ID
		//    2 : NPC ID
		//    3 : x
		//    4 : cy
		//    5 : fh
		//    6 : rx0
		//    7 : rx1
		currentid = atoi(npcRow[1]);

		if(currentid != previousid && previousid != -1){
			NPCs::addNPC(previousid, npcs);
			npcs.clear();
		}
		NPCInfo npc;
		npc.id = atoi(npcRow[2]);
		npc.x = atoi(npcRow[3]);
		npc.cy = atoi(npcRow[4]);
		npc.fh = atoi(npcRow[5]);
		npc.rx0 = atoi(npcRow[6]);
		npc.rx1 = atoi(npcRow[7]);
		npcs.push_back(npc);

		previousid = atoi(npcRow[1]);
	}
	if(previousid != -1){
		NPCs::addNPC(previousid, npcs);
		npcs.clear();
	}

	// Mobs
	query << "SELECT * FROM mapmobdata ORDER BY mapid ASC";

	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	currentid = 0;
	previousid = -1;
	SpawnsInfo spawns;
	while (mysqlpp::Row mobRow = res.fetch_row()) {
		// Col0 : Row ID
		//    1 : Map ID
		//    2 : Mob ID
		//    3 : x
		//    4 : cy
		//    5 : fh
		currentid = atoi(mobRow[1]);

		if(currentid != previousid && previousid != -1){
			Mobs::addSpawn(previousid, spawns);
			spawns.clear();
		}
		SpawnInfo spawn;
		spawn.id = atoi(mobRow[2]);
		spawn.x = atoi(mobRow[3]);
		spawn.cy = atoi(mobRow[4]);
		spawn.fh = atoi(mobRow[5]);
		spawns.push_back(spawn);

		previousid = atoi(mobRow[1]);
	}
	if(previousid != -1){
		Mobs::addSpawn(previousid, spawns);
		spawns.clear();
	}

	//Footholds
	query << "SELECT * FROM mapfootholddata ORDER BY mapid ASC";

	if (!(res = query.use())) {
		printf("FAILED: %s\n", db.error());
		exit(1);
	}

	currentid = 0;
	previousid = -1;
	FootholdsInfo foots;
	while (mysqlpp::Row footsRow = res.fetch_row()) {
		// Col0 : Row ID
		//    1 : Map ID
		//    2 : x1
		//    3 : x2
		//    4 : y1
		//    5 : y2
		currentid = atoi(footsRow[1]);
		if(currentid != previousid && previousid != -1){
			Drops::addFoothold(previousid, foots);
			Drops::objids[previousid] = 100;
			foots.clear();
		}
		FootholdInfo foot;
		foot.x1 = atoi(footsRow[2]);
		foot.x2 = atoi(footsRow[3]);
		foot.y1 = atoi(footsRow[4]);
		foot.y2 = atoi(footsRow[4]);
		foots.push_back(foot);

		previousid = atoi(footsRow[1]);
	}
	// Add final entry
	if(previousid != -1){
		Drops::addFoothold(previousid, foots);
		Drops::objids[previousid] = 100;
	}
	printf("DONE\n");
}
