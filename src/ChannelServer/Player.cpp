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
#include "BuddyListHandler.h"
#include "BuddyListPacket.h"
#include "ChannelServer.h"
#include "ChatHandler.h"
#include "CommandHandler.h"
#include "Connectable.h"
#include "Database.h"
#include "Drops.h"
#include "Fame.h"
#include "Inventory.h"
#include "KeyMaps.h"
#include "Levels.h"
#include "LevelsPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "Mobs.h"
#include "NPCs.h"
#include "PacketReader.h"
#include "Party.h"
#include "Pets.h"
#include "PlayerHandler.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "Quests.h"
#include "Reactors.h"
#include "RecvHeader.h"
#include "ServerPacket.h"
#include "SkillMacros.h"
#include "Skills.h"
#include "Summons.h"
#include "Trades.h"
#include "WorldServerConnectPlayer.h"
#include "WorldServerConnectPacket.h"

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
		WorldServerConnectPacket::removePlayer(ChannelServer::Instance()->getWorldPlayer(), id);
		Maps::getMap(this->getMap())->removePlayer(this);
		Players::Instance()->removePlayer(this);
	}
}

void Player::realHandleRequest(PacketReader &packet) {
	switch (packet.getShort()) {
		case RECV_ADD_SKILL: Skills::addSkill(this, packet); break;
		case RECV_ADD_STAT: Levels::addStat(this, packet); break;
		case RECV_BUDDYLIST: BuddyListHandler::handleBuddyList(this, packet); break;
		case RECV_CANCEL_ITEM: Inventory::cancelItem(this, packet); break;
		case RECV_CANCEL_SKILL: Skills::cancelSkill(this, packet); break;
		case RECV_CHANGE_CHANNEL: changeChannel(packet.getByte()); break;
		case RECV_CHANGE_MAP: Maps::usePortal(this, packet); break;
		case RECV_CHANGE_MAP_SPECIAL: Maps::useScriptedPortal(this, packet); break;
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
		case RECV_LOOT_ITEM: Drops::player_loot(this, packet); break;
		case RECV_PET_LOOT: Drops::pet_loot(this, packet); break;
		case RECV_MOVE_ITEM: Inventory::itemMove(this, packet); break;
		case RECV_MOVE_PLAYER: PlayerHandler::handleMoving(this, packet); break;
		case RECV_NPC_TALK: NPCs::handleNPC(this, packet); break;
		case RECV_NPC_TALK_CONT: NPCs::handleNPCIn(this, packet); break;
		case RECV_ANIMATE_NPC: NPCs::handleNPCAnimation(this, packet); break;
		case RECV_PARTY_ACTION: Party::handleRequest(this, packet); break;
		case RECV_PET_CHAT: Pets::chat(this, packet); break;
		case RECV_PET_COMMAND: Pets::showAnimation(this, packet); break;
		case RECV_PET_FEED: Pets::feedPet(this, packet); break;
		case RECV_PET_MOVE: Pets::movePet(this, packet); break;
		case RECV_PET_SUMMON: Pets::summonPet(this, packet); break;
		case RECV_MOVE_SUMMON: Summons::moveSummon(this, packet); break;
		case RECV_DAMAGE_MOB_SUMMON: Mobs::damageMobSummon(this, packet); break;
		case RECV_DAMAGE_SUMMON: Summons::damageSummon(this, packet); break;
		case RECV_PLAYER_ROOM_ACTION: Trades::tradeHandler(this, packet); break;
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
		case RECV_USE_SKILLBOOK: Inventory::useSkillbook(this, packet); break;
		case RECV_USE_SUMMON_BAG: Inventory::useSummonBag(this, packet); break;
	}
}

void Player::playerConnect(PacketReader &packet) {
	int32_t id = packet.getInt();
	if (!Connectable::Instance()->checkPlayer(id)) {
		// Hacking
		getSession()->disconnect();
		return;
	}
	this->id = id;
	activeBuffs.reset(new PlayerActiveBuffs(this));
	summons.reset(new PlayerSummons(this));
	buddyList.reset(new PlayerBuddyList(this));
	quests.reset(new PlayerQuests(this));
	pets.reset(new PlayerPets(this));
	// Character info
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT characters.*, users.gm FROM characters LEFT JOIN users on characters.userid = users.id WHERE characters.id = " << this->id;
	mysqlpp::StoreQueryResult res = query.store();

	if (res.empty()) {
		// Hacking
		getSession()->disconnect();
		return;
	}

	userid = res[0]["userid"];
	world_id = (uint8_t) res[0]["world_id"];
	res[0]["name"].to_string(name);
	gender = (uint8_t) res[0]["gender"];
	skin = (uint8_t) res[0]["skin"];
	eyes = res[0]["eyes"];
	hair = res[0]["hair"];
	level = (uint8_t) res[0]["level"];
	job = (int16_t) res[0]["job"];
	str = (int16_t) res[0]["str"];
	dex = (int16_t) res[0]["dex"];
	intt = (int16_t) res[0]["int"];
	luk = (int16_t) res[0]["luk"];
	hp = (int16_t) res[0]["chp"];
	rmhp = mhp = (int16_t) res[0]["mhp"];
	mp = (int16_t) res[0]["cmp"];
	rmmp = mmp = (int16_t) res[0]["mmp"];
	hpmp_ap = (int16_t) res[0]["hpmp_ap"];
	ap = (int16_t) res[0]["ap"];
	sp = (int16_t) res[0]["sp"];
	exp = res[0]["exp"];
	fame = (int16_t) res[0]["fame"];
	map = res[0]["map"];
	mappos = (uint8_t) res[0]["pos"];
	buddylist_size = res[0]["buddylist_size"];
	gm = res[0]["gm"];

	// Inventory
	uint8_t maxslots[5];
	maxslots[0] = (uint8_t) res[0]["equip_slots"];
	maxslots[1] = (uint8_t) res[0]["use_slots"];
	maxslots[2] = (uint8_t) res[0]["setup_slots"];
	maxslots[3] = (uint8_t) res[0]["etc_slots"];
	maxslots[4] = (uint8_t) res[0]["cash_slots"];
	inv.reset(new PlayerInventory(this, maxslots, res[0]["mesos"]));

	// Skills
	skills.reset(new PlayerSkills(this));

	// Storage
	storage.reset(new PlayerStorage(this));

	// Key Maps and Macros
	KeyMaps keyMaps;
	keyMaps.load(this->id);

	SkillMacros skillMacros;
	skillMacros.load(this->id);

	// Character variables
	query << "SELECT * FROM character_variables WHERE charid = " << this->id;
	res = query.store();
	for (size_t i = 0; i < res.size(); i++) {
		variables[(string) res[i]["key"]] = string(res[i]["value"]);
	}

	if (Maps::getMap(map)->getInfo()->forcedReturn != 999999999) {
		map = Maps::getMap(map)->getInfo()->forcedReturn;
		mappos = 0;
		if (hp == 0)
			hp = 50;
	}
	else {
		if (hp == 0) {
			hp = 50;
			map = Maps::getMap(map)->getInfo()->rm;
		}
	}

	m_pos = Maps::getMap(map)->getSpawnPoint(mappos)->pos;
	m_stance = 0;
	m_foothold = 0;

	PlayerPacket::connectData(this);

	if (ChannelServer::Instance()->getScrollingHeader().size() > 0)
		ServerPacket::showScrollingHeader(this, ChannelServer::Instance()->getScrollingHeader());

	for (int8_t i = 0; i < 3; i++) {
		if (Pet *pet = pets->getSummoned(i))
			pet->setPos(Maps::getMap(map)->getSpawnPoint(mappos)->pos);
	}

	PlayerPacket::showKeys(this, &keyMaps);

	BuddyListPacket::update(this, BuddyListPacket::add);

	if (skillMacros.getMax() > -1)
		PlayerPacket::showSkillMacros(this, &skillMacros);

	Maps::newMap(this, map);

	setOnline(true);
	isconnect = true;
	WorldServerConnectPacket::registerPlayer(ChannelServer::Instance()->getWorldPlayer(), id, name, map, job, level);
}

void Player::setHP(int16_t shp, bool is) {
	if (shp < 0)
		hp = 0;
	else if (shp > mhp)
		hp = mhp;
	else
		hp = shp;
	if (is)
		PlayerPacket::updateStatShort(this, 0x400, hp);
	getActiveBuffs()->checkBerserk();
}

void Player::modifyHP(int16_t nhp, bool is) {
	if ((hp + nhp) < 0)
		hp = 0;
	else if ((hp + nhp) > mhp)
		hp = mhp;
	else
		hp = (hp + nhp);
	if (is)
		PlayerPacket::updateStatShort(this, 0x400, hp);
	getActiveBuffs()->checkBerserk();
}

void Player::damageHP(uint16_t dhp) {
	hp = (dhp > hp ? 0 : hp - dhp);
	PlayerPacket::updateStatShort(this, 0x400, hp);
	getActiveBuffs()->checkBerserk();
}

void Player::setMP(int16_t smp, bool is) {
	if (!(getActiveBuffs()->getActiveSkillLevel(2121004) > 0 || getActiveBuffs()->getActiveSkillLevel(2221004) > 0 || getActiveBuffs()->getActiveSkillLevel(2321004) > 0)) {
		if (smp < 0)
			mp = 0;
		else if (smp > mmp)
			mp = mmp;
		else
			mp = smp;
	}
	PlayerPacket::updateStatShort(this, 0x1000, mp, is);
}

void Player::modifyMP(int16_t nmp, bool is) {
	if (!(getActiveBuffs()->getActiveSkillLevel(2121004) > 0 || getActiveBuffs()->getActiveSkillLevel(2221004) > 0 || getActiveBuffs()->getActiveSkillLevel(2321004) > 0)) {
		if ((mp + nmp) < 0)
			mp = 0;
		else if ((mp + nmp) > mmp)
			mp = mmp;
		else
			mp = (mp + nmp);
	}
	PlayerPacket::updateStatShort(this, 0x1000, mp, is);
}

void Player::damageMP(uint16_t dmp) {
	if (!(getActiveBuffs()->getActiveSkillLevel(2121004) > 0 ||	getActiveBuffs()->getActiveSkillLevel(2221004) > 0 || getActiveBuffs()->getActiveSkillLevel(2321004) > 0)) {
		mp = (dmp > mp ? 0 : mp - dmp);
	}
	PlayerPacket::updateStatShort(this, 0x1000, mp, false);
}

void Player::setSp(int16_t sp) {
	this->sp = sp;
	PlayerPacket::updateStatShort(this, 0x8000, sp);
}

void Player::setAp(int16_t ap) {
	this->ap = ap;
	PlayerPacket::updateStatShort(this, 0x4000, ap);
}

void Player::setJob(int16_t job) {
	this->job = job;
	PlayerPacket::updateStatShort(this, 0x20, job);
	LevelsPacket::jobChange(this);
	WorldServerConnectPacket::updateJob(ChannelServer::Instance()->getWorldPlayer(), this->id, job);
}

void Player::setStr(int16_t str) {
	this->str = str;
	PlayerPacket::updateStatShort(this, 0x40, str);
}

void Player::setDex(int16_t dex) {
	this->dex = dex;
	PlayerPacket::updateStatShort(this, 0x80, dex);
}

void Player::setInt(int16_t intt) {
	this->intt = intt;
	PlayerPacket::updateStatShort(this, 0x100, intt);
}

void Player::setLuk(int16_t luk) {
	this->luk = luk;
	PlayerPacket::updateStatShort(this, 0x200, luk);
}

void Player::setMHP(int16_t mhp) {
	if (mhp > 30000)
		mhp = 30000;
	this->mhp = mhp;
	PlayerPacket::updateStatShort(this, 0x800, rmhp);
	getActiveBuffs()->checkBerserk();
}

void Player::setMMP(int16_t mmp) {
	if (mmp > 30000)
		mmp = 30000;
	this->mmp = mmp;
	PlayerPacket::updateStatShort(this, 0x2000, rmmp);
}

void Player::setHyperBody(int16_t modx, int16_t mody) {
	modx += 100;
	mody += 100;
	mhp = ((rmhp * modx / 100) > 30000 ? 30000 : rmhp * modx / 100);
	mmp = ((rmmp * mody / 100) > 30000 ? 30000 : rmmp * mody / 100);
	PlayerPacket::updateStatShort(this, 0x800, rmhp);
	PlayerPacket::updateStatShort(this, 0x2000, rmmp);
	getActiveBuffs()->checkBerserk();
}

void Player::setRMHP(int16_t rmhp) {
	if (rmhp > 30000)
		rmhp = 30000;
	this->rmhp = rmhp;
	PlayerPacket::updateStatShort(this, 0x800, rmhp);
}

void Player::setRMMP(int16_t rmmp) {
	if (rmmp > 30000)
		rmmp = 30000;
	this->rmmp = rmmp;
	PlayerPacket::updateStatShort(this, 0x2000, rmmp);
}

void Player::modifyRMHP(int16_t mod) {
	rmhp = (((rmhp + mod) > 30000) ? 30000 : (rmhp + mod));
	PlayerPacket::updateStatShort(this, 0x800, rmhp);
}

void Player::modifyRMMP(int16_t mod) {
	rmmp = (((rmmp + mod) > 30000) ? 30000 : (rmmp + mod));
	PlayerPacket::updateStatShort(this, 0x2000, rmmp);
}

void Player::setExp(int32_t exp) {
	this->exp = exp;
	PlayerPacket::updateStatInt(this, 0x10000, exp);
}

void Player::setLevel(uint8_t level) {
	this->level = level;
	PlayerPacket::updateStatShort(this, 0x10, level);
	LevelsPacket::levelUp(this);
	WorldServerConnectPacket::updateLevel(ChannelServer::Instance()->getWorldPlayer(), this->id, level);
}

void Player::changeChannel(int8_t channel) {
	ChannelServer::Instance()->getWorldPlayer()->playerChangeChannel(id, channel);
}

void Player::changeKey(PacketReader &packet) {
	packet.skipBytes(4);
	int32_t howmany = packet.getInt();
	if (howmany == 0)
		return;

	KeyMaps keyMaps; // We don't need old values here because it is only used to save the new values
	for (int32_t i = 0; i < howmany; i++) {
		int32_t pos = packet.getInt();
		int8_t type = packet.getByte();
		int32_t action = packet.getInt();
		keyMaps.add(pos, new KeyMaps::KeyMap(type, action));
	}

	// Update to MySQL
	keyMaps.save(this->id);
}

void Player::changeSkillMacros(PacketReader &packet) {
	uint8_t num = packet.getByte();
	if (num == 0)
		return;

	SkillMacros skillMacros;
	for (uint8_t i = 0; i < num; i++) {
		string name = packet.getString();
		bool shout = packet.getByte() != 0;
		int32_t skill1 = packet.getInt();
		int32_t skill2 = packet.getInt();
		int32_t skill3 = packet.getInt();

		skillMacros.add(i, new SkillMacros::SkillMacro(name, shout, skill1, skill2, skill3));
	}
	skillMacros.save(this->id);
}

void Player::setHair(int32_t id) {
	this->hair = id;
	PlayerPacket::updateStatInt(this, 0x04, id);
}

void Player::setEyes(int32_t id) {
	this->eyes = id;
	PlayerPacket::updateStatInt(this, 0x02, id);
}

void Player::setSkin(int8_t id) {
	this->skin = id;
	PlayerPacket::updateStatInt(this, 0x01, id);
}

void Player::setFame(int16_t fame) {
	if (fame < -30000)
		fame = -30000;
	else if (fame > 30000)
		fame = 30000;
	this->fame = fame;
	PlayerPacket::updateStatInt(this, 0x20000, fame);
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
	int32_t t = clock();
	// Deleting old warnings
	for (size_t i = 0; i < warnings.size(); i++) {
		if (warnings[i] + 300000 < t) {
			warnings.erase(warnings.begin() + i);
			i--;
		}
	}
	warnings.push_back(t);
	if (warnings.size() > 50) {
		// Hacker - Temp DCing
		getSession()->disconnect();
		return true;
	}
	return false;
}

void Player::saveStats() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET "
		<< "level = " << (int16_t) this->level << ","
		<< "job = " << this->job << ","
		<< "str = " <<this->str << ","
		<< "dex = " << this->dex << ","
		<< "`int` = " << this->intt << ","
		<< "luk = " << this->luk << ","
		<< "chp = " << this->hp << ","
		<< "mhp = " << this->rmhp << ","
		<< "cmp = " << this->mp << ","
		<< "mmp = " << this->rmmp << ","
		<< "hpmp_ap = " << this->hpmp_ap << ","
		<< "ap = " << this->ap << ","
		<< "sp = " << this->sp << ","
		<< "exp = " << this->exp << ","
		<< "fame = " << this->fame << ","
		<< "map = " << this->map << ","
		<< "gender = " << (int16_t) this->gender << ","
		<< "skin = " << (int16_t) this->skin << ","
		<< "eyes = " << this->eyes << ","
		<< "hair = " << this->hair << ","
		<< "mesos = " << inv->getMesos() << ","
		<< "equip_slots = " << (int16_t) inv->getMaxSlots(1) << ","
		<< "use_slots = " << (int16_t) inv->getMaxSlots(2) << ","
		<< "setup_slots = " << (int16_t) inv->getMaxSlots(3) << ","
		<< "etc_slots = " << (int16_t) inv->getMaxSlots(4) << ","
		<< "cash_slots = " << (int16_t) inv->getMaxSlots(5) << ","
		<< "buddylist_size = " << this->buddylist_size
		<< " WHERE id = " << this->id;
	query.exec();
}

void Player::saveVariables() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "DELETE FROM character_variables WHERE charid = " << this->id;
	query.exec();

	if (variables.size() > 0) {
		bool firstrun = true;
		for (unordered_map<string, string>::iterator iter = variables.begin(); iter != variables.end(); iter++) {
			if (firstrun) {
				query << "INSERT INTO character_variables VALUES (";
				firstrun = false;
			}
			else {
				query << ",(";
			}
			query << this->id << ","
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
	int32_t onlineid = online ? ChannelServer::Instance()->getOnlineId() : 0;
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = " << onlineid <<
			", characters.online = " << online << " WHERE characters.id = " << this->id;
	query.exec();
}

void Player::setLevelDate() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET time_level = NOW() WHERE characters.id = " << this->id;
	query.exec();
}

void Player::acceptDeath() {
	int32_t tomap;
	if (!Maps::getMap(this->map))
		tomap = this->getMap();
	else
		tomap = Maps::getMap(this->getMap())->getInfo()->rm;
	setHP(50, false);
	getActiveBuffs()->removeBuff();
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
