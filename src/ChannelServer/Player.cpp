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
#include "Drops.h"
#include "Inventory.h"
#include "Maps.h"
#include "Mobs.h"
#include "NPCs.h"
#include "Reactors.h"
#include "Players.h"
#include "MySQLM.h"
#include "PlayerPacket.h"
#include "Levels.h"
#include "Skills.h"
#include "Quests.h"
#include "Fame.h"
#include "ChannelServer.h"
#include "RecvHeader.h"
#include "WorldServerConnectPlayer.h"
#include "WorldServerConnectPlayerPacket.h"
#include "ServerPacket.h"
#include "Connectable.h"
#include "ReadPacket.h"
#include "Pos.h"
#include "KeyMaps.h"
#include "SkillMacros.h"
#include "Party.h"
#include "BuddyList.h"
#include "Trades.h"
#include "TradesPacket.h"
#include "LevelsPacket.h"

Player::~Player() {
	if (isconnect) {
		if (save_on_dc) {
			save();
			setOnline(false);
		}
		if (this->isTrading() == 1)
			Trades::cancelTrade(this);
		Skills::stopTimersPlayer(this);
		Skills::stopCooldownTimersPlayer(this);
		Inventory::stopTimersPlayer(this);
		WorldServerConnectPlayerPacket::removePlayer(ChannelServer::Instance()->getWorldPlayer(), id);
		Maps::maps[this->getMap()]->removePlayer(this);
		Players::deletePlayer(this);
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
		case RECV_CHAT: Players::chatHandler(this, packet); break;
		case RECV_COMMAND: Players::commandHandler(this, packet); break;
		case RECV_CONTROL_MOB: Mobs::monsterControl(this, packet); break;
		case RECV_DAMAGE_MOB: Mobs::damageMob(this, packet); break;
		case RECV_DAMAGE_MOB_RANGED: Mobs::damageMobRanged(this, packet); break;
		case RECV_DAMAGE_MOB_SPELL: Mobs::damageMobSpell(this, packet); break;
		case RECV_DAMAGE_PLAYER: Players::damagePlayer(this, packet); break;
		case RECV_DROP_MESO: Drops::dropMesos(this, packet); break;
		case RECV_FACE_EXPERIMENT: Players::faceExperiment(this, packet); break;
		case RECV_FAME: Fame::handleFame(this, packet); break;
		case RECV_GET_PLAYER_INFO: Players::getPlayerInfo(this, packet); break;
		case RECV_GET_QUEST: Quests::getQuest(this, packet); break;
		case RECV_GROUP_CHAT: Players::groupChatHandler(this, packet); break;
		case RECV_HEAL_PLAYER: Players::healPlayer(this, packet); break;
		case RECV_HIT_REACTOR: Reactors::hitReactor(this, packet); break;
		case RECV_KEYMAP: changeKey(packet); break;
		case RECV_LOOT_ITEM: Drops::lootItem(this, packet); break;
		case RECV_MOVE_ITEM: Inventory::itemMove(this, packet); break;
		case RECV_MOVE_PLAYER: Players::handleMoving(this, packet); break;
		case RECV_NPC_TALK: NPCs::handleNPC(this, packet); break;
		case RECV_NPC_TALK_CONT: NPCs::handleNPCIn(this, packet); break;
		case RECV_PARTY_ACTION: Party::handleRequest(this, packet); break;
		case RECV_SHOP_ACTION: Trades::tradeHandler(this, packet); break;
		case RECV_SHOP_ENTER: Inventory::useShop(this, packet); break;
		case RECV_USE_STORAGE: Inventory::useStorage(this, packet); break;
		case RECV_SKILL_MACRO: changeSkillMacros(packet); break;
		case RECV_SPECIAL_SKILL: Players::handleSpecialSkills(this, packet); break;
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
		disconnect();
		return;
	}
	this->id = id;
	skills.reset(new PlayerSkills(this));
	quests.reset(new PlayerQuests(this));
	buddyList.reset(new BuddyList(this));

	// Character info
	mysqlpp::Query query = Database::chardb.query();
	query << "SELECT characters.*, users.gm FROM characters LEFT JOIN users on characters.userid = users.id WHERE characters.id = " << mysqlpp::quote << this->id;
	mysqlpp::StoreQueryResult res = query.store();

	if (res.empty()) {
		//hacking
		disconnect();
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

	query << "SELECT inv, slot, itemid, amount, type, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump FROM items WHERE charid = " << mysqlpp::quote << this->id;
	res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		Item *item = new Item;
		item->id = res[i][2];
		item->amount = res[i][3];
		item->type = (unsigned char) res[i][4];
		item->slots = (unsigned char) res[i][5];
		item->scrolls = (unsigned char) res[i][6];
		item->istr = res[i][7];
		item->idex = res[i][8];
		item->iint = res[i][9];
		item->iluk = res[i][10];
		item->ihp = res[i][11];
		item->imp = res[i][12];
		item->iwatk = res[i][13];
		item->imatk = res[i][14];
		item->iwdef = res[i][15];
		item->imdef = res[i][16];
		item->iacc = res[i][17];
		item->iavo = res[i][18];
		item->ihand = res[i][19];
		item->ijump = res[i][20];
		item->ispeed = res[i][21];
		inv->addItem((unsigned char) res[i][0], res[i][1], item);
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

	query << "SELECT itemid, amount, type, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump FROM storageitems WHERE userid = " << mysqlpp::quote << this->userid << " AND world_id = " << (short) this->world_id << " ORDER BY slot ASC";
	res = query.store();
	for (size_t i = 0; i < res.num_rows(); i++) {
		Item *item = new Item;
		item->id = res[i][0];
		item->amount = res[i][1];
		item->type = (unsigned char) res[i][2];
		item->slots = (unsigned char) res[i][3];
		item->scrolls = (unsigned char) res[i][4];
		item->istr = res[i][5];
		item->idex = res[i][6];
		item->iint = res[i][7];
		item->iluk = res[i][8];
		item->ihp = res[i][9];
		item->imp = res[i][10];
		item->iwatk = res[i][11];
		item->imatk = res[i][12];
		item->iwdef = res[i][13];
		item->imdef = res[i][14];
		item->iacc = res[i][15];
		item->iavo = res[i][16];
		item->ihand = res[i][17];
		item->ijump = res[i][18];
		item->ispeed = res[i][19];
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

	PlayerPacket::connectData(this);
	
	if (ChannelServer::Instance()->getScrollingHeader().size() > 0) {
		ServerPacket::showScrollingHeader(this, ChannelServer::Instance()->getScrollingHeader());
	}

	pos = Maps::maps[map]->getSpawnPoint()->pos;

	type = 0;
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
}

void Player::setMP(int mp, bool is) {
	if (!(skills->getActiveSkillLevel(2121004) > 0 || skills->getActiveSkillLevel(2221004) > 0 || skills->getActiveSkillLevel(2321004) > 0)) {
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
	PlayerPacket::updateStat(this, 0x800, mhp);
}

void Player::setRMHP(int rmhp) {
	if (rmhp > 30000) { rmhp = 30000; }
	this->rmhp = rmhp;
	PlayerPacket::updateStat(this, 0x800, rmhp);
}

void Player::setMMP(int mmp) {
	if (mmp > 30000) { mmp = 30000; }
	this->mmp = mmp;
	PlayerPacket::updateStat(this, 0x2000, mmp);
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
		disconnect();
		return true;
	}
	return false;
}

void Player::saveSkills() {
	mysqlpp::Query query = Database::chardb.query();

	bool firstrun = true;
	for (int i = 0; i < skills->getSkillsNum(); i++) {
		if (firstrun) {
			query << "REPLACE INTO skills VALUES (" << mysqlpp::quote << this->id << "," << mysqlpp::quote << skills->getSkillID(i) << "," << mysqlpp::quote << skills->getSkillLevel(skills->getSkillID(i)) << "," << mysqlpp::quote << skills->getMaxSkillLevel(skills->getSkillID(i)) << ")";
			firstrun = false;
		}
		else {
			query << ",(" << mysqlpp::quote << this->id << "," << mysqlpp::quote << skills->getSkillID(i) << "," << mysqlpp::quote << skills->getSkillLevel(skills->getSkillID(i)) << "," << mysqlpp::quote << skills->getMaxSkillLevel(skills->getSkillID(i)) << ")";
		}
	}
	query.exec();
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

void Player::saveInventory() {
	mysqlpp::Query query = Database::chardb.query();

	query << "DELETE FROM items WHERE charid = " << mysqlpp::quote << this->id;
	query.exec();

	bool firstrun = true;
	for (char i = 1; i <= 5; i++) {
		iteminventory *items = inv->getItems(i);
		for (iteminventory::iterator iter = items->begin(); iter != items->end(); iter++) {
			Item *item = iter->second;
			if (firstrun) {
				query << "INSERT INTO items VALUES (";
				firstrun = false;
			}
			else {
				query << ",(";
			}
			query << mysqlpp::quote << this->id << ","
				<< mysqlpp::quote << (short) i << ","
				<< mysqlpp::quote << iter->first << ","
				<< mysqlpp::quote << item->id << ","
				<< mysqlpp::quote << item->amount << ","
				<< mysqlpp::quote << (short) item->type << ","
				<< mysqlpp::quote << (short) item->slots << ","
				<< mysqlpp::quote << (short) item->scrolls << ","
				<< mysqlpp::quote << item->istr << ","
				<< mysqlpp::quote << item->idex << ","
				<< mysqlpp::quote << item->iint << ","
				<< mysqlpp::quote << item->iluk << ","
				<< mysqlpp::quote << item->ihp << ","
				<< mysqlpp::quote << item->imp << ","
				<< mysqlpp::quote << item->iwatk << ","
				<< mysqlpp::quote << item->imatk << ","
				<< mysqlpp::quote << item->iwdef << ","
				<< mysqlpp::quote << item->imdef << ","
				<< mysqlpp::quote << item->iacc << ","
				<< mysqlpp::quote << item->iavo << ","
				<< mysqlpp::quote << item->ihand << ","
				<< mysqlpp::quote << item->ijump << ","
				<< mysqlpp::quote << item->ispeed << ")";
		}
	}
	query.exec();
}

void Player::saveStorage() {
	mysqlpp::Query query = Database::getCharQuery();
	query << "REPLACE INTO storage VALUES ("
		<< this->userid << ", "
		<< (short) this->world_id << ", "
		<< (short) storage->getSlots() << ", "
		<< storage->getMesos() << ")";
	query.exec();

	query << "DELETE FROM storageitems WHERE userid = " << this->userid << " AND world_id = " << (short) this->world_id;
	query.exec();

	bool firstrun = true;
	for (char i = 0; i < this->storage->getNumItems(); i++) {
		if (firstrun) {
			query << "INSERT INTO storageitems VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		Item *item = this->storage->getItem(i);
		query << mysqlpp::quote << this->userid << ","
			<< mysqlpp::quote << (short) this->world_id << ","
			<< mysqlpp::quote << (short) i << ","
			<< mysqlpp::quote << item->id << ","
			<< mysqlpp::quote << item->amount << ","
			<< mysqlpp::quote << (short) item->type << ","
			<< mysqlpp::quote << (short) item->slots << ","
			<< mysqlpp::quote << (short) item->scrolls << ","
			<< mysqlpp::quote << item->istr << ","
			<< mysqlpp::quote << item->idex << ","
			<< mysqlpp::quote << item->iint << ","
			<< mysqlpp::quote << item->iluk << ","
			<< mysqlpp::quote << item->ihp << ","
			<< mysqlpp::quote << item->imp << ","
			<< mysqlpp::quote << item->iwatk << ","
			<< mysqlpp::quote << item->imatk << ","
			<< mysqlpp::quote << item->iwdef << ","
			<< mysqlpp::quote << item->imdef << ","
			<< mysqlpp::quote << item->iacc << ","
			<< mysqlpp::quote << item->iavo << ","
			<< mysqlpp::quote << item->ihand << ","
			<< mysqlpp::quote << item->ijump << ","
			<< mysqlpp::quote << item->ispeed << ")";
	}
	query.exec();
}

void Player::saveVariables() {
	if (variables.size() > 0) {
		mysqlpp::Query query = Database::chardb.query();

		bool firstrun = true;
		for (hash_map <string, string>::iterator iter = variables.begin(); iter != variables.end(); iter++) {
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

void Player::save() {
	saveSkills();
	saveStats();
	saveInventory();
	saveStorage();
	saveVariables();
}

void Player::setOnline(bool online) {
	int onlineid = online ? ChannelServer::Instance()->getOnlineId() : 0;
	mysqlpp::Query query = Database::chardb.query();
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = " << mysqlpp::quote << onlineid <<
			", characters.online = " << mysqlpp::quote << online << " WHERE characters.id = " << mysqlpp::quote << this->id;
	query.exec();
}

void Player::acceptDeath(int mapid) {
	setHP(50, false);
	Skills::stopAllBuffs(this);
	Maps::changeMap(this, mapid, 0);
}
