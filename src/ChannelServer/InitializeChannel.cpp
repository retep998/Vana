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
			if(previousid != -1){
				Mobs::addMob(previousid, mob);
				mob.summon.clear();
			}
			mob.hp  = atoi(mobRow[2]);
			mob.mp  = atoi(mobRow[3]);
			mob.exp = atoi(mobRow[4]);
		}

		if(!mobRow[5].is_null()){
			mob.summon.push_back(atoi(mobRow[5]));
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
		//   12 : Weapon Attack
		//   13 : Magic Attack
		//   14 : Avoidability
		//   15 : Accuracy
		//   16 : Weapon Defense
		//   17 : Magic Defense
		//   18 : Speed
		//   19 : Jump
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
		//   34 : Item Hands
		//   35 : Item Jump
		//   36 : Item Speed
		//   37 : Mob ID
		//   38 : Chance
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
			item.quest = atoi(itemRow[5]);
			item.consume = atoi(itemRow[6]);
			cons.hp = atoi(itemRow[7]);
			cons.mp = atoi(itemRow[8]);
			cons.hpr = atoi(itemRow[9]);
			cons.mpr = atoi(itemRow[10]);
			cons.moveTo = atoi(itemRow[11]);
			// Scrolling
			cons.success = atoi(itemRow[20]);
			cons.cursed = atoi(itemRow[21]);
			cons.istr = atoi(itemRow[22]);
			cons.idex = atoi(itemRow[23]);
			cons.iint = atoi(itemRow[24]);
			cons.iluk = atoi(itemRow[25]);
			cons.ihp = atoi(itemRow[26]);
			cons.imp = atoi(itemRow[27]);
			cons.iwatk = atoi(itemRow[28]);
			cons.imatk = atoi(itemRow[29]);
			cons.iwdef = atoi(itemRow[30]);
			cons.imdef = atoi(itemRow[31]);
			cons.iacc = atoi(itemRow[32]);
			cons.iavo = atoi(itemRow[33]);
			cons.ihand = atoi(itemRow[34]);
			cons.ijump = atoi(itemRow[35]);
			cons.ispeed = atoi(itemRow[36]);
		}
		if(!itemRow[37].is_null()){
			s.mobid = atoi(itemRow[37]);
			s.chance = atoi(itemRow[38]);
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
		//   19 : Cash
		//   20 : Quest
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
		equip.cash = atoi(equipRow[19]);
		equip.quest = atoi(equipRow[20]);
		// Add equip to the drops table
		Drops::addEquip(equipID,equip);
	}
	printf("DONE\n");
}
// Shops
void Initializing::initializeShops(){
	printf("Initializing Shops... ");
	// Col0 : RowID
	//    1 : shopid
	//    2 : NPC ID
	//    3 : Item ID
	//    4 : Price
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
		req.ismob = atoi(requestRow[2]);
		req.isitem = atoi(requestRow[3]);
		req.isquest = atoi(requestRow[4]);
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
		//    7 : Object ID
		//    8 : Count
		//    9 : Gender
		//   10 : Job
		//   11 : Prop
		currentid = atoi(rewardRow[1]);

		if(currentid != previousid && previousid != -1){
			Quests::addReward(previousid, rwas);
			rwas.clear();
		}

		QuestRewardInfo rwa;
		rwa.start = atoi(rewardRow[2]);
		rwa.isitem = atoi(rewardRow[3]);
		rwa.isexp = atoi(rewardRow[4]);
		rwa.ismesos = atoi(rewardRow[5]);
		rwa.isfame = atoi(rewardRow[6]);
		rwa.id = atoi(rewardRow[7]);
		rwa.count = atoi(rewardRow[8]);
		rwa.gender = atoi(rewardRow[9]);
		rwa.job = atoi(rewardRow[10]);
		rwa.prop = atoi(rewardRow[11]);
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

	int count = 1;
	int currentid = 0;
	int previousid = -1;
	SkillsLevelInfo skill;
	while (mysqlpp::Row skillRow = res.fetch_row()) {
		// Col0 : Row ID
		//    1 : Skill ID
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
		currentid = atoi(skillRow[1]);
		if(currentid != previousid && previousid != -1){
			Skills::addSkill(previousid, skill);
			skill.clear();
			count = 1;
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
		skill[count] = level;
		count++;

		previousid = atoi(skillRow[1]);
	}
	if(previousid != -1){
		Skills::addSkill(previousid, skill);
		skill.clear();
	}
	printf("DONE\n");
}
// Maps
void Initializing::initializeMaps(){
	printf("Initializing Maps... ");
	// Maps and portals
	mysqlpp::Query query = db.query("SELECT mapdata.*, portaldata.portalid, portaldata.pfrom, portaldata.pto, portaldata.toid, portaldata.type, portaldata.x, portaldata.y FROM mapdata LEFT JOIN portaldata ON mapdata.mapid=portaldata.mapid ORDER BY mapdata.mapid ASC");

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
		//    3 : Portal ID
		//    4 : From
		//    5 : To
		//    6 : To ID
		//    7 : Type
		//    8 : x
		//    9 : y
		currentid = atoi(mapRow[1]);

		if(currentid != previousid){
			if(previousid != -1){
				Maps::addMap(previousid, map);
				map.Portals.clear();
			}
			map.rm = atoi(mapRow[2]);
		}
		PortalInfo portal;
		if(!mapRow[3].is_null()){
			portal.id = atoi(mapRow[3]);
			strcpy_s(portal.from, mapRow[4]);
			strcpy_s(portal.to, mapRow[5]);
			portal.toid = atoi(mapRow[6]);
			portal.type = atoi(mapRow[7]);
			portal.x = atoi(mapRow[8]);
			portal.y = atoi(mapRow[9]);
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

void Initializing::initializeTimers(){
	printf("Initializing Timers... ");
	Timer::timer = new Timer();
	Skills::startTimer();
	Maps::startTimer();
	printf("DONE\n");
}
