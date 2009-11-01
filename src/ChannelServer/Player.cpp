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
#include "AlliancePacket.h"
#include "BuddyListHandler.h"
#include "BuddyListPacket.h"
#include "BbsPacket.h"
#include "ChannelServer.h"
#include "ChatHandler.h"
#include "CommandHandler.h"
#include "Connectable.h"
#include "Database.h"
#include "DropHandler.h"
#include "Fame.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "GuildPacket.h"
#include "Instance.h"
#include "Inventory.h"
#include "InventoryHandler.h"
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "KeyMaps.h"
#include "LevelsPacket.h"
#include "MapleSession.h"
#include "MapPacket.h"
#include "Maps.h"
#include "MobHandler.h"
#include "MonsterBookPacket.h"
#include "Npc.h"
#include "NpcHandler.h"
#include "PacketReader.h"
#include "Party.h"
#include "PartyHandler.h"
#include "Pets.h"
#include "PlayerDataProvider.h"
#include "PlayerHandler.h"
#include "PlayerPacket.h"
#include "Quests.h"
#include "Reactors.h"
#include "RecvHeader.h"
#include "ServerPacket.h"
#include "SkillMacros.h"
#include "Skills.h"
#include "StringUtilities.h"
#include "Summons.h"
#include "SyncPacket.h"
#include "TimeUtilities.h"
#include "TradeHandler.h"
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
			SyncPacket::removePlayer(ChannelServer::Instance()->getWorldConnection(), id);	
		}
		Maps::getMap(map)->removePlayer(this);
		PlayerDataProvider::Instance()->removePlayer(this);
	}
}

void Player::realHandleRequest(PacketReader &packet) {
	switch (packet.get<int16_t>()) {
		case CMSG_ALLIANCE: AlliancePacket::handlePacket(this, packet); break;
		case CMSG_ALLIANCE_DENIED: AlliancePacket::handleDenyPacket(this, packet); break;		
		case CMSG_ADMIN_COMMAND: CommandHandler::handleAdminCommand(this, packet); break;
		case CMSG_ADMIN_MESSENGER: PlayerHandler::handleAdminMessenger(this, packet); break;
		case CMSG_ATTACK_ENERGY_CHARGE: PlayerHandler::useEnergyChargeAttack(this, packet); break;
		case CMSG_ATTACK_MAGIC: PlayerHandler::useSpellAttack(this, packet); break;
		case CMSG_ATTACK_MELEE: PlayerHandler::useMeleeAttack(this, packet); break;
		case CMSG_ATTACK_RANGED: PlayerHandler::useRangedAttack(this, packet); break;
		case CMSG_BBS: BbsPacket::handleBbsPacket(this, packet); break;
		case CMSG_BUDDY: BuddyListHandler::handleBuddyList(this, packet); break;
		case CMSG_CASH_ITEM_USE: InventoryHandler::useCashItem(this, packet); break;
		case CMSG_CASH_SHOP: PlayerPacket::sendBlockedMessage(this, 0x02); break;
		case CMSG_CHAIR: InventoryHandler::handleChair(this, packet); break;
		case CMSG_CHALKBOARD: InventoryPacket::sendChalkboardUpdate(this); setChalkboard(""); break;
		case CMSG_CHANNEL_CHANGE: changeChannel(packet.get<int8_t>()); break;
		case CMSG_COMMAND: CommandHandler::handleCommand(this, packet); break;
		case CMSG_DROP_MESOS: DropHandler::dropMesos(this, packet); break;
		case CMSG_EMOTE: PlayerHandler::handleFacialExpression(this, packet); break;
		case CMSG_FAME: Fame::handleFame(this, packet); break;
		case CMSG_FRIENDLY_MOB_DAMAGE: MobHandler::friendlyDamaged(this, packet); break;
		case CMSG_GUILD: GuildPacket::handlePacket(this, packet); break;
		case CMSG_GUILD_DENIED: GuildPacket::handleDenyPacket(this, packet); break;
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
		case CMSG_MULTI_STAT_ADDITION: stats->addStatMulti(packet); break;
		case CMSG_NPC_ANIMATE: NpcHandler::handleNpcAnimation(this, packet); break;
		case CMSG_NPC_TALK: NpcHandler::handleNpc(this, packet); break;
		case CMSG_NPC_TALK_CONT: NpcHandler::handleNpcIn(this, packet); break;
		case CMSG_PARTY: PartyHandler::handleRequest(this, packet); break;
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
		case CMSG_REVIVE_EFFECT: InventoryHandler::useItemEffect(this, packet); break;
		case CMSG_SCROLL_USE: InventoryHandler::useScroll(this, packet); break;
		case CMSG_SHOP: InventoryHandler::useShop(this, packet); break;
		case CMSG_SKILL_ADD: Skills::addSkill(this, packet); break;
		case CMSG_SKILL_CANCEL: Skills::cancelSkill(this, packet); break;
		case CMSG_SKILL_USE: Skills::useSkill(this, packet); break;
		case CMSG_SKILLBOOK_USE: InventoryHandler::useSkillbook(this, packet); break;
		case CMSG_SPECIAL_SKILL: PlayerHandler::handleSpecialSkills(this, packet); break;
		case CMSG_STAT_ADDITION: stats->addStat(packet); break;
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
	query << "SELECT characters.*, users.gm, users.admin FROM characters LEFT JOIN users on characters.userid = users.id WHERE characters.id = " << id;
	mysqlpp::StoreQueryResult res = query.store();

	if (res.empty()) {
		// Hacking
		getSession()->disconnect();
		return;
	}

	res[0]["name"].to_string(name);
	user_id		= res[0]["userid"];
	map		    = res[0]["map"];
	gm_level	= res[0]["gm"];
	admin		= StringUtilities::atob(res[0]["admin"]);
	eyes		= res[0]["eyes"];
	hair		= res[0]["hair"];
	world_id	= static_cast<int8_t>(res[0]["world_id"]);
	gender		= static_cast<int8_t>(res[0]["gender"]);
	skin		= static_cast<int8_t>(res[0]["skin"]);
	map_pos		= static_cast<int8_t>(res[0]["pos"]);
	buddylist_size = static_cast<uint8_t>(res[0]["buddylist_size"]);
	guildid		= res[0]["guildid"];
	guildrank	= static_cast<uint8_t>(res[0]["guildrank"]);
	alliancerank = static_cast<uint8_t>(res[0]["alliancerank"]);
	allianceid = res[0]["allianceid"];

	if (Maps::getMap(map)->getInfo()->forcedReturn != Maps::NoMap) {
		map = Maps::getMap(map)->getInfo()->forcedReturn;
		map_pos = 0;
	}
	else if (static_cast<int16_t>(res[0]["chp"]) == 0) {
		map = Maps::getMap(map)->getInfo()->rm;
	}

	m_pos = Maps::getMap(map)->getSpawnPoint(map_pos)->pos;
	m_stance = 0;
	m_foothold = 0;

	// Stats
	stats.reset(new PlayerStats(this, static_cast<uint8_t>(res[0]["level"]),
		static_cast<int16_t>(res[0]["job"]),
		static_cast<int16_t>(res[0]["fame"]),
		static_cast<int16_t>(res[0]["str"]),
		static_cast<int16_t>(res[0]["dex"]),
		static_cast<int16_t>(res[0]["int"]),
		static_cast<int16_t>(res[0]["luk"]),
		static_cast<int16_t>(res[0]["ap"]),
		static_cast<uint16_t>(res[0]["hpmp_ap"]),
		static_cast<int16_t>(res[0]["sp"]),
		static_cast<int16_t>(res[0]["chp"]),
		static_cast<int16_t>(res[0]["mhp"]),
		static_cast<int16_t>(res[0]["cmp"]),
		static_cast<int16_t>(res[0]["mmp"]),
		res[0]["exp"]));

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
	if (PlayerDataProvider::Instance()->checkPlayer(id)) {
		PacketReader pack = PlayerDataProvider::Instance()->getPacket(id);

		setConnectionTime(pack.get<int64_t>());

		getActiveBuffs()->parseBuffTransferPacket(pack);
		if (getActiveBuffs()->hasHyperBody()) {
			int32_t skillid = getActiveBuffs()->getHyperBody();
			uint8_t hblevel = getActiveBuffs()->getActiveSkillLevel(skillid);
			SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillid, hblevel);
			stats->setHyperBody(hb->x, hb->y);
		}

		getSummons()->parseSummonTransferPacket(pack);

		PlayerDataProvider::Instance()->removePacket(id);
	}
	else {
		// No packet, that means that they're connecting for the first time
		setConnectionTime(time(0));
	}

	// The rest
	variables.reset(new PlayerVariables(this));
	buddyList.reset(new PlayerBuddyList(this));
	quests.reset(new PlayerQuests(this));
	randStream.reset(new PlayerRandStream(this));
	monsterBook.reset(new PlayerMonsterBook(this));

	getMonsterBook()->setCover(res[0]["monsterbookcover"]);

	// Key Maps and Macros
	KeyMaps keyMaps;
	keyMaps.load(id);

	SkillMacros skillMacros;
	skillMacros.load(id);

	stats->checkHpMp(); // Adjust down HP or MP if necessary

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
	SyncPacket::registerPlayer(ChannelServer::Instance()->getWorldConnection(), getIp(), id, name, map, stats->getJob(), stats->getLevel(), guildid, guildrank, allianceid, alliancerank);
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
	if (!getChalkboard().empty() && (newmap->getInfo()->fieldLimit & FieldLimitBits::Chalkboard) != 0) {
		setChalkboard("");
	}
	SyncPacket::updateMap(ChannelServer::Instance()->getWorldConnection(), id, mapid);
	MapPacket::changeMap(this);
	Maps::addPlayer(this, mapid);
}

string Player::getMedalName() {
	string ret;
	if (int32_t itemid = getInventory()->getEquippedId(EquipSlots::Medal)) { // Check if there's an item at that slot
		ret = "<";
		ret += ItemDataProvider::Instance()->getItemName(itemid);
		ret += "> ";
	}
	ret += getName();
	return ret;
}

void Player::changeChannel(int8_t channel) {
	SyncPacket::playerChangeChannel(ChannelServer::Instance()->getWorldConnection(), this, channel);
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
		<< "level = " << static_cast<int16_t>(stats->getLevel()) << "," // Queries have problems with int8_t due to being derived from ostream
		<< "job = " << stats->getJob() << ","
		<< "str = " << stats->getStr() << ","
		<< "dex = " << stats->getDex() << ","
		<< "`int` = " << stats->getInt() << ","
		<< "luk = " << stats->getLuk() << ","
		<< "chp = " << stats->getHp() << ","
		<< "mhp = " << stats->getMHp(true) << ","
		<< "cmp = " << stats->getMp() << ","
		<< "mmp = " << stats->getMMp(true) << ","
		<< "hpmp_ap = " << stats->getHpMpAp() << ","
		<< "ap = " << stats->getAp() << ","
		<< "sp = " << stats->getSp() << ","
		<< "exp = " << stats->getExp() << ","
		<< "fame = " << stats->getFame() << ","
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
		<< "guildid = " << guildid << ","
		<< "guildrank = " << static_cast<int16_t>(guildrank) << ","
		<< "allianceid = " << allianceid << ","
		<< "alliancerank = " << static_cast<int16_t>(alliancerank) << ","
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

void Player::acceptDeath(bool wheel) {
	int32_t tomap = (Maps::getMap(map) ? Maps::getMap(map)->getInfo()->rm : map);
	if (wheel) {
		tomap = this->getMap();
	}
	stats->setHp(50, false);
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
