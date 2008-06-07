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
#include "BufferUtilities.h"
#include "ChannelServer.h"
#include "RecvHeader.h"
#include "WorldServerConnectPlayer.h"
#include "WorldServerConnectPlayerPacket.h"
#include "ServerPacket.h"
#include "Connectable.h"
#include "ReadPacket.h"
#include "Pos.h"
#include "KeyMaps.h"

Player::~Player() {
	if (isconnect) {
		if (save_on_dc)
			save();
		Skills::stopTimerPlayer(this);
		Inventory::stopTimerPlayer(this);
		WorldServerConnectPlayerPacket::removePlayer(ChannelServer::Instance()->getWorldPlayer(), id);
		Maps::removePlayer(this);
		Players::deletePlayer(this);
		setOnline(false);
	}
}

void Player::realHandleRequest(ReadPacket *packet) {
	switch(packet->getShort()) {
		case RECV_CHANNEL_LOGIN: playerConnect(packet); break;
		case RECV_NPC_TALK_CONT: NPCs::handleNPCIn(this, packet); break;
		case RECV_SHOP_ENTER: Inventory::useShop(this, packet); break;
		case RECV_NPC_TALK: NPCs::handleNPC(this, packet); break;
		case RECV_CHANGE_CHANNEL: changeChannel(packet->getByte()); break;
		case RECV_DAMAGE_PLAYER: Players::damagePlayer(this, packet->getBuffer()); break;
		case RECV_STOP_CHAIR: Inventory::stopChair(this, packet); break;
		case RECV_CHAT: Players::chatHandler(this, packet->getBuffer()); break;
		case RECV_USE_CHAIR: Inventory::useChair(this, packet); break;
		case RECV_USE_ITEM_EFFECT: Inventory::useItemEffect(this, packet); break;
		case RECV_DAMAGE_MOB_SPELL: Mobs::damageMobSpell(this, packet->getBuffer()); break;
		case RECV_CHANGE_MAP: Maps::moveMap(this, packet); break;
		case RECV_MOVE_PLAYER: Players::handleMoving(this, packet->getBuffer(), packet->getBufferLength()); break;
		case RECV_DAMAGE_MOB_RANGED: Mobs::damageMobRanged(this, packet->getBuffer(), packet->getBufferLength()); break;
		case RECV_GET_PLAYER_INFO: Players::getPlayerInfo(this, packet->getBuffer()); break;
		case RECV_CHANGE_MAP_SPECIAL: Maps::moveMapS(this, packet); break; // Portals that cause scripted events
		case RECV_USE_SUMMON_BAG: Inventory::useSummonBag(this, packet); break;
		case RECV_ADD_SKILL: Skills::addSkill(this, packet); break;
		case RECV_CANCEL_SKILL: Skills::cancelSkill(this, packet); break;
		case RECV_USE_SKILL: Skills::useSkill(this, packet); break;
		case RECV_USE_CASH_ITEM: Inventory::useCashItem(this, packet); break;
		case RECV_COMMAND: Players::commandHandler(this, packet->getBuffer()); break;
		case RECV_DAMAGE_MOB: Mobs::damageMob(this, packet->getBuffer()); break;
		case RECV_FACE_EXPERIMENT: Players::faceExperiment(this, packet->getBuffer()); break;
		case RECV_HIT_REACTOR: Reactors::hitReactor(this, packet); break;
		case RECV_MOVE_ITEM: Inventory::itemMove(this, packet); break;
		case RECV_USE_ITEM: Inventory::useItem(this, packet); break;
		case RECV_CANCEL_ITEM: Inventory::cancelItem(this, packet); break;
		case RECV_USE_SKILLBOOK: Inventory::useSkillbook(this, packet); break; // Skillbooks
		case RECV_USE_RETURN_SCROLL: Inventory::useReturnScroll(this, packet); break;
		case RECV_USE_SCROLL: Inventory::useScroll(this, packet); break;
		case RECV_ADD_STAT: Levels::addStat(this, packet); break;
		case RECV_HEAL_PLAYER: Players::healPlayer(this, packet->getBuffer()); break;
		case RECV_DROP_MESO: Drops::dropMesos(this, packet); break;
		case RECV_FAME: Fame::handleFame(this, packet); break;
		case RECV_GET_QUEST: Quests::getQuest(this, packet); break;
		case RECV_KEYMAP: changeKey(packet);
		case RECV_LOOT_ITEM: Drops::lootItem(this, packet); break;
		case RECV_CONTROL_MOB: Mobs::monsterControl(this, packet->getBuffer(), packet->getBufferLength()); break;
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
	inv.reset(new PlayerInventory);
	skills.reset(new PlayerSkills);
	quests.reset(new PlayerQuests);
	skills->setPlayer(this);
	quests->setPlayer(this);

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
	inv->setPlayer(this);

	query << "SELECT * FROM equip WHERE charid = " << mysqlpp::quote << getPlayerid();
	res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		Equip* equip = new Equip;
		equip->pos = res[i][1];
		equip->id = res[i][2];
		equip->type = (unsigned char) res[i][3];
		equip->slots = (unsigned char) res[i][4];
		equip->scrolls = res[i][5];
		equip->istr = res[i][6];
		equip->idex = res[i][7];
		equip->iint = res[i][8];
		equip->iluk = res[i][9];
		equip->ihp = res[i][10];
		equip->imp = res[i][11];
		equip->iwatk = res[i][12];
		equip->imatk = res[i][13];
		equip->iwdef = res[i][14];
		equip->imdef = res[i][15];
		equip->iacc = res[i][16];
		equip->iavo = res[i][17];
		equip->ihand = res[i][18];
		equip->ijump = res[i][19];
		equip->ispeed = res[i][20];
		inv->addEquip(equip);
	}

	query << "SELECT inv, pos, itemid, amount FROM items WHERE charid = " << mysqlpp::quote << getPlayerid();
	res = query.store();
	for (size_t i = 0; i < res.num_rows(); ++i) {
		Item* item = new Item;
		item->inv = (unsigned char) res[i][0];
		item->pos = res[i][1];
		item->id = res[i][2];
		item->amount = res[i][3];
		inv->addItem(item);
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

	if (Maps::info[map].forcedReturn != 999999999) {
		map = Maps::info[map].forcedReturn;
	}

	PlayerPacket::connectData(this);
	
	if (ChannelServer::Instance()->getScrollingHeader().size() > 0) {
		ServerPacket::changeScrollingHeader(ChannelServer::Instance()->getScrollingHeader());
	}

	pos.x = Maps::info[map].Portals[0].x;
	pos.y = Maps::info[map].Portals[0].y;

	type = 0;
	PlayerPacket::showKeys(this, &keyMaps);
	Maps::newMap(this, map);

	setOnline(true);
	isconnect = true;
	WorldServerConnectPlayerPacket::registerPlayer(ChannelServer::Instance()->getWorldPlayer(), id, name);
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

void Player::setMMP(int mmp) {
	if (mmp > 30000) { mmp = 30000; }
	this->mmp = mmp;
	PlayerPacket::updateStat(this, 0x2000, getRMMP());
}

void Player::setLevel(int level) {
	this->level = (unsigned char) level;
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
	PlayerPacket::newHair(this);
}

void Player::setEyes(int id) {
	this->eyes = id;
	PlayerPacket::newEyes(this);
}

void Player::setSkin(char id) {
	this->skin = id;
	PlayerPacket::newSkin(this);
}

bool Player::addWarning() {
	int t = clock();
	// Deleting old warnings
	for (unsigned int i=0; i<warnings.size(); i++) {
		if (warnings[i] + 300000 < t) {
			warnings.erase(warnings.begin()+i);
			i--;
		}
	}
	warnings.push_back(t);
	if (warnings.size()>50) {
		// Hacker - Temp DCing
		disconnect();
		return true;
	}
	return false;
}

void Player::saveSkills() {
	mysqlpp::Query query = db.query();

	bool firstrun = true;
	for (int i=0; i<skills->getSkillsNum(); i++) {
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
	for (int i=0; i<inv->getEquipNum(); i++) {
		if (firstrun) {
			query << "INSERT INTO equip VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << mysqlpp::quote << getPlayerid() << ","
				<< mysqlpp::quote << (short) inv->getEquipPos(i) << ","
				<< mysqlpp::quote << inv->getEquip(i)->id << ","
				<< mysqlpp::quote << (short) Drops::equips[inv->getEquip(i)->id].type << ","
				<< mysqlpp::quote << (short) inv->getEquip(i)->slots << ","
				<< mysqlpp::quote << inv->getEquip(i)->scrolls << ","
				<< mysqlpp::quote << inv->getEquip(i)->istr << ","
				<< mysqlpp::quote << inv->getEquip(i)->idex << ","
				<< mysqlpp::quote << inv->getEquip(i)->iint << ","
				<< mysqlpp::quote << inv->getEquip(i)->iluk << ","
				<< mysqlpp::quote << inv->getEquip(i)->ihp << ","
				<< mysqlpp::quote << inv->getEquip(i)->imp << ","
				<< mysqlpp::quote << inv->getEquip(i)->iwatk << ","
				<< mysqlpp::quote << inv->getEquip(i)->imatk << ","
				<< mysqlpp::quote << inv->getEquip(i)->iwdef << ","
				<< mysqlpp::quote << inv->getEquip(i)->imdef << ","
				<< mysqlpp::quote << inv->getEquip(i)->iacc << ","
				<< mysqlpp::quote << inv->getEquip(i)->iavo << ","
				<< mysqlpp::quote << inv->getEquip(i)->ihand << ","
				<< mysqlpp::quote << inv->getEquip(i)->ijump << ","
				<< mysqlpp::quote << inv->getEquip(i)->ispeed << ")";
	}
	query.exec();
}

void Player::saveItems() {
	mysqlpp::Query query = db.query();
	query << "DELETE FROM items WHERE charid = " << mysqlpp::quote << this->getPlayerid();
	query.exec();

	bool firstrun = true;
	for (int i=0; i<inv->getItemNum(); i++) {
		if (firstrun) {
			query << "INSERT INTO items VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << mysqlpp::quote << getPlayerid() << ","
				<< mysqlpp::quote << (short) inv->getItem(i)->inv << ","
				<< mysqlpp::quote << inv->getItem(i)->pos << ","
				<< mysqlpp::quote << inv->getItem(i)->id << ","
				<< mysqlpp::quote << inv->getItem(i)->amount << ")";
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
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = " << mysqlpp::quote << onlineid << " WHERE characters.id = " << mysqlpp::quote << getPlayerid();
	query.exec();
}
