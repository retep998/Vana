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
#include "Player.h"
#include "BuddyList.h"
#include "ChannelServer.h"
#include "ChatHandler.h"
#include "CommandHandler.h"
#include "Connectable.h"
#include "Drops.h"
#include "Fame.h"
#include "Inventory.h"
#include "KeyMaps.h"
#include "Levels.h"
#include "LevelsPacket.h"
#include "Maps.h"
#include "Mobs.h"
#include "MySQLM.h"
#include "NPCs.h"
#include "Party.h"
#include "PlayerHandler.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "Pos.h"
#include "Quests.h"
#include "Reactors.h"
#include "ReadPacket.h"
#include "RecvHeader.h"
#include "ServerPacket.h"
#include "SkillMacros.h"
#include "Skills.h"
#include "Trades.h"
#include "TradesPacket.h"
#include "WorldServerConnectPlayer.h"
#include "WorldServerConnectPlayerPacket.h"
#include "Pets.h"

Player::~Player() {
	if (isconnect) {
		//if (this->getHP() == 0)
		//	this->acceptDeath();
		// "Bug" in global, would be fixed here:
		// When disconnecting and dead, you actually go back to forced return map before the death return map
		// (that means that it's parsed while logging in, not while logging out)
		if (save_on_dc) {
			saveAll();
			setOnline(false);
		}
		if (isTrading()) {
			Trades::cancelTrade(this);
		}
		WorldServerConnectPlayerPacket::removePlayer(ChannelServer::Instance()->getWorldPlayer(), id);
		Maps::maps[this->getMap()]->removePlayer(this);
		Players::Instance()->removePlayer(this);
	}
}

void Player::realHandleRequest(ReadPacket *packet) {
	switch (packet->getShort()) {
		case RECV_ADD_SKILL: Skills::addSkill(this, packet); break;
		case RECV_ADD_STAT: Levels::addStat(this, packet); break;
		case RECV_CANCEL_ITEM: Inventory::cancelItem(this, packet); break;
		case RECV_CANCEL_SKILL: Skills::cancelSkill(this, packet); break;
		case RECV_CHANGE_CHANNEL: changeChannel(packet->getByte()); break;
		case RECV_CHANGE_MAP: Maps::usePortal(this, packet); break;
		case RECV_CHANGE_MAP_SPECIAL: Maps::useScriptedPortal(this, packet); break; // Portals that cause scripted events
		case RECV_CHANNEL_LOGIN: playerConnect(packet); break;
		case RECV_CHAT: ChatHandler::handleChat(this, packet); break;
		case RECV_COMMAND: CommandHandler::handleCommand(this, packet); break;
		case RECV_CONTROL_MOB: Mobs::monsterControl(this, packet); break;
		case RECV_DAMAGE_MOB: Mobs::damageMob(this, packet); break;
		case RECV_DAMAGE_MOB_RANGED: Mobs::damageMobRanged(this, packet); break;
		case RECV_DAMAGE_MOB_SPELL: Mobs::damageMobSpell(this, packet); break;
		case RECV_DAMAGE_PLAYER: PlayerHandler::handleDamage(this, packet); break;
		case RECV_DROP_MESO: Drops::dropMesos(this, packet); break;
		case RECV_FACE_EXPRESSION: PlayerHandler::handleFacialExpression(this, packet); break;
		case RECV_FAME: Fame::handleFame(this, packet); break;
		case RECV_GET_PLAYER_INFO: PlayerHandler::handleGetInfo(this, packet); break;
		case RECV_GET_QUEST: Quests::getQuest(this, packet); break;
		case RECV_GROUP_CHAT: ChatHandler::handleGroupChat(this, packet); break;
		case RECV_HEAL_PLAYER: PlayerHandler::handleHeal(this, packet); break;
		case RECV_HIT_REACTOR: Reactors::hitReactor(this, packet); break;
		case RECV_KEYMAP: changeKey(packet); break;
		case RECV_LOOT_ITEM: Drops::lootItem(this, packet); break;
		case RECV_MOVE_ITEM: Inventory::itemMove(this, packet); break;
		case RECV_MOVE_PLAYER: PlayerHandler::handleMoving(this, packet); break;
		case RECV_NPC_TALK: NPCs::handleNPC(this, packet); break;
		case RECV_NPC_TALK_CONT: NPCs::handleNPCIn(this, packet); break;
		case RECV_PARTY_ACTION: Party::handleRequest(this, packet); break;
		case RECV_PET_CHAT: Pets::chat(this, packet); break;
		case RECV_PET_COMMAND: Pets::showAnimation(this, packet); break;
		case RECV_PET_FEED: Pets::feedPet(this, packet); break;
		case RECV_PET_LOOT: Pets::lootItem(this, packet); break;
		case RECV_PET_MOVE: Pets::movePet(this, packet); break;
		case RECV_PET_SUMMON: Pets::summonPet(this, packet); break;
		case RECV_SHOP_ACTION: Trades::tradeHandler(this, packet); break;
		case RECV_SHOP_ENTER: Inventory::useShop(this, packet); break;
		case RECV_USE_STORAGE: Inventory::useStorage(this, packet); break;
		case RECV_SKILL_MACRO: changeSkillMacros(packet); break;
		case RECV_SPECIAL_SKILL: PlayerHandler::handleSpecialSkills(this, packet); break;
		case RECV_STOP_CHAIR: Inventory::stopChair(this, packet); break;
		case RECV_USE_CASH_ITEM: Inventory::useCashItem(this, packet); break;
		case RECV_USE_CHAIR: Inventory::useChair(this, packet); break;
		case RECV_USE_ITEM: Inventory::useItem(this, packet); break;
		case RECV_USE_ITEM_EFFECT: Inventory::useItemEffect(this, packet); break;
		case RECV_USE_RETURN_SCROLL: Inventory::useReturnScroll(this, packet); break;
		case RECV_USE_SCROLL: Inventory::useScroll(this, packet); break;
		case RECV_USE_SKILL: Skills::useSkill(this, packet); break;
		case RECV_USE_SKILLBOOK: Inventory::useSkillbook(this, packet); break; // Skillbooks
		case RECV_USE_SUMMON_BAG: Inventory::useSummonBag(this, packet); break;
	}
}

void Player::playerConnect(ReadPacket *packet) {
	int id = packet->getInt();
	if (!Connectable::Instance()->checkPlayer(id)) {
		//hacking
		packetHandler->disconnect();
		return;
	}
	this->id = id;
	activeBuffs.reset(new PlayerActiveBuffs(this));
	skills.reset(new PlayerSkills(this));
	quests.reset(new PlayerQuests(this));
	buddyList.reset(new BuddyList(this));
	pets.reset(new PlayerPets(this));
	// Character info
	mysqlpp::Query query = Database::chardb.query();
	query << "SELECT characters.*, users.gm FROM characters LEFT JOIN users on characters.userid = users.id WHERE characters.id = " << mysqlpp::quote << this->id;
	mysqlpp::StoreQueryResult res = query.store();

	if (res.empty()) {
		//hacking
		packetHandler->disconnect();
		return;
	}

	userid = res[0]["userid"];
	world_id = (unsigned char) res[0]["world_id"];
	res[0]["name"].to_string(name);
	gender = (unsigned char) res[0]["gender"];
	skin = (unsigned char) res[0]["skin"];
	eyes = res[0]["eyes"];
	hair = res[0]["hair"];
	level = (unsigned char) res[0]["level"];
	job = (short) res[0]["job"];
	str = (short) res[0]["str"];
	dex = (short) res[0]["dex"];
	intt = (short) res[0]["int"];
	luk = (short) res[0]["luk"];
	hp = (short) res[0]["chp"];
	rmhp = mhp = (short) res[0]["mhp"];
	mp = (short) res[0]["cmp"];
	rmmp = mmp = (short) res[0]["mmp"];
	hpmp_ap = (short) res[0]["hpmp_ap"];
	ap = (short) res[0]["ap"];
	sp = (short) res[0]["sp"];
	exp = res[0]["exp"];
	fame = (short) res[0]["fame"];
	map = res[0]["map"];
	mappos = (unsigned char) res[0]["pos"];
	gm = res[0]["gm"];

	// Inventory
	unsigned char maxslots[5];
	maxslots[0] = (unsigned char) res[0]["equip_slots"];
	maxslots[1] = (unsigned char) res[0]["use_slots"];
	maxslots[2] = (unsigned char) res[0]["setup_slots"];
	maxslots[3] = (unsigned char) res[0]["etc_slots"];
	maxslots[4] = (unsigned char) res[0]["cash_slots"];
	inv.reset(new PlayerInventory(this, maxslots));

	inv->setMesosStart(res[0]["mesos"]);

	query << "SELECT inv, slot, itemid, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, petid, items.name, pets.index, pets.name, pets.level, pets.closeness, pets.fullness FROM items LEFT JOIN pets ON items.petid=pets.id WHERE charid = " << mysqlpp::quote << getId();
	res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		Item *item = new Item;
		item->id = res[i][2];
		item->amount = res[i][3];
		item->slots = (unsigned char) res[i][4];
		item->scrolls = (unsigned char) res[i][5];
		item->istr = res[i][6];
		item->idex = res[i][7];
		item->iint = res[i][8];
		item->iluk = res[i][9];
		item->ihp = res[i][10];
		item->imp = res[i][11];
		item->iwatk = res[i][12];
		item->imatk = res[i][13];
		item->iwdef = res[i][14];
		item->imdef = res[i][15];
		item->iacc = res[i][16];
		item->iavo = res[i][17];
		item->ihand = res[i][18];
		item->ispeed = res[i][19];
		item->ijump = res[i][20];
		item->petid = res[i][21];
		res[i][22].to_string(item->name);
		inv->addItem((unsigned char) res[i][0], res[i][1], item);
		if (item->petid != 0) {
			Pet *pet = new Pet(this);
			pet->setId(item->petid);
			pet->setType(item->id);
			pet->setIndex((signed char) res[i][23]);
			pet->setName((string) res[i][24]);
			pet->setLevel((unsigned char) res[i][25]);
			pet->setCloseness((short) res[i][26]);
			pet->setFullness((unsigned char) res[i][27]);
			pet->setInventorySlot((unsigned char)res[i][1]);
			pet->setSummoned(false);
			pets->addPet(pet);
			if (pet->getIndex() != -1)
				pets->setSummoned(pet->getId(), pet->getIndex());
		}
	}

	// Skills
	query << "SELECT skillid, points, maxlevel FROM skills WHERE charid = " << mysqlpp::quote << this->id;
	res = query.store();
	for (size_t i = 0; i < res.num_rows(); i++) {
		skills->addSkillLevel(res[i][0], res[i][1], false);
		if (FORTHJOB_SKILL(res[i][0])) {
			skills->setMaxSkillLevel(res[i][0], res[i][2]);
		}
	}

	// Storage
	query << "SELECT slots, mesos FROM storage WHERE userid = " << this->userid << " AND world_id = " << (short) this->world_id;
	res = query.store();
	if (res.num_rows() != 0)
		storage.reset(new PlayerStorage(this, (unsigned char) res[0][0], res[0][1]));
	else
		storage.reset(new PlayerStorage(this, 4, 0));

	query << "SELECT itemid, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, name FROM storageitems WHERE userid = " << mysqlpp::quote << this->userid << " AND world_id = " << (short) this->world_id << " ORDER BY slot ASC";
	res = query.store();
	for (size_t i = 0; i < res.num_rows(); i++) {
		Item *item = new Item;
		item->id = res[i][0];
		item->amount = res[i][1];
		item->slots = (unsigned char) res[i][2];
		item->scrolls = (unsigned char) res[i][3];
		item->istr = res[i][4];
		item->idex = res[i][5];
		item->iint = res[i][6];
		item->iluk = res[i][7];
		item->ihp = res[i][8];
		item->imp = res[i][9];
		item->iwatk = res[i][10];
		item->imatk = res[i][11];
		item->iwdef = res[i][12];
		item->imdef = res[i][13];
		item->iacc = res[i][14];
		item->iavo = res[i][15];
		item->ihand = res[i][16];
		item->ispeed = res[i][17];
		item->ijump = res[i][18];
		res[i][19].to_string(item->name);
		storage->addItem(item);
	}

	// Key Maps and Macros
	KeyMaps keyMaps;
	keyMaps.load(this->id);

	SkillMacros skillMacros;
	skillMacros.load(this->id);

	// Character variables
	query << "SELECT * FROM character_variables WHERE charid = " << this->id;
	res = query.store();
	for (size_t i = 0; i < res.size(); i++) {
		variables[(string) res[i]["key"]] = res[i]["value"];
	}

	if (Maps::maps[map]->getInfo().forcedReturn != 999999999) {
		map = Maps::maps[map]->getInfo().forcedReturn;
	}
	if (hp == 0) // If dead
		hp = 50;
	m_pos = Maps::maps[map]->getSpawnPoint(mappos)->pos;
	m_stance = 0;
	m_foothold = 0;

	PlayerPacket::connectData(this);
	
	if (ChannelServer::Instance()->getScrollingHeader().size() > 0) {
		ServerPacket::showScrollingHeader(this, ChannelServer::Instance()->getScrollingHeader());
	}

	for (char i = 0; i < 3; i++)
		if (pets->getSummoned(i))
			pets->getPet(pets->getSummoned(i))->setPos(Maps::maps[map]->getSpawnPoint(mappos)->pos);

	PlayerPacket::showKeys(this, &keyMaps);

	if (skillMacros.getMax() > -1) {
		PlayerPacket::showSkillMacros(this, &skillMacros);
	}

	Maps::newMap(this, map);

	setOnline(true);
	isconnect = true;
	WorldServerConnectPlayerPacket::registerPlayer(ChannelServer::Instance()->getWorldPlayer(), id, name, map, job, level);
}

void Player::setHP(int hp, bool is) {
	if (hp < 0)
		this->hp = 0;
	else if (hp > mhp)
		this->hp = mhp;
	else
		this->hp = hp;
	if (is)
		PlayerPacket::updateStat(this, 0x400, static_cast<short>(this->hp));
	getActiveBuffs()->checkBerserk();
}

void Player::setMP(int mp, bool is) {
	if (!(getActiveBuffs()->getActiveSkillLevel(2121004) > 0 || getActiveBuffs()->getActiveSkillLevel(2221004) > 0 || getActiveBuffs()->getActiveSkillLevel(2321004) > 0)) {
		if (mp < 0)
			this->mp = 0;
		else if (mp > mmp)
			this->mp = mmp;
		else
			this->mp = mp;
	}
	PlayerPacket::updateStat(this, 0x1000, static_cast<short>(this->mp), is);
}

void Player::setSp(short sp) {
	this->sp = sp;
	PlayerPacket::updateStat(this, 0x8000, sp);
}

void Player::setAp(short ap) {
	this->ap = ap;
	PlayerPacket::updateStat(this, 0x4000, ap);
}

void Player::setJob(short job) {
	this->job = job;
	PlayerPacket::updateStat(this, 0x20, job);
	LevelsPacket::jobChange(this);
	WorldServerConnectPlayerPacket::updateJob(ChannelServer::Instance()->getWorldPlayer(), this->id, job);
}

void Player::setStr(short str) {
	this->str = str;
	PlayerPacket::updateStat(this, 0x40, str);
}

void Player::setDex(short dex) {
	this->dex = dex;
	PlayerPacket::updateStat(this, 0x80, dex);
}

void Player::setInt(short intt) {
	this->intt = intt;
	PlayerPacket::updateStat(this, 0x100, intt);
}

void Player::setLuk(short luk) {
	this->luk = luk;
	PlayerPacket::updateStat(this, 0x200, luk);
}

void Player::setMHP(int mhp) {
	if (mhp > 30000) { mhp = 30000; }
	this->mhp = mhp;
	PlayerPacket::updateStat(this, 0x800, rmhp);
	getActiveBuffs()->checkBerserk();
}

void Player::setRMHP(int rmhp) {
	if (rmhp > 30000) { rmhp = 30000; }
	this->rmhp = rmhp;
	PlayerPacket::updateStat(this, 0x800, rmhp);
}

void Player::setMMP(int mmp) {
	if (mmp > 30000) { mmp = 30000; }
	this->mmp = mmp;
	PlayerPacket::updateStat(this, 0x2000, rmmp);
}

void Player::setRMMP(int rmmp) {
	if (rmmp > 30000) { rmmp = 30000; }
	this->rmmp = rmmp;
	PlayerPacket::updateStat(this, 0x2000, rmmp);
}

void Player::setExp(int exp) {
	this->exp = exp;
	PlayerPacket::updateStat(this, 0x10000, exp);
}

void Player::setLevel(unsigned char level) {
	this->level = level;
	PlayerPacket::updateStat(this, 0x10, level);
	LevelsPacket::levelUP(this);
	WorldServerConnectPlayerPacket::updateLevel(ChannelServer::Instance()->getWorldPlayer(), this->id, level);
}

void Player::changeChannel(char channel) {
	ChannelServer::Instance()->getWorldPlayer()->playerChangeChannel(id, channel);
}

void Player::changeKey(ReadPacket *packet) {
	packet->skipBytes(4);
	int howmany = packet->getInt();
	if (howmany == 0) return;

	KeyMaps keyMaps; // We don't need old values here because it is only used to save the new values
	for (int i = 0; i < howmany; i++) {
		int pos = packet->getInt();
		char type = packet->getByte();
		int action = packet->getInt();
		keyMaps.add(pos, new KeyMaps::KeyMap(type, action));
	}

	// Update to MySQL
	keyMaps.save(this->id);
}

void Player::changeSkillMacros(ReadPacket *packet) {
	unsigned char num = packet->getByte();
	if (num == 0) return;

	SkillMacros skillMacros;
	for (unsigned char i = 0; i < num; i++) {
		string name = packet->getString();
		bool shout = packet->getByte() != 0;
		int skill1 = packet->getInt();
		int skill2 = packet->getInt();
		int skill3 = packet->getInt();

		skillMacros.add(i, new SkillMacros::SkillMacro(name, shout, skill1, skill2, skill3));
	}
	skillMacros.save(this->id);
}

void Player::setHair(int id) {
	this->hair = id;
	PlayerPacket::updateStat(this, 0x04, id);
}

void Player::setEyes(int id) {
	this->eyes = id;
	PlayerPacket::updateStat(this, 0x02, id);
}

void Player::setSkin(char id) {
	this->skin = id;
	PlayerPacket::updateStat(this, 0x01, id);
}

void Player::setFame(short fame) {
	if (fame < -30000)
		fame = -30000;
	else if (fame > 30000)
		fame = 30000;
	this->fame = fame;
	PlayerPacket::updateStat(this, 0x20000, fame);
}

void Player::deleteVariable(const string &name) {
	if (variables.find(name) != variables.end())
		variables.erase(name);
}

void Player::setVariable(const string &name, const string &val) {
	variables[name] = val;
}

string Player::getVariable(const string &name) {
	return (variables.find(name) == variables.end()) ? "" : variables[name];
}

bool Player::addWarning() {
	int t = clock();
	// Deleting old warnings
	for (size_t i = 0; i < warnings.size(); i++) {
		if (warnings[i] + 300000 < t) {
			warnings.erase(warnings.begin()+i);
			i--;
		}
	}
	warnings.push_back(t);
	if (warnings.size() > 50) {
		// Hacker - Temp DCing
		packetHandler->disconnect();
		return true;
	}
	return false;
}

void Player::saveStats() {
	mysqlpp::Query query = Database::chardb.query();
	query << "UPDATE characters SET "
		<< "level = " << mysqlpp::quote << this->level << ","
		<< "job = " << mysqlpp::quote << this->job << ","
		<< "str = " << mysqlpp::quote << this->str << ","
		<< "dex = " << mysqlpp::quote << this->dex << ","
		<< "`int` = " << mysqlpp::quote << this->intt << ","
		<< "luk = " << mysqlpp::quote << this->luk << ","
		<< "chp = " << mysqlpp::quote << this->hp << ","
		<< "mhp = " << mysqlpp::quote << this->rmhp << ","
		<< "cmp = " << mysqlpp::quote << this->mp << ","
		<< "mmp = " << mysqlpp::quote << this->rmmp << ","
		<< "hpmp_ap = " << mysqlpp::quote << this->hpmp_ap << ","
		<< "ap = " << mysqlpp::quote << this->ap << ","
		<< "sp = " << mysqlpp::quote << this->sp << ","
		<< "exp = " << mysqlpp::quote << this->exp << ","
		<< "fame = " << mysqlpp::quote << this->fame << ","
		<< "map = " << mysqlpp::quote << this->map << ","
		<< "gender = " << mysqlpp::quote << (short) this->gender << ","
		<< "skin = " << mysqlpp::quote << (short) this->skin << ","
		<< "eyes = " << mysqlpp::quote << this->eyes << ","
		<< "hair = " << mysqlpp::quote << this->hair << ","
		<< "mesos = " << mysqlpp::quote << inv->getMesos() << ","
		<< "equip_slots = " << mysqlpp::quote << inv->getMaxSlots(1) << ","
		<< "use_slots = "   << mysqlpp::quote << inv->getMaxSlots(2) << ","
		<< "setup_slots = " << mysqlpp::quote << inv->getMaxSlots(3) << ","
		<< "etc_slots = "   << mysqlpp::quote << inv->getMaxSlots(4) << ","
		<< "cash_slots = "  << mysqlpp::quote << inv->getMaxSlots(5)
		<< " WHERE id = " << this->id;
	query.exec();
}

void Player::saveVariables() {
	if (variables.size() > 0) {
		mysqlpp::Query query = Database::chardb.query();

		bool firstrun = true;
		for (unordered_map<string, string>::iterator iter = variables.begin(); iter != variables.end(); iter++) {
			if (firstrun) {
				query << "REPLACE INTO character_variables VALUES (";
				firstrun = false;
			}
			else {
				query << ",(";
			}
			query << mysqlpp::quote << this->id << ","
					<< mysqlpp::quote << iter->first << ","
					<< mysqlpp::quote << iter->second << ")";
		}
		query.exec();
	}
}

void Player::saveAll() {
	saveStats();
	saveVariables();
	getInventory()->save();
	getPets()->save();
	getSkills()->save();
	getStorage()->save();
}

void Player::setOnline(bool online) {
	int onlineid = online ? ChannelServer::Instance()->getOnlineId() : 0;
	mysqlpp::Query query = Database::chardb.query();
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = " << mysqlpp::quote << onlineid <<
			", characters.online = " << mysqlpp::quote << online << " WHERE characters.id = " << mysqlpp::quote << this->id;
	query.exec();
}

void Player::acceptDeath() {
	int tomap;
	if (Maps::maps.find(this->getMap()) == Maps::maps.end())
		tomap = this->getMap();
	else
		tomap = Maps::maps[this->getMap()]->getInfo().rm;
	setHP(50, false);
	Skills::stopAllBuffs(this);
	Maps::changeMap(this, tomap, 0);
}

bool Player::hasGMEquip() {
	if (getInventory()->getEquippedID(1) == 1002140) // Hat
		return true;
	if (getInventory()->getEquippedID(5) == 1042003) // Top
		return true;
	if (getInventory()->getEquippedID(6) == 1062007) // Pants
		return true;
	if (getInventory()->getEquippedID(11) == 1322013) // Weapon
		return true;
	return false;
}