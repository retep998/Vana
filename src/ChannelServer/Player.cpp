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
#include "Player.h"
#include "BuddyListHandler.h"
#include "BuddyListPacket.h"
#include "BuffHolder.h"
#include "ChannelServer.h"
#include "ChatHandler.h"
#include "CommandHandler.h"
#include "Connectable.h"
#include "Database.h"
#include "Drops.h"
#include "Fame.h"
#include "GameConstants.h"
#include "Instance.h"
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
#include "TimeUtilities.h"
#include "TradeHandler.h"
#include "WorldServerConnectPlayer.h"
#include "WorldServerConnectPacket.h"
#include <boost/array.hpp>

Player::Player() :
fall_counter(0),
tradestate(0),
shop(0),
item_effect(0),
chair(0),
party(0),
save_on_dc(true),
is_connect(false),
npc(0),
instance(0)
{
}

Player::~Player() {
	if (is_connect) {
		if (getParty() != 0) {
			getParty()->setMember(getId(), 0);
		}
		if (getInstance() != 0) {
			getInstance()->removePlayer(getId());
			getInstance()->sendMessage(PlayerDisconnect, getId());
		}
		if (getNPC() != 0) {
			delete getNPC();
		}
		//if (this->getHp() == 0)
		//	this->acceptDeath();
		// "Bug" in global, would be fixed here:
		// When disconnecting and dead, you actually go back to forced return map before the death return map
		// (that means that it's parsed while logging in, not while logging out)
		if (save_on_dc) {
			saveAll(true);
			setOnline(false);
		}
		if (isTrading()) {
			TradeHandler::cancelTrade(this);
		}
		WorldServerConnectPacket::removePlayer(ChannelServer::Instance()->getWorldPlayer(), id);
		Maps::getMap(map)->removePlayer(this);
		Players::Instance()->removePlayer(this);
	}
}

void Player::realHandleRequest(PacketReader &packet) {
	switch (packet.get<int16_t>()) {
		case RECV_ADD_SKILL: Skills::addSkill(this, packet); break;
		case RECV_ADD_STAT: Levels::addStat(this, packet); break;
		case RECV_ANIMATE_NPC: NPCs::handleNPCAnimation(this, packet); break;
		case RECV_BUDDYLIST: BuddyListHandler::handleBuddyList(this, packet); break;
		case RECV_CANCEL_ITEM: Inventory::cancelItem(this, packet); break;
		case RECV_CANCEL_SKILL: Skills::cancelSkill(this, packet); break;
		case RECV_CHANGE_CHANNEL: changeChannel(packet.get<int8_t>()); break;
		case RECV_CHANGE_MAP: Maps::usePortal(this, packet); break;
		case RECV_CHANGE_MAP_SPECIAL: Maps::useScriptedPortal(this, packet); break;
		case RECV_CHANNEL_LOGIN: playerConnect(packet); break;
		case RECV_CHAT: ChatHandler::handleChat(this, packet); break;
		case RECV_COMMAND: CommandHandler::handleCommand(this, packet); break;
		case RECV_CONTROL_MOB: Mobs::monsterControl(this, packet); break;
		case RECV_DAMAGE_MOB: Mobs::damageMob(this, packet); break;
		case RECV_DAMAGE_MOB_RANGED: Mobs::damageMobRanged(this, packet); break;
		case RECV_DAMAGE_MOB_SPELL: Mobs::damageMobSpell(this, packet); break;
		case RECV_DAMAGE_MOB_ENERGYCHARGE: Mobs::damageMobEnergyCharge(this, packet); break;
		case RECV_DAMAGE_MOB_SUMMON: Mobs::damageMobSummon(this, packet); break;
		case RECV_DAMAGE_PLAYER: PlayerHandler::handleDamage(this, packet); break;
		case RECV_DAMAGE_SUMMON: Summons::damageSummon(this, packet); break;
		case RECV_DROP_MESO: Drops::dropMesos(this, packet); break;
		case RECV_FACE_EXPRESSION: PlayerHandler::handleFacialExpression(this, packet); break;
		case RECV_FAME: Fame::handleFame(this, packet); break;
		case RECV_GET_PLAYER_INFO: PlayerHandler::handleGetInfo(this, packet); break;
		case RECV_GET_QUEST: Quests::getQuest(this, packet); break;
		case RECV_GROUP_CHAT: ChatHandler::handleGroupChat(this, packet); break;
		case RECV_HEAL_PLAYER: PlayerHandler::handleHeal(this, packet); break;
		case RECV_HIT_REACTOR: Reactors::hitReactor(this, packet); break;
		case RECV_KEYMAP: changeKey(packet); break;
		case RECV_LOOT_ITEM: Drops::playerLoot(this, packet); break;
		case RECV_MOB_BOMB_EXPLOSION: Mobs::handleBomb(this, packet); break;
		case RECV_MOVE_ITEM: Inventory::itemMove(this, packet); break;
		case RECV_MOVE_PLAYER: PlayerHandler::handleMoving(this, packet); break;
		case RECV_MOVE_SUMMON: Summons::moveSummon(this, packet); break;
		case RECV_NPC_TALK: NPCs::handleNPC(this, packet); break;
		case RECV_NPC_TALK_CONT: NPCs::handleNPCIn(this, packet); break;
		case RECV_PARTY_ACTION: PartyFunctions::handleRequest(this, packet); break;
		case RECV_PET_CHAT: Pets::handleChat(this, packet); break;
		case RECV_PET_COMMAND: Pets::handleCommand(this, packet); break;
		case RECV_PET_FEED: Pets::handleFeed(this, packet); break;
		case RECV_PET_LOOT: Drops::petLoot(this, packet); break;
		case RECV_PET_MOVE: Pets::handleMovement(this, packet); break;
		case RECV_PET_SUMMON: Pets::handleSummon(this, packet); break;
		case RECV_PLAYER_ROOM_ACTION: TradeHandler::tradeHandler(this, packet); break;
		case RECV_SHOP_ENTER: Inventory::useShop(this, packet); break;
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
		case RECV_USE_STORAGE: Inventory::useStorage(this, packet); break;
		case RECV_USE_SUMMON_BAG: Inventory::useSummonBag(this, packet); break;
	}
}

void Player::playerConnect(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	if (!Connectable::Instance()->checkPlayer(id)) {
		// Hacking
		getSession()->disconnect();
		return;
	}
	this->id = id;

	// Character info
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT characters.*, users.gm FROM characters LEFT JOIN users on characters.userid = users.id WHERE characters.id = " << id;
	mysqlpp::StoreQueryResult res = query.store();

	if (res.empty()) {
		// Hacking
		getSession()->disconnect();
		return;
	}

	res[0]["name"].to_string(name);
	user_id		= res[0]["userid"];
	exp			= res[0]["exp"];
	map			= res[0]["map"];
	gm_level			= res[0]["gm"];
	eyes		= res[0]["eyes"];
	hair		= res[0]["hair"];
	world_id	= static_cast<int8_t>(res[0]["world_id"]);
	gender		= static_cast<int8_t>(res[0]["gender"]);
	skin		= static_cast<int8_t>(res[0]["skin"]);
	map_pos		= static_cast<int8_t>(res[0]["pos"]);
	level		= static_cast<uint8_t>(res[0]["level"]);
	job			= static_cast<int16_t>(res[0]["job"]);
	str			= static_cast<int16_t>(res[0]["str"]);
	dex			= static_cast<int16_t>(res[0]["dex"]);
	intt		= static_cast<int16_t>(res[0]["int"]);
	luk			= static_cast<int16_t>(res[0]["luk"]);
	hp			= static_cast<int16_t>(res[0]["chp"]);
	rmhp = mhp	= static_cast<int16_t>(res[0]["mhp"]);
	mp			= static_cast<int16_t>(res[0]["cmp"]);
	rmmp = mmp	= static_cast<int16_t>(res[0]["mmp"]);
	ap			= static_cast<int16_t>(res[0]["ap"]);
	sp			= static_cast<int16_t>(res[0]["sp"]);
	fame		= static_cast<int16_t>(res[0]["fame"]);
	hpmp_ap		= static_cast<uint16_t>(res[0]["hpmp_ap"]);
	buddylist_size = static_cast<uint8_t>(res[0]["buddylist_size"]);

	// Inventory
	pets.reset(new PlayerPets(this));
	boost::array<uint8_t, 5> maxslots;
	maxslots[0] = static_cast<uint8_t>(res[0]["equip_slots"]);
	maxslots[1] = static_cast<uint8_t>(res[0]["use_slots"]);
	maxslots[2] = static_cast<uint8_t>(res[0]["setup_slots"]);
	maxslots[3] = static_cast<uint8_t>(res[0]["etc_slots"]);
	maxslots[4] = static_cast<uint8_t>(res[0]["cash_slots"]);
	inv.reset(new PlayerInventory(this, maxslots, res[0]["mesos"]));
	storage.reset(new PlayerStorage(this));

	// Buffs
	activeBuffs.reset(new PlayerActiveBuffs(this));
	if (BuffHolder::Instance()->checkPlayer(id)) {
		PlayerActiveBuffs *mybuffs = getActiveBuffs();
		PlayerActiveBuffs *existingbuffs = BuffHolder::Instance()->getBuffs(id);
		vector<BuffStorage> buffstoadd = BuffHolder::Instance()->getStoredBuffs(id);
		for (size_t i = 0; i < buffstoadd.size(); i++) {
			BuffStorage cbuff = buffstoadd[i];
			mybuffs->addBuff(cbuff.skillid, cbuff.timeleft);
			mybuffs->setActiveSkillLevel(cbuff.skillid, cbuff.level);
			Buffs::Instance()->doAct(this, cbuff.skillid, cbuff.level);
		}
		mybuffs->setEnergyChargeLevel(existingbuffs->getEnergyChargeLevel(), true);
		mybuffs->setBooster(existingbuffs->getBooster());
		mybuffs->setCharge(existingbuffs->getCharge());
		mybuffs->setCombo(existingbuffs->getCombo(), false);
		mybuffs->setDebuffMask(existingbuffs->getDebuffMask());

		MapEntryBuffs entr = existingbuffs->getMapEntryBuffs();
		mybuffs->setMountInfo(entr.mountskill, entr.mountid);
		mybuffs->setMapEntryBuffs(entr);
		
		ActiveBuffsByType bufftypes = existingbuffs->getBuffTypes();
		mybuffs->setActiveBuffsByType(bufftypes);
		if (mybuffs->hasHyperBody()) {
			int32_t skillid = mybuffs->getHyperBody();
			uint8_t hblevel = mybuffs->getActiveSkillLevel(skillid);
			setHyperBody(Skills::skills[skillid][hblevel].x, Skills::skills[skillid][hblevel].y);
		}
		BuffHolder::Instance()->removeBuffs(id);
	}

	// Skills
	skills.reset(new PlayerSkills(this));

	// Player variables
	variables.reset(new PlayerVariables(this));

	// The rest
	summons.reset(new PlayerSummons(this));
	buddyList.reset(new PlayerBuddyList(this));
	quests.reset(new PlayerQuests(this));

	// Key Maps and Macros
	KeyMaps keyMaps;
	keyMaps.load(id);

	SkillMacros skillMacros;
	skillMacros.load(id);

	if (Maps::getMap(map)->getInfo()->forcedReturn != 999999999) {
		map = Maps::getMap(map)->getInfo()->forcedReturn;
		map_pos = 0;
		if (hp == 0)
			hp = 50;
	}
	else {
		if (hp == 0) {
			hp = 50;
			map = Maps::getMap(map)->getInfo()->rm;
		}
	}

	if (hp > mhp)
		hp = mhp;
	if (mp > mmp)
		mp = mmp;

	m_pos = Maps::getMap(map)->getSpawnPoint(map_pos)->pos;
	m_stance = 0;
	m_foothold = 0;

	PlayerPacket::connectData(this);

	if (ChannelServer::Instance()->getScrollingHeader().size() > 0)
		ServerPacket::showScrollingHeader(this, ChannelServer::Instance()->getScrollingHeader());

	for (int8_t i = 0; i < 3; i++) {
		if (Pet *pet = pets->getSummoned(i))
			pet->setPos(Maps::getMap(map)->getSpawnPoint(map_pos)->pos);
	}

	PlayerPacket::showKeys(this, &keyMaps);

	BuddyListPacket::update(this, BuddyListPacket::add);

	if (skillMacros.getMax() > -1)
		PlayerPacket::showSkillMacros(this, &skillMacros);

	Maps::newMap(this, map);

	setOnline(true);
	is_connect = true;
	WorldServerConnectPacket::registerPlayer(ChannelServer::Instance()->getWorldPlayer(), ip, id, name, map, job, level);
}

void Player::setHp(int16_t shp, bool is) {
	if (shp < 0)
		hp = 0;
	else if (shp > mhp)
		hp = mhp;
	else
		hp = shp;
	if (is)
		PlayerPacket::updateStatShort(this, 0x400, hp);
	if (getParty())
		getParty()->showHpBar(this);
	getActiveBuffs()->checkBerserk();
	if (hp == 0 && getInstance() != 0) {
		getInstance()->sendMessage(PlayerDeath, getId());
	}
	if (hp == 0)
		loseExp();
}

void Player::modifyHp(int16_t nhp, bool is) {
	if ((hp + nhp) < 0)
		hp = 0;
	else if ((hp + nhp) > mhp)
		hp = mhp;
	else
		hp = (hp + nhp);
	if (is)
		PlayerPacket::updateStatShort(this, 0x400, hp);
	if (getParty())
		getParty()->showHpBar(this);
	getActiveBuffs()->checkBerserk();
	if (hp == 0 && getInstance() != 0) {
		getInstance()->sendMessage(PlayerDeath, getId());
	}
	if (hp == 0)
		loseExp();
}

void Player::damageHp(uint16_t dhp) {
	hp = (dhp > hp ? 0 : hp - dhp);
	PlayerPacket::updateStatShort(this, 0x400, hp);
	if (getParty())
		getParty()->showHpBar(this);
	getActiveBuffs()->checkBerserk();
	if (hp == 0 && getInstance() != 0) {
		getInstance()->sendMessage(PlayerDeath, getId());
	}
	if (hp == 0)
		loseExp();
}

void Player::setMp(int16_t smp, bool is) {
	if (!getActiveBuffs()->hasInfinity()) {
		if (smp < 0)
			mp = 0;
		else if (smp > mmp)
			mp = mmp;
		else
			mp = smp;
	}
	PlayerPacket::updateStatShort(this, 0x1000, mp, is);
}

void Player::modifyMp(int16_t nmp, bool is) {
	if (!getActiveBuffs()->hasInfinity()) {
		if ((mp + nmp) < 0)
			mp = 0;
		else if ((mp + nmp) > mmp)
			mp = mmp;
		else
			mp = (mp + nmp);
	}
	PlayerPacket::updateStatShort(this, 0x1000, mp, is);
}

void Player::damageMp(uint16_t dmp) {
	if (!getActiveBuffs()->hasInfinity()) {
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
	WorldServerConnectPacket::updateJob(ChannelServer::Instance()->getWorldPlayer(), id, job);
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

void Player::setMHp(int16_t mhp) {
	if (mhp > 30000)
		mhp = 30000;
	else if (mhp < 1)
		mhp = 1;
	this->mhp = mhp;
	PlayerPacket::updateStatShort(this, 0x800, rmhp);
	if (getParty())
		getParty()->showHpBar(this);
	getActiveBuffs()->checkBerserk();
}

void Player::setMMp(int16_t mmp) {
	if (mmp > 30000)
		mmp = 30000;
	else if (mmp < 1)
		mmp = 1;
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
	if (getParty())
		getParty()->showHpBar(this);
	getActiveBuffs()->checkBerserk();
}

void Player::setRMHp(int16_t rmhp) {
	if (rmhp > 30000)
		rmhp = 30000;
	else if (rmhp < 1)
		rmhp = 1;
	this->rmhp = rmhp;
	PlayerPacket::updateStatShort(this, 0x800, rmhp);
}

void Player::setRMMp(int16_t rmmp) {
	if (rmmp > 30000)
		rmmp = 30000;
	else if (rmmp < 1)
		rmmp = 1;
	this->rmmp = rmmp;
	PlayerPacket::updateStatShort(this, 0x2000, rmmp);
}

void Player::modifyRMHp(int16_t mod) {
	rmhp = (((rmhp + mod) > 30000) ? 30000 : (rmhp + mod));
	PlayerPacket::updateStatShort(this, 0x800, rmhp);
}

void Player::modifyRMMp(int16_t mod) {
	rmmp = (((rmmp + mod) > 30000) ? 30000 : (rmmp + mod));
	PlayerPacket::updateStatShort(this, 0x2000, rmmp);
}

void Player::setExp(int32_t exp) {
	if (this->exp < 0)
		exp = 0;
	this->exp = exp;
	PlayerPacket::updateStatInt(this, 0x10000, exp);
}

void Player::setLevel(uint8_t level) {
	this->level = level;
	PlayerPacket::updateStatShort(this, 0x10, level);
	LevelsPacket::levelUp(this);
	WorldServerConnectPacket::updateLevel(ChannelServer::Instance()->getWorldPlayer(), id, level);
}

void Player::changeChannel(int8_t channel) {
	ChannelServer::Instance()->getWorldPlayer()->playerChangeChannel(id, channel, getActiveBuffs());
}

void Player::changeKey(PacketReader &packet) {
	packet.skipBytes(4);
	int32_t howmany = packet.get<int32_t>();
	if (howmany == 0)
		return;

	KeyMaps keyMaps; // We don't need old values here because it is only used to save the new values
	for (int32_t i = 0; i < howmany; i++) {
		int32_t pos = packet.get<int32_t>();
		int8_t type = packet.get<int8_t>();
		int32_t action = packet.get<int32_t>();
		keyMaps.add(pos, new KeyMaps::KeyMap(type, action));
	}

	// Update to MySQL
	keyMaps.save(this->id);
}

void Player::changeSkillMacros(PacketReader &packet) {
	uint8_t num = packet.get<int8_t>();
	if (num == 0)
		return;

	SkillMacros skillMacros;
	for (uint8_t i = 0; i < num; i++) {
		string name = packet.getString();
		bool shout = packet.get<int8_t>() != 0;
		int32_t skill1 = packet.get<int32_t>();
		int32_t skill2 = packet.get<int32_t>();
		int32_t skill3 = packet.get<int32_t>();

		skillMacros.add(i, new SkillMacros::SkillMacro(name, shout, skill1, skill2, skill3));
	}
	skillMacros.save(id);
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

bool Player::addWarning() {
	int32_t t = TimeUtilities::clock_in_ms();
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
		<< "level = " << static_cast<int16_t>(level) << "," // Queries have problems with int8_t due to being derived from ostream
		<< "job = " << job << ","
		<< "str = " << str << ","
		<< "dex = " << dex << ","
		<< "`int` = " << intt << ","
		<< "luk = " << luk << ","
		<< "chp = " << hp << ","
		<< "mhp = " << rmhp << ","
		<< "cmp = " << mp << ","
		<< "mmp = " << rmmp << ","
		<< "hpmp_ap = " << hpmp_ap << ","
		<< "ap = " << ap << ","
		<< "sp = " << sp << ","
		<< "exp = " << exp << ","
		<< "fame = " << fame << ","
		<< "map = " << map << ","
		<< "gender = " << static_cast<int16_t>(gender) << ","
		<< "skin = " << static_cast<int16_t>(skin) << ","
		<< "eyes = " << eyes << ","
		<< "hair = " << hair << ","
		<< "mesos = " << inv->getMesos() << ","
		<< "equip_slots = " << static_cast<int16_t>(inv->getMaxSlots(1)) << ","
		<< "use_slots = " << static_cast<int16_t>(inv->getMaxSlots(2)) << ","
		<< "setup_slots = " << static_cast<int16_t>(inv->getMaxSlots(3)) << ","
		<< "etc_slots = " << static_cast<int16_t>(inv->getMaxSlots(4)) << ","
		<< "cash_slots = " << static_cast<int16_t>(inv->getMaxSlots(5)) << ","
		<< "buddylist_size = " << static_cast<int16_t>(buddylist_size)
		<< " WHERE id = " << id;
	query.exec();
}

void Player::saveAll(bool savecooldowns) {
	saveStats();
	getInventory()->save();
	getPets()->save();
	getSkills()->save(savecooldowns);
	getStorage()->save();
	getVariables()->save();
}

void Player::setOnline(bool online) {
	int32_t onlineid = online ? ChannelServer::Instance()->getOnlineId() : 0;
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = " << onlineid <<
			", characters.online = " << online << " WHERE characters.id = " << id;
	query.exec();
}

void Player::setLevelDate() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET time_level = NOW() WHERE characters.id = " << id;
	query.exec();
}

void Player::acceptDeath() {
	int32_t tomap = (Maps::getMap(map) ? Maps::getMap(map)->getInfo()->rm : map);
	setHp(50, false);
	getActiveBuffs()->removeBuff();
	Maps::changeMap(this, tomap);
}

bool Player::hasGmEquip() {
	if (getInventory()->getEquippedId(EquipSlots::Helm) == GmSuit::Hat)
		return true;
	if (getInventory()->getEquippedId(EquipSlots::Top) == GmSuit::Top)
		return true;
	if (getInventory()->getEquippedId(EquipSlots::Bottom) == GmSuit::Bottom)
		return true;
	if (getInventory()->getEquippedId(EquipSlots::Weapon) == GmSuit::Weapon)
		return true;
	return false;
}

void Player::setBuddyListSize(uint8_t size) {
	buddylist_size = size;
	BuddyListPacket::showSize(this);
}

void Player::loseExp() {
	if (getJob() != 0 && getLevel() < 200) {
		Map *loc = Maps::getMap(getMap());
		int32_t fieldlimit = loc->getInfo()->fieldLimit;
		int8_t exploss = 10;
		if (fieldlimit & 0x20 || loc->getInfo()->town)
			exploss = 1;
		else {
			switch (getJob() / 100) {
				case 2: // Magicians
					exploss = 7;
					break;
				case 4: // Thieves
					exploss = 5;
					break;
			}
		}
		int64_t exp = getExp();
		exp -= Levels::exps[getLevel() - 1] * exploss / 100;
		setExp(static_cast<int32_t>(exp));
	}
}
