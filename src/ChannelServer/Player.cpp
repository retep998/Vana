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
#include "ChannelServer.h"
#include "ChatHandler.h"
#include "CommandHandler.h"
#include "Connectable.h"
#include "Database.h"
#include "DropHandler.h"
#include "Fame.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Instance.h"
#include "Inventory.h"
#include "InventoryHandler.h"
#include "InventoryPacket.h"
#include "KeyMaps.h"
#include "Levels.h"
#include "LevelsPacket.h"
#include "MapleSession.h"
#include "MapPacket.h"
#include "Maps.h"
#include "MobHandler.h"
#include "MonsterBookPacket.h"
#include "NPC.h"
#include "NPCHandler.h"
#include "PacketReader.h"
#include "Party.h"
#include "Pets.h"
#include "PlayerHandler.h"
#include "PlayerPacket.h"
#include "PlayerPacketHolder.h"
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
#include "WorldServerConnection.h"
#include "WorldServerConnectPacket.h"
#include <boost/array.hpp>

Player::Player() :
fall_counter(0),
shop(0),
item_effect(0),
chair(0),
mapchair(0),
trade_id(0),
trade_state(false),
save_on_dc(true),
is_connect(false),
npc(0),
party(0),
instance(0)
{
}

Player::~Player() {
	if (is_connect) {
		if (isTrading()) {
			TradeHandler::cancelTrade(this);
		}
		int32_t isleader = 0;
		if (getParty() != 0) {
			getParty()->setMember(getId(), 0);
			isleader = getParty()->isLeader(getId()) ? 1 : 0;
		}
		if (getInstance() != 0) {
			getInstance()->removePlayer(getId());
			getInstance()->sendMessage(PlayerDisconnect, getId(), isleader);
		}
		if (getNPC() != 0) {
			delete getNPC();
		}
		if (getMapChair() != 0) {
			Maps::getMap(getMap())->playerSeated(getMapChair(), 0);
		}
		//if (this->getHp() == 0)
		//	this->acceptDeath();
		// "Bug" in global, would be fixed here:
		// When disconnecting and dead, you actually go back to forced return map before the death return map
		// (that means that it's parsed while logging in, not while logging out)
		PortalInfo *closest = Maps::getMap(getMap())->getNearestSpawnPoint(getPos());
		if (closest != 0) {
			map_pos = closest->id;
		}
		if (save_on_dc) {
			saveAll(true);
			setOnline(false);
		}
		if (ChannelServer::Instance()->isConnected()) { // Do not connect to worldserver if the worldserver has disconnected
			WorldServerConnectPacket::removePlayer(ChannelServer::Instance()->getWorldConnection(), id);	
		}
		Maps::getMap(map)->removePlayer(this);
		Players::Instance()->removePlayer(this);
	}
}

void Player::realHandleRequest(PacketReader &packet) {
	switch (packet.get<int16_t>()) {
		case CMSG_ATTACK_ENERGY_CHARGE: PlayerHandler::useEnergyChargeAttack(this, packet); break;
		case CMSG_ATTACK_MAGIC: PlayerHandler::useSpellAttack(this, packet); break;
		case CMSG_ATTACK_MELEE: PlayerHandler::useMeleeAttack(this, packet); break;
		case CMSG_ATTACK_RANGED: PlayerHandler::useRangedAttack(this, packet); break;
		case CMSG_BUDDY: BuddyListHandler::handleBuddyList(this, packet); break;
		case CMSG_CASH_ITEM_USE: InventoryHandler::useCashItem(this, packet); break;
		case CMSG_CASH_SHOP: PlayerPacket::sendBlockedMessage(this, 0x02); break;
		case CMSG_CHAIR: InventoryHandler::handleChair(this, packet); break;
		case CMSG_CHANNEL_CHANGE: changeChannel(packet.get<int8_t>()); break;
		case CMSG_COMMAND: CommandHandler::handleCommand(this, packet); break;
		case CMSG_DROP_MESOS: DropHandler::dropMesos(this, packet); break;
		case CMSG_EMOTE: PlayerHandler::handleFacialExpression(this, packet); break;
		case CMSG_FAME: Fame::handleFame(this, packet); break;
		case CMSG_FRIENDLY_MOB_DAMAGE: MobHandler::friendlyDamaged(this, packet); break;
		case CMSG_HAMMER: InventoryHandler::handleHammerTime(this); break;
		case CMSG_ITEM_CANCEL: InventoryHandler::cancelItem(this, packet); break;
		case CMSG_ITEM_EFFECT: InventoryHandler::useItemEffect(this, packet); break;
		case CMSG_ITEM_LOOT: DropHandler::playerLoot(this, packet); break;
		case CMSG_ITEM_MOVE: InventoryHandler::itemMove(this, packet); break;
		case CMSG_ITEM_USE: InventoryHandler::useItem(this, packet); break;
		case CMSG_KEYMAP: changeKey(packet); break;
		case CMSG_MACRO_LIST: changeSkillMacros(packet); break;
		case CMSG_MAP_CHANGE: Maps::usePortal(this, packet); break;
		case CMSG_MAP_CHANGE_SPECIAL: Maps::useScriptedPortal(this, packet); break;
		case CMSG_MESSAGE_GROUP: ChatHandler::handleGroupChat(this, packet); break;
		case CMSG_MOB_CONTROL: MobHandler::monsterControl(this, packet); break;
		case CMSG_MOB_EXPLOSION: MobHandler::handleBomb(this, packet); break;
		case CMSG_MOB_TURNCOAT_DAMAGE: MobHandler::handleTurncoats(this, packet); break;
		case CMSG_MONSTER_BOOK: PlayerHandler::handleMonsterBook(this, packet); break;
		case CMSG_MTS: PlayerPacket::sendBlockedMessage(this, 0x03); break;
		case CMSG_MULTI_STAT_ADDITION: Levels::addStatMulti(this, packet); break;
		case CMSG_NPC_ANIMATE: NpcHandler::handleNpcAnimation(this, packet); break;
		case CMSG_NPC_TALK: NpcHandler::handleNpc(this, packet); break;
		case CMSG_NPC_TALK_CONT: NpcHandler::handleNpcIn(this, packet); break;
		case CMSG_PARTY: PartyFunctions::handleRequest(this, packet); break;
		case CMSG_PET_CHAT: Pets::handleChat(this, packet); break;
		case CMSG_PET_COMMAND: Pets::handleCommand(this, packet); break;
		case CMSG_PET_FOOD_USE: Pets::handleFeed(this, packet); break;
		case CMSG_PET_LOOT: DropHandler::petLoot(this, packet); break;
		case CMSG_PET_MOVEMENT: Pets::handleMovement(this, packet); break;
		case CMSG_PET_SUMMON: Pets::handleSummon(this, packet); break;
		case CMSG_PLAYER_CHAT: ChatHandler::handleChat(this, packet); break;
		case CMSG_PLAYER_DAMAGE: PlayerHandler::handleDamage(this, packet); break;
		case CMSG_PLAYER_HEAL: PlayerHandler::handleHeal(this, packet); break;
		case CMSG_PLAYER_INFO: PlayerHandler::handleGetInfo(this, packet); break;
		case CMSG_PLAYER_LOAD: playerConnect(packet); break;
		case CMSG_PLAYER_MOVE: PlayerHandler::handleMoving(this, packet); break;
		case CMSG_PLAYER_ROOM: TradeHandler::tradeHandler(this, packet); break;
		case CMSG_QUEST_OBTAIN: Quests::getQuest(this, packet); break;
		case CMSG_REACTOR_HIT: Reactors::hitReactor(this, packet); break;
		case CMSG_REACTOR_TOUCH: Reactors::touchReactor(this, packet); break;
		case CMSG_SCROLL_USE: InventoryHandler::useScroll(this, packet); break;
		case CMSG_SHOP: InventoryHandler::useShop(this, packet); break;
		case CMSG_SKILL_ADD: Skills::addSkill(this, packet); break;
		case CMSG_SKILL_CANCEL: Skills::cancelSkill(this, packet); break;
		case CMSG_SKILL_USE: Skills::useSkill(this, packet); break;
		case CMSG_SKILLBOOK_USE: InventoryHandler::useSkillbook(this, packet); break;
		case CMSG_SPECIAL_SKILL: PlayerHandler::handleSpecialSkills(this, packet); break;
		case CMSG_STAT_ADDITION: Levels::addStat(this, packet); break;
		case CMSG_STORAGE: InventoryHandler::useStorage(this, packet); break;
		case CMSG_SUMMON_ATTACK: PlayerHandler::useSummonAttack(this, packet); break;
		case CMSG_SUMMON_BAG_USE: InventoryHandler::useSummonBag(this, packet); break;
		case CMSG_SUMMON_DAMAGE: Summons::damageSummon(this, packet); break;
		case CMSG_SUMMON_MOVEMENT: Summons::moveSummon(this, packet); break;
		case CMSG_TELEPORT_ROCK: InventoryHandler::handleRockFunctions(this, packet); break;
		case CMSG_TELEPORT_ROCK_USE: packet.skipBytes(5); InventoryHandler::handleRockTeleport(this, packet.get<int8_t>(), Items::SpecialTeleportRock, packet); break;
		case CMSG_TOWN_SCROLL_USE: InventoryHandler::useReturnScroll(this, packet); break;
		case CMSG_USE_CHAIR: InventoryHandler::useChair(this, packet); break;
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
	exp		= res[0]["exp"];
	map		= res[0]["map"];
	gm_level	= res[0]["gm"];
	eyes		= res[0]["eyes"];
	hair		= res[0]["hair"];
	world_id	= static_cast<int8_t>(res[0]["world_id"]);
	gender		= static_cast<int8_t>(res[0]["gender"]);
	skin		= static_cast<int8_t>(res[0]["skin"]);
	map_pos		= static_cast<int8_t>(res[0]["pos"]);
	level		= static_cast<uint8_t>(res[0]["level"]);
	job		= static_cast<int16_t>(res[0]["job"]);
	str		= static_cast<int16_t>(res[0]["str"]);
	dex		= static_cast<int16_t>(res[0]["dex"]);
	intt		= static_cast<int16_t>(res[0]["int"]);
	luk		= static_cast<int16_t>(res[0]["luk"]);
	hp		= static_cast<int16_t>(res[0]["chp"]);
	rmhp = mhp	= static_cast<int16_t>(res[0]["mhp"]);
	mp		= static_cast<int16_t>(res[0]["cmp"]);
	rmmp = mmp	= static_cast<int16_t>(res[0]["mmp"]);
	ap		= static_cast<int16_t>(res[0]["ap"]);
	sp		= static_cast<int16_t>(res[0]["sp"]);
	fame		= static_cast<int16_t>(res[0]["fame"]);
	hpmp_ap		= static_cast<uint16_t>(res[0]["hpmp_ap"]);
	buddylist_size = static_cast<uint8_t>(res[0]["buddylist_size"]);

	if (Maps::getMap(map)->getInfo()->forcedReturn != Maps::NoMap) {
		map = Maps::getMap(map)->getInfo()->forcedReturn;
		map_pos = 0;
		if (hp == 0)
			hp = 50;
	}
	else if (hp == 0) {
		hp = 50;
		map = Maps::getMap(map)->getInfo()->rm;
	}

	m_pos = Maps::getMap(map)->getSpawnPoint(map_pos)->pos;
	m_stance = 0;
	m_foothold = 0;

	// Inventory
	mounts.reset(new PlayerMounts(this));
	pets.reset(new PlayerPets(this));
	boost::array<uint8_t, Inventories::InventoryCount> maxslots;
	maxslots[0] = static_cast<uint8_t>(res[0]["equip_slots"]);
	maxslots[1] = static_cast<uint8_t>(res[0]["use_slots"]);
	maxslots[2] = static_cast<uint8_t>(res[0]["setup_slots"]);
	maxslots[3] = static_cast<uint8_t>(res[0]["etc_slots"]);
	maxslots[4] = static_cast<uint8_t>(res[0]["cash_slots"]);
	inv.reset(new PlayerInventory(this, maxslots, res[0]["mesos"]));
	storage.reset(new PlayerStorage(this));

	// Skills
	skills.reset(new PlayerSkills(this));

	// Buffs/summons
	activeBuffs.reset(new PlayerActiveBuffs(this));
	summons.reset(new PlayerSummons(this));

	// Packet transferring on channel switch
	if (PlayerPacketHolder::Instance()->checkPlayer(id)) {
		PacketReader pack = PlayerPacketHolder::Instance()->getPacket(id);

		setConnectionTime(pack.get<int64_t>());

		getActiveBuffs()->parseBuffTransferPacket(pack);
		if (getActiveBuffs()->hasHyperBody()) {
			int32_t skillid = getActiveBuffs()->getHyperBody();
			uint8_t hblevel = getActiveBuffs()->getActiveSkillLevel(skillid);
			SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillid, hblevel);
			setHyperBody(hb->x, hb->y);
		}

		getSummons()->parseSummonTransferPacket(pack);

		PlayerPacketHolder::Instance()->removePacket(id);
	}
	else {
		// No packet, that means that they're connecting for the first time
		setConnectionTime(time(0));
	}

	// The rest
	variables.reset(new PlayerVariables(this));
	buddyList.reset(new PlayerBuddyList(this));
	quests.reset(new PlayerQuests(this));
	monsterBook.reset(new PlayerMonsterBook(this));

	getMonsterBook()->setCover(res[0]["monsterbookcover"]);

	// Key Maps and Macros
	KeyMaps keyMaps;
	keyMaps.load(id);

	SkillMacros skillMacros;
	skillMacros.load(id);

	if (hp > mhp)
		hp = mhp;
	if (mp > mmp)
		mp = mmp;

	PlayerPacket::connectData(this);

	if (ChannelServer::Instance()->getScrollingHeader().size() > 0)
		ServerPacket::showScrollingHeader(this, ChannelServer::Instance()->getScrollingHeader());

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = pets->getSummoned(i))
			pet->setPos(Maps::getMap(map)->getSpawnPoint(map_pos)->pos);
	}

	PlayerPacket::showKeys(this, &keyMaps);

	BuddyListPacket::update(this, BuddyListPacket::add);

	PlayerPacket::showSkillMacros(this, &skillMacros);

	Maps::addPlayer(this, map);

	setOnline(true);
	is_connect = true;
	WorldServerConnectPacket::registerPlayer(ChannelServer::Instance()->getWorldConnection(), getIp(), id, name, map, job, level);
}

void Player::setHp(int16_t shp, bool is) {
	if (shp < 0)
		hp = 0;
	else if (shp > mhp)
		hp = mhp;
	else
		hp = shp;
	if (is)
		PlayerPacket::updateStatShort(this, Stats::Hp, hp);
	modifiedHp();
}

void Player::modifyHp(int16_t nhp, bool is) {
	if ((hp + nhp) < 0)
		hp = 0;
	else if ((hp + nhp) > mhp)
		hp = mhp;
	else
		hp = (hp + nhp);
	if (is)
		PlayerPacket::updateStatShort(this, Stats::Hp, hp);
	modifiedHp();
}

void Player::damageHp(uint16_t dhp) {
	hp = (dhp > hp ? 0 : hp - dhp);
	PlayerPacket::updateStatShort(this, Stats::Hp, hp);
	modifiedHp();
}

void Player::modifiedHp() {
	if (getParty())
		getParty()->showHpBar(this);
	getActiveBuffs()->checkBerserk();
	if (hp == 0) {
		if (getInstance() != 0) {
			getInstance()->sendMessage(PlayerDeath, getId());
		}
		loseExp();
		Summons::removeSummon(this, false, true, false, 2);
	}
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
	PlayerPacket::updateStatShort(this, Stats::Mp, mp, is);
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
	PlayerPacket::updateStatShort(this, Stats::Mp, mp, is);
}

void Player::damageMp(uint16_t dmp) {
	if (!getActiveBuffs()->hasInfinity()) {
		mp = (dmp > mp ? 0 : mp - dmp);
	}
	PlayerPacket::updateStatShort(this, Stats::Mp, mp, false);
}

void Player::setSp(int16_t sp) {
	this->sp = sp;
	PlayerPacket::updateStatShort(this, Stats::Sp, sp);
}

void Player::setAp(int16_t ap) {
	this->ap = ap;
	PlayerPacket::updateStatShort(this, Stats::Ap, ap);
}

void Player::setJob(int16_t job) {
	this->job = job;
	PlayerPacket::updateStatShort(this, Stats::Job, job);
	LevelsPacket::jobChange(this);
	WorldServerConnectPacket::updateJob(ChannelServer::Instance()->getWorldConnection(), id, job);
}

void Player::setStr(int16_t str) {
	this->str = str;
	PlayerPacket::updateStatShort(this, Stats::Str, str);
}

void Player::setDex(int16_t dex) {
	this->dex = dex;
	PlayerPacket::updateStatShort(this, Stats::Dex, dex);
}

void Player::setInt(int16_t intt) {
	this->intt = intt;
	PlayerPacket::updateStatShort(this, Stats::Int, intt);
}

void Player::setLuk(int16_t luk) {
	this->luk = luk;
	PlayerPacket::updateStatShort(this, Stats::Luk, luk);
}

void Player::setMHp(int16_t mhp) {
	if (mhp > Stats::MaxMaxHp)
		mhp = Stats::MaxMaxHp;
	else if (mhp < Stats::MinMaxHp)
		mhp = Stats::MinMaxHp;
	this->mhp = mhp;
	PlayerPacket::updateStatShort(this, Stats::MaxHp, rmhp);
	modifiedHp();
}

void Player::setMMp(int16_t mmp) {
	if (mmp > Stats::MaxMaxMp)
		mmp = Stats::MaxMaxMp;
	else if (mmp < Stats::MinMaxMp)
		mmp = Stats::MinMaxMp;
	this->mmp = mmp;
	PlayerPacket::updateStatShort(this, Stats::MaxMp, rmmp);
}

void Player::setHyperBody(int16_t modx, int16_t mody) {
	modx += 100;
	mody += 100;
	mhp = ((rmhp * modx / 100) > Stats::MaxMaxHp ? Stats::MaxMaxHp : rmhp * modx / 100);
	mmp = ((rmmp * mody / 100) > Stats::MaxMaxMp ? Stats::MaxMaxMp : rmmp * mody / 100);
	PlayerPacket::updateStatShort(this, Stats::MaxHp, rmhp);
	PlayerPacket::updateStatShort(this, Stats::MaxMp, rmmp);
	if (getParty())
		getParty()->showHpBar(this);
	getActiveBuffs()->checkBerserk();
}

void Player::setRMHp(int16_t rmhp) {
	if (rmhp > Stats::MaxMaxHp)
		rmhp = Stats::MaxMaxHp;
	else if (rmhp < Stats::MinMaxHp)
		rmhp = Stats::MinMaxHp;
	this->rmhp = rmhp;
	PlayerPacket::updateStatShort(this, Stats::MaxHp, rmhp);
}

void Player::setRMMp(int16_t rmmp) {
	if (rmmp > Stats::MaxMaxMp)
		rmmp = Stats::MaxMaxMp;
	else if (rmmp < Stats::MinMaxMp)
		rmmp = Stats::MinMaxMp;
	this->rmmp = rmmp;
	PlayerPacket::updateStatShort(this, Stats::MaxMp, rmmp);
}

void Player::modifyRMHp(int16_t mod) {
	rmhp = (((rmhp + mod) > Stats::MaxMaxHp) ? Stats::MaxMaxHp : (rmhp + mod));
	PlayerPacket::updateStatShort(this, Stats::MaxHp, rmhp);
}

void Player::modifyRMMp(int16_t mod) {
	rmmp = (((rmmp + mod) > Stats::MaxMaxMp) ? Stats::MaxMaxMp : (rmmp + mod));
	PlayerPacket::updateStatShort(this, Stats::MaxMp, rmmp);
}

void Player::setExp(int32_t exp) {
	if (exp < 0)
		exp = 0;
	this->exp = exp;
	PlayerPacket::updateStatInt(this, Stats::Exp, exp);
}

void Player::setMap(int32_t mapid, PortalInfo *portal, bool instance) {
	if (!Maps::getMap(mapid)) {
		MapPacket::portalBlocked(this);
		return;
	}
	Map *oldmap = Maps::getMap(map);
	Map *newmap = Maps::getMap(mapid);

	if (portal == 0)
		portal = newmap->getSpawnPoint();

	if (!instance) { // Only trigger the message for natural map changes not caused by moveAllPlayers, etc.
		int32_t ispartyleader = (getParty() != 0 ? (getParty()->isLeader(getId()) ? 1 : 0) : 0);
		if (oldmap->getInstance() != 0) {
			oldmap->getInstance()->sendMessage(PlayerChangeMap, id, mapid, map, ispartyleader);
		}
		if (newmap->getInstance() != 0) {
			newmap->getInstance()->sendMessage(PlayerChangeMap, id, mapid, map, ispartyleader);
		}
	}

	oldmap->removePlayer(this);
	map = mapid;
	map_pos = portal->id;
	used_portals.clear();
	setPos(Pos(portal->pos.x, portal->pos.y - 40));
	setStance(0);
	setFh(0);
	setFallCounter(0);
	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = getPets()->getSummoned(i)) {
			pet->setPos(portal->pos);
		}
	}

	if (getSummons()->getPuppet() != 0) { // Puppets and non-moving summons don't go with you
		Summons::removeSummon(this, true, true, false, 0);
	}
	if (getSummons()->getSummon() != 0 && getSummons()->getSummon()->getType() == 0) {
		Summons::removeSummon(this, false, true, false, 0);
	}
	if (getActiveBuffs()->hasMarkedMonster()) {
		Buffs::endBuff(this, getActiveBuffs()->getHomingBeacon());
	}
	WorldServerConnectPacket::updateMap(ChannelServer::Instance()->getWorldConnection(), id, mapid);
	MapPacket::changeMap(this);
	Maps::addPlayer(this, mapid);
}

void Player::setLevel(uint8_t level) {
	this->level = level;
	PlayerPacket::updateStatShort(this, Stats::Level, level);
	LevelsPacket::levelUp(this);
	WorldServerConnectPacket::updateLevel(ChannelServer::Instance()->getWorldConnection(), id, level);
}

void Player::changeChannel(int8_t channel) {
	ChannelServer::Instance()->getWorldConnection()->playerChangeChannel(this, channel);
}

void Player::changeKey(PacketReader &packet) {
	int32_t mode = packet.get<int32_t>();
	int32_t howmany = packet.get<int32_t>();

	if (howmany == 0)
		return;

	if (mode == 0) {
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
	else if (mode == 1) {
		// Update auto-HP potion
	}
	else if (mode == 2) {
		// Update auto-MP potion
	}
	// howmany = potion ID for the above, deallocate on 0, I imagine
}

void Player::changeSkillMacros(PacketReader &packet) {
	uint8_t num = packet.get<int8_t>();
	if (num == 0)
		return;

	SkillMacros skillMacros;
	for (uint8_t i = 0; i < num; i++) {
		string name = packet.getString();
		bool shout = packet.getBool();
		int32_t skill1 = packet.get<int32_t>();
		int32_t skill2 = packet.get<int32_t>();
		int32_t skill3 = packet.get<int32_t>();

		skillMacros.add(i, new SkillMacros::SkillMacro(name, shout, skill1, skill2, skill3));
	}
	skillMacros.save(id);
}

void Player::setHair(int32_t id) {
	this->hair = id;
	PlayerPacket::updateStatInt(this, Stats::Hair, id);
}

void Player::setEyes(int32_t id) {
	this->eyes = id;
	PlayerPacket::updateStatInt(this, Stats::Eyes, id);
}

void Player::setSkin(int8_t id) {
	this->skin = id;
	PlayerPacket::updateStatInt(this, Stats::Skin, id);
}

void Player::setFame(int16_t fame) {
	if (fame < Stats::MinFame)
		fame = Stats::MinFame;
	else if (fame > Stats::MaxFame)
		fame = Stats::MaxFame;
	this->fame = fame;
	PlayerPacket::updateStatInt(this, Stats::Fame, fame);
}

bool Player::addWarning() {
	int32_t t = TimeUtilities::getTickCount();
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
		<< "pos = " << static_cast<int16_t>(map_pos) << ","
		<< "gender = " << static_cast<int16_t>(gender) << ","
		<< "skin = " << static_cast<int16_t>(skin) << ","
		<< "eyes = " << eyes << ","
		<< "hair = " << hair << ","
		<< "mesos = " << inv->getMesos() << ","
		<< "equip_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::EquipInventory)) << ","
		<< "use_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::UseInventory)) << ","
		<< "setup_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::SetupInventory)) << ","
		<< "etc_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::EtcInventory)) << ","
		<< "cash_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::CashInventory)) << ","
		<< "buddylist_size = " << static_cast<int16_t>(buddylist_size) << ","
		<< "monsterbookcover = " << getMonsterBook()->getCover() 
		<< " WHERE id = " << id;
	query.exec();
}

void Player::saveAll(bool savecooldowns) {
	saveStats();
	getInventory()->save();
	getMonsterBook()->save();
	getMounts()->save();
	getPets()->save();
	getQuests()->save();
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
	setMap(tomap);
}

bool Player::hasGmEquip() {
	if (getInventory()->getEquippedId(EquipSlots::Helm) == Items::GmHat)
		return true;
	if (getInventory()->getEquippedId(EquipSlots::Top) == Items::GmTop)
		return true;
	if (getInventory()->getEquippedId(EquipSlots::Bottom) == Items::GmBottom)
		return true;
	if (getInventory()->getEquippedId(EquipSlots::Weapon) == Items::GmWeapon)
		return true;
	return false;
}

void Player::setBuddyListSize(uint8_t size) {
	buddylist_size = size;
	BuddyListPacket::showSize(this);
}

void Player::loseExp() {
	if (!GameLogicUtilities::isBeginnerJob(getJob()) && getLevel() < Levels::getMaxLevel(getJob()) && getMap() != Maps::SorcerersRoom) {
		uint16_t charms = getInventory()->getItemAmount(Items::SafetyCharm);
		if (charms > 0) {
			Inventory::takeItem(this, Items::SafetyCharm, 1);
			charms--;
			if (charms > 0xFF)
				charms = 0xFF;
			InventoryPacket::useCharm(this, static_cast<uint8_t>(charms));
			return;
		}
		Map *loc = Maps::getMap(getMap());
		int8_t exploss = 10;
		if ((loc->getInfo()->fieldLimit & FieldLimitBits::RegularExpLoss) != 0 || loc->getInfo()->town)
			exploss = 1;
		else {
			switch (GameLogicUtilities::getJobTrack(getJob(), true)) {
				case Jobs::JobTracks::Magician:
					exploss = 7;
					break;
				case Jobs::JobTracks::Thief:
					exploss = 5;
					break;
			}
		}
		int32_t exp = getExp();
		exp -= static_cast<int32_t>(static_cast<int64_t>(Levels::getExp(getLevel())) * exploss / 100);
		setExp(exp);
	}
}
