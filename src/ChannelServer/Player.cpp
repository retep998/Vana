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
#include "Party.h"
#include "BuddyList.h"

Player::~Player() {
	if (isconnect) {
		if (save_on_dc) {
			save();
			setOnline(false);
		}
		Skills::stopTimerPlayer(this);
		Inventory::stopTimerPlayer(this);
		WorldServerConnectPlayerPacket::removePlayer(ChannelServer::Instance()->getWorldPlayer(), id);
		Maps::maps[this->getMap()]->removePlayer(this);
		Players::deletePlayer(this);
	}
}

void Player::realHandleRequest(ReadPacket *packet) {
	switch(packet->getShort()) {
		case RECV_CHANNEL_LOGIN: playerConnect(packet); break;
		case RECV_NPC_TALK_CONT: NPCs::handleNPCIn(this, packet); break;
		case RECV_SHOP_ENTER: Inventory::useShop(this, packet); break;
		case RECV_NPC_TALK: NPCs::handleNPC(this, packet); break;
		case RECV_CHANGE_CHANNEL: changeChannel(packet->getByte()); break;
		case RECV_DAMAGE_PLAYER: Players::damagePlayer(this, packet); break;
		case RECV_STOP_CHAIR: Inventory::stopChair(this, packet); break;
		case RECV_CHAT: Players::chatHandler(this, packet); break;
		case RECV_USE_CHAIR: Inventory::useChair(this, packet); break;
		case RECV_USE_ITEM_EFFECT: Inventory::useItemEffect(this, packet); break;
		case RECV_DAMAGE_MOB_SPELL: Mobs::damageMobSpell(this, packet); break;
		case RECV_CHANGE_MAP: Maps::moveMap(this, packet); break;
		case RECV_MOVE_PLAYER: Players::handleMoving(this, packet); break;
		case RECV_PARTY_ACTION: Party::handleRequest(this, packet); break;
		case RECV_DAMAGE_MOB_RANGED: Mobs::damageMobRanged(this, packet); break;
		case RECV_GROUP_CHAT: Players::groupChatHandler(this, packet); break;
		case RECV_GET_PLAYER_INFO: Players::getPlayerInfo(this, packet); break;
		case RECV_CHANGE_MAP_SPECIAL: Maps::moveMapS(this, packet); break; // Portals that cause scripted events
		case RECV_USE_SUMMON_BAG: Inventory::useSummonBag(this, packet); break;
		case RECV_ADD_SKILL: Skills::addSkill(this, packet); break;
		case RECV_CANCEL_SKILL: Skills::cancelSkill(this, packet); break;
		case RECV_USE_SKILL: Skills::useSkill(this, packet); break;
		case RECV_USE_CASH_ITEM: Inventory::useCashItem(this, packet); break;
		case RECV_COMMAND: Players::commandHandler(this, packet); break;
		case RECV_DAMAGE_MOB: Mobs::damageMob(this, packet); break;
		case RECV_FACE_EXPERIMENT: Players::faceExperiment(this, packet); break;
		case RECV_HIT_REACTOR: Reactors::hitReactor(this, packet); break;
		case RECV_MOVE_ITEM: Inventory::itemMove(this, packet); break;
		case RECV_USE_ITEM: Inventory::useItem(this, packet); break;
		case RECV_CANCEL_ITEM: Inventory::cancelItem(this, packet); break;
		case RECV_USE_SKILLBOOK: Inventory::useSkillbook(this, packet); break; // Skillbooks
		case RECV_USE_RETURN_SCROLL: Inventory::useReturnScroll(this, packet); break;
		case RECV_USE_SCROLL: Inventory::useScroll(this, packet); break;
		case RECV_ADD_STAT: Levels::addStat(this, packet); break;
		case RECV_HEAL_PLAYER: Players::healPlayer(this, packet); break;
		case RECV_DROP_MESO: Drops::dropMesos(this, packet); break;
		case RECV_FAME: Fame::handleFame(this, packet); break;
		case RECV_GET_QUEST: Quests::getQuest(this, packet); break;
		case RECV_KEYMAP: changeKey(packet); break;
		case RECV_LOOT_ITEM: Drops::lootItem(this, packet); break;
		case RECV_CONTROL_MOB: Mobs::monsterControl(this, packet); break;
		case RECV_SPECIAL_SKILL: Players::handleSpecialSkills(this, packet); break;
	}
}

void Player::playerConnect(ReadPacket *packet) {
	int id = packet->getInt();
	if (!Connectable::Instance()->checkPlayer(id)) {
		//hacking
		disconnect();
		return;
	}
	setPlayerid(id);
	inv.reset(new PlayerInventory(this));
	skills.reset(new PlayerSkills(this));
	quests.reset(new PlayerQuests(this));
	buddyList.reset(new BuddyList(this));

	mysqlpp::Query query = db.query();
	query << "SELECT characters.*, users.gm FROM characters LEFT JOIN users on characters.userid = users.id WHERE characters.id = " << mysqlpp::quote << getPlayerid();
	mysqlpp::StoreQueryResult res = query.store();

	if (res.empty()) {
		//hacking
		disconnect();
		return;
	}

	strcpy_s(name, res[0]["name"]);
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

	inv->setMesosStart(res[0]["mesos"]);

	query << "SELECT pos, equipid, type, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump FROM equip WHERE charid = " << mysqlpp::quote << getPlayerid();
	res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		Equip *equip = new Equip;
		equip->id = res[i][1];
		equip->type = (unsigned char) res[i][2];
		equip->slots = (unsigned char) res[i][3];
		equip->scrolls = (unsigned char) res[i][4];
		equip->istr = res[i][5];
		equip->idex = res[i][6];
		equip->iint = res[i][7];
		equip->iluk = res[i][8];
		equip->ihp = res[i][9];
		equip->imp = res[i][10];
		equip->iwatk = res[i][11];
		equip->imatk = res[i][12];
		equip->iwdef = res[i][13];
		equip->imdef = res[i][14];
		equip->iacc = res[i][15];
		equip->iavo = res[i][16];
		equip->ihand = res[i][17];
		equip->ijump = res[i][18];
		equip->ispeed = res[i][19];
		inv->addEquip(res[i][0], equip);
	}

	query << "SELECT inv, pos, itemid, amount FROM items WHERE charid = " << mysqlpp::quote << getPlayerid();
	res = query.store();
	for (size_t i = 0; i < res.num_rows(); ++i) {
		Item *item = new Item;
		item->id = res[i][2];
		item->amount = res[i][3];
		inv->addItem((unsigned char) res[i][0], res[i][1], item);
	}

	query << "SELECT skillid, points, maxlevel FROM skills WHERE charid = " << mysqlpp::quote << getPlayerid();
	res = query.store();
	for (size_t i = 0; i < res.num_rows(); ++i) {
		skills->addSkillLevel(res[i][0], res[i][1], false);
		if (FORTHJOB_SKILL(res[i][0])) {
			skills->setMaxSkillLevel(res[i][0], res[i][2]);
		}
	}

	KeyMaps keyMaps;
	keyMaps.load(getPlayerid());

	query << "SELECT * FROM character_variables WHERE charid = " << mysqlpp::quote << getPlayerid();
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

	pos = Maps::maps[map]->getPortalByID(0)->pos;

	type = 0;
	PlayerPacket::showKeys(this, &keyMaps);
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
	if (mp < 0)
		this->mp = 0;
	else if (mp > mmp)
		this->mp = mmp;
	else
		this->mp = mp;
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
	WorldServerConnectPlayerPacket::updateJob(ChannelServer::Instance()->getWorldPlayer(), this->getPlayerid(), job);
	PlayerPacket::updateStat(this, 0x20, job);
}

void Player::setExp(int exp, bool is) {
	this->exp = exp;
	if (is)
		PlayerPacket::updateStat(this, 0x10000, exp);
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
	PlayerPacket::updateStat(this, 0x800, getRMHP());
}

void Player::setRMHP(int rmhp) {
	if (rmhp > 30000) { rmhp = 30000; }
	this->rmhp = rmhp;
	PlayerPacket::updateStat(this, 0x800, rmhp);
}

void Player::setMMP(int mmp) {
	if (mmp > 30000) { mmp = 30000; }
	this->mmp = mmp;
	PlayerPacket::updateStat(this, 0x800, getRMHP());
}

void Player::setRMMP(int rmmp) {
	if (rmmp > 30000) { rmmp = 30000; }
	this->rmmp = rmmp;
	PlayerPacket::updateStat(this, 0x2000, rmmp);
}

void Player::setLevel(int level) {
	this->level = (unsigned char) level;
	WorldServerConnectPlayerPacket::updateLevel(ChannelServer::Instance()->getWorldPlayer(), getPlayerid(), level);
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
	keyMaps.save(getPlayerid());
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

bool Player::addWarning() {
	int t = clock();
	// Deleting old warnings
	for (unsigned int i = 0; i < warnings.size(); i++) {
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
	mysqlpp::Query query = db.query();

	bool firstrun = true;
	for (int i = 0; i < skills->getSkillsNum(); i++) {
		if (firstrun) {
			query << "REPLACE INTO skills VALUES (" << mysqlpp::quote << getPlayerid() << "," << mysqlpp::quote << skills->getSkillID(i) << "," << mysqlpp::quote << skills->getSkillLevel(skills->getSkillID(i)) << "," << mysqlpp::quote << skills->getMaxSkillLevel(skills->getSkillID(i)) << ")";
			firstrun = false;
		}
		else {
			query << ",(" << mysqlpp::quote << getPlayerid() << "," << mysqlpp::quote << skills->getSkillID(i) << "," << mysqlpp::quote << skills->getSkillLevel(skills->getSkillID(i)) << "," << mysqlpp::quote << skills->getMaxSkillLevel(skills->getSkillID(i)) << ")";
		}
	}
	query.exec();
}

void Player::saveStats() {
	mysqlpp::Query query = db.query();
	query << "UPDATE characters SET "
			<< "level = " << mysqlpp::quote << getLevel() << ","
			<< "job = " << mysqlpp::quote << getJob() << ","
			<< "str = " << mysqlpp::quote << getStr() << ","
			<< "dex = " << mysqlpp::quote << getDex() << ","
			<< "`int` = " << mysqlpp::quote << getInt() << ","
			<< "luk = " << mysqlpp::quote << getLuk() << ","
			<< "chp = " << mysqlpp::quote << getHP() << ","
			<< "mhp = " << mysqlpp::quote << getRMHP() << ","
			<< "cmp = " << mysqlpp::quote << getMP() << ","
			<< "mmp = " << mysqlpp::quote << getRMMP() << ","
			<< "hpmp_ap = " << mysqlpp::quote << getHPMPAp() << ","
			<< "ap = " << mysqlpp::quote << getAp() << ","
			<< "sp = " << mysqlpp::quote << getSp() << ","
			<< "exp = " << mysqlpp::quote << getExp() << ","
			<< "fame = " << mysqlpp::quote << getFame() << ","
			<< "map = " << mysqlpp::quote << getMap() << ","
			<< "gender = " << mysqlpp::quote << (short) getGender() << ","
			<< "skin = " << mysqlpp::quote << (short) getSkin() << ","
			<< "eyes = " << mysqlpp::quote << getEyes() << ","
			<< "hair = " << mysqlpp::quote << getHair() << ","
			<< "mesos = " << mysqlpp::quote << inv->getMesos()
			<< " WHERE id = " << getPlayerid();
	query.exec();
}

void Player::saveEquips() {
	mysqlpp::Query query = db.query();
	query << "DELETE FROM equip WHERE charid = " << mysqlpp::quote << this->getPlayerid();
	query.exec();

	bool firstrun = true;
	equipinventory *equips = inv->getEquips();
	for (equipinventory::iterator iter = equips->begin(); iter != equips->end(); iter++) {
		Equip *equip = iter->second;
		if (firstrun) {
			query << "INSERT INTO equip VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << mysqlpp::quote << getPlayerid() << ","
			<< mysqlpp::quote << iter->first << ","
			<< mysqlpp::quote << equip->id << ","
			<< mysqlpp::quote << (short) equip->type << ","
			<< mysqlpp::quote << (short) equip->slots << ","
			<< mysqlpp::quote << (short) equip->scrolls << ","
			<< mysqlpp::quote << equip->istr << ","
			<< mysqlpp::quote << equip->idex << ","
			<< mysqlpp::quote << equip->iint << ","
			<< mysqlpp::quote << equip->iluk << ","
			<< mysqlpp::quote << equip->ihp << ","
			<< mysqlpp::quote << equip->imp << ","
			<< mysqlpp::quote << equip->iwatk << ","
			<< mysqlpp::quote << equip->imatk << ","
			<< mysqlpp::quote << equip->iwdef << ","
			<< mysqlpp::quote << equip->imdef << ","
			<< mysqlpp::quote << equip->iacc << ","
			<< mysqlpp::quote << equip->iavo << ","
			<< mysqlpp::quote << equip->ihand << ","
			<< mysqlpp::quote << equip->ijump << ","
			<< mysqlpp::quote << equip->ispeed << ")";
	}
	query.exec();
}

void Player::saveItems() {
	mysqlpp::Query query = db.query();
	query << "DELETE FROM items WHERE charid = " << mysqlpp::quote << this->getPlayerid();
	query.exec();

	bool firstrun = true;
	for (char i = 2; i <= 5; i++) {
		for (short s = 1; s <= inv->getMaxslots(i); s++) {
			Item *item = inv->getItem(i, s);
			if (item == 0)
				continue;
			if (firstrun) {
				query << "INSERT INTO items VALUES (";
				firstrun = false;
			}
			else {
				query << ",(";
			}
			query << mysqlpp::quote << getPlayerid() << ","
				<< mysqlpp::quote << (short) i << ","
				<< mysqlpp::quote << s << ","
				<< mysqlpp::quote << item->id << ","
				<< mysqlpp::quote << item->amount << ")";
		}
	}
	query.exec();
}

void Player::saveVariables() {
	if (variables.size() > 0) {
		mysqlpp::Query query = db.query();

		bool firstrun = true;
		for (hash_map <string, string>::iterator iter = variables.begin(); iter != variables.end(); iter++) {
			if (firstrun) {
				query << "REPLACE INTO character_variables VALUES (";
				firstrun = false;
			}
			else {
				query << ",(";
			}
			query << mysqlpp::quote << getPlayerid() << ","
					<< mysqlpp::quote << iter->first << ","
					<< mysqlpp::quote << iter->second << ")";
		}
		query.exec();
	}
}

void Player::save() {
	saveSkills();
	saveStats();
	saveEquips();
	saveItems();
	saveVariables();
}

void Player::setOnline(bool online) {
	int onlineid = online ? ChannelServer::Instance()->getOnlineId() : 0;
	mysqlpp::Query query = db.query();
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = " << mysqlpp::quote << onlineid <<
			", characters.online = " << mysqlpp::quote << online << " WHERE characters.id = " << mysqlpp::quote << getPlayerid();
	query.exec();
}
