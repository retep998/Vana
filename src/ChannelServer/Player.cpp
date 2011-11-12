/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "IpUtilities.h"
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
#include "Pet.h"
#include "PetHandler.h"
#include "PlayerDataProvider.h"
#include "PlayerHandler.h"
#include "PlayerPacket.h"
#include "Quests.h"
#include "ReactorHandler.h"
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
#include <limits>
#include <stdexcept>

Player::Player() :
fall_counter(0),
shop(0),
item_effect(0),
chair(0),
mapchair(0),
trade_id(0),
m_portalCount(0),
trade_state(false),
save_on_dc(true),
is_connect(false),
npc(nullptr),
party(nullptr),
instance(nullptr),
tickCount(-1),
door(nullptr),
changing_channel(false)
{
}

Player::~Player() {
	if (is_connect) {
		Maps::getMap(map)->removePlayer(this);
		is_connect = false; // Do not accept packets anymore (we don't get them anyway).

		if (Door *door = getDoor()) {
			door->closeDoor();
		}
		if (isTrading()) {
			TradeHandler::cancelTrade(this);
		}
		int32_t isleader = 0;
		if (Party *p = getParty()) {
			p->setMember(getId(), nullptr);
			isleader = p->isLeader(getId()) ? 1 : 0;
		}
		if (Instance *i = getInstance()) {
			i->removePlayer(getId());
			i->sendMessage(PlayerDisconnect, getId(), isleader);
		}
		if (getMapChair() != 0) {
			Maps::getMap(getMap())->playerSeated(getMapChair(), nullptr);
		}
		//if (this->getHp() == 0)
		//	this->acceptDeath();
		// "Bug" in global, would be fixed here:
		// When disconnecting and dead, you actually go back to forced return map before the death return map
		// (that means that it's parsed while logging in, not while logging out)
		if (PortalInfo *closest = Maps::getMap(getMap())->getNearestSpawnPoint(getPos())) {
			map_pos = closest->id;
		}
		if (save_on_dc) {
			saveAll(true);
			setOnline(false);
		}
		if (ChannelServer::Instance()->isConnected()) { // Do not connect to worldserver if the worldserver has disconnected
			if (!isChangingChannel()) {
				SyncPacket::buddyOnline(ChannelServer::Instance()->getWorldConnection(), getId(), getBuddyList()->getBuddyIds(), false);
			}
			SyncPacket::removePlayer(ChannelServer::Instance()->getWorldConnection(), id);
		}
		PlayerDataProvider::Instance()->removePlayer(this);
	}
}

void Player::realHandleRequest(PacketReader &packet) {
	try {
		header_t header = packet.getHeader();
		if (!is_connect) {
			// We don't want to accept any other packet than the one for loading the character
			if (header == CMSG_PLAYER_LOAD) {
				playerConnect(packet);
			}
		}
		else {
			switch (header) {
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
				case CMSG_CASH_SHOP: changeServer(true); break;
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
				case CMSG_ITEM_LOOT: DropHandler::lootItem(this, packet); break;
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
				case CMSG_MTS: changeServer(false); break;
				case CMSG_MULTI_STAT_ADDITION: stats->addStatMulti(packet); break;
				case CMSG_MYSTIC_DOOR_ENTRY: PlayerHandler::handleDoorUse(this, packet); break;
				case CMSG_NPC_ANIMATE: NpcHandler::handleNpcAnimation(this, packet); break;
				case CMSG_NPC_TALK: NpcHandler::handleNpc(this, packet); break;
				case CMSG_NPC_TALK_CONT: NpcHandler::handleNpcIn(this, packet); break;
				case CMSG_PARTY: PartyHandler::handleRequest(this, packet); break;
				case CMSG_PET_CHAT: PetHandler::handleChat(this, packet); break;
				case CMSG_PET_COMMAND: PetHandler::handleCommand(this, packet); break;
				case CMSG_PET_CONSUME_POTION: PetHandler::handleConsumePotion(this, packet); break;
				case CMSG_PET_FOOD_USE: PetHandler::handleFeed(this, packet); break;
				case CMSG_PET_LOOT: DropHandler::petLoot(this, packet); break;
				case CMSG_PET_MOVEMENT: PetHandler::handleMovement(this, packet); break;
				case CMSG_PET_SUMMON: PetHandler::handleSummon(this, packet); break;
				case CMSG_PLAYER_CHAT: ChatHandler::handleChat(this, packet); break;
				case CMSG_PLAYER_DAMAGE: PlayerHandler::handleDamage(this, packet); break;
				case CMSG_PLAYER_HEAL: PlayerHandler::handleHeal(this, packet); break;
				case CMSG_PLAYER_INFO: PlayerHandler::handleGetInfo(this, packet); break;
				case CMSG_PLAYER_MOVE: PlayerHandler::handleMoving(this, packet); break;
				case CMSG_PLAYER_ROOM: TradeHandler::tradeHandler(this, packet); break;
				case CMSG_PONG: handlePong(); break;
				case CMSG_QUEST_OBTAIN: Quests::getQuest(this, packet); break;
				case CMSG_REACTOR_HIT: ReactorHandler::hitReactor(this, packet); break;
				case CMSG_REACTOR_TOUCH: ReactorHandler::touchReactor(this, packet); break;
				case CMSG_REVIVE_EFFECT: InventoryHandler::useItemEffect(this, packet); break;
				case CMSG_SCROLL_USE: InventoryHandler::useScroll(this, packet); break;
				case CMSG_SHOP: NpcHandler::useShop(this, packet); break;
				case CMSG_SKILL_ADD: Skills::addSkill(this, packet); break;
				case CMSG_SKILL_CANCEL: Skills::cancelSkill(this, packet); break;
				case CMSG_SKILL_USE: Skills::useSkill(this, packet); break;
				case CMSG_SKILLBOOK_USE: InventoryHandler::useSkillbook(this, packet); break;
				case CMSG_SPECIAL_SKILL: PlayerHandler::handleSpecialSkills(this, packet); break;
				case CMSG_STAT_ADDITION: stats->addStat(packet); break;
				case CMSG_STORAGE: NpcHandler::useStorage(this, packet); break;
				case CMSG_SUMMON_ATTACK: PlayerHandler::useSummonAttack(this, packet); break;
				case CMSG_SUMMON_BAG_USE: InventoryHandler::useSummonBag(this, packet); break;
				case CMSG_SUMMON_DAMAGE: Summons::damageSummon(this, packet); break;
				case CMSG_SUMMON_MOVEMENT: Summons::moveSummon(this, packet); break;
				case CMSG_TELEPORT_ROCK: InventoryHandler::handleRockFunctions(this, packet); break;
				case CMSG_TELEPORT_ROCK_USE: InventoryHandler::handleRockTeleport(this, Items::SpecialTeleportRock, packet); break;
				case CMSG_TOWN_SCROLL_USE: InventoryHandler::useReturnScroll(this, packet); break;
				case CMSG_USE_CHAIR: InventoryHandler::useChair(this, packet); break;
				case CMSG_USE_PIRATE_GRENADE: PlayerHandler::usePirateGrenade(this, packet); break;
				case CMSG_USE_REWARD_ITEM: InventoryHandler::handleRewardItem(this, packet); break;
				case CMSG_USE_SCRIPT_ITEM: InventoryHandler::handleScriptItem(this, packet); break;
#ifdef VANA_SHOW_UNKNOWN_PACKETS
				default: {
					packet.reset();
					std::stringstream x;
					x << "Unknown Packet. Player ID: " << getId() << "; Packet: " << packet;
					ChannelServer::Instance()->log(LogTypes::Info, x.str());
					break;
				}
#endif
			}
		}
	}
	catch (std::range_error) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly

		packet.reset();
		std::stringstream x;
		x << "Player ID: " << getId() << "; Packet: " << packet;
		ChannelServer::Instance()->log(LogTypes::MalformedPacket, x.str());
		getSession()->disconnect();
	}
}

void Player::playerConnect(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	if (!Connectable::Instance()->checkPlayer(id, getIp())) {
		// Hacking
		getSession()->disconnect();
		return;
	}
	this->id = id;

	// Character info
	mysqlpp::Query query = Database::getCharDB().query();

	query << "SELECT "
		<< "c.*, u.gm, u.admin FROM characters c "
		<< "LEFT JOIN users u ON c.userid = u.id "
		<< "WHERE c.id = " << id;

	mysqlpp::StoreQueryResult res = query.store();

	if (res.empty()) {
		// Hacking
		getSession()->disconnect();
		return;
	}

	res[0]["name"].to_string(name);
	user_id		= res[0]["userid"];
	map			= res[0]["map"];
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
	bool checked = PlayerDataProvider::Instance()->checkPlayer(id);

	if (checked) {
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

	if (isGm() || isAdmin()) {
		if (!checked) {
			map = Maps::GmMap;
			map_pos = -1;
		}
	}
	else {
		if (Maps::getMap(map)->getForcedReturn() != Maps::NoMap) {
			map = Maps::getMap(map)->getForcedReturn();
			map_pos = -1;
		}
		else if (static_cast<int16_t>(res[0]["chp"]) == 0) {
			map = Maps::getMap(map)->getReturnMap();
			map_pos = -1;
		}
	}
	m_pos = Maps::getMap(map)->getSpawnPoint(map_pos)->pos;
	m_stance = 0;
	m_foothold = 0;

	PlayerPacket::connectData(this);

	if (ChannelServer::Instance()->getScrollingHeader().size() > 0) {
		ServerPacket::showScrollingHeader(this, ChannelServer::Instance()->getScrollingHeader());
	}

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = pets->getSummoned(i)) {
			pet->setPos(Maps::getMap(map)->getSpawnPoint(map_pos)->pos);
		}
	}

	PlayerPacket::showKeys(this, &keyMaps);

	BuddyListPacket::update(this, BuddyListPacket::ActionTypes::Add);
	getBuddyList()->checkForPendingBuddy();

	PlayerPacket::showSkillMacros(this, &skillMacros);

	Maps::addPlayer(this, map);

	std::stringstream x;
	x << getName() << " (" << getId() << ") connected from " << IpUtilities::ipToString(getIp());
	ChannelServer::Instance()->log(LogTypes::Info, x.str());

	setOnline(true);
	is_connect = true;
	SyncPacket::registerPlayer(ChannelServer::Instance()->getWorldConnection(), getIp(), id, name, map, stats->getJob(), stats->getLevel(), guildid, guildrank, allianceid, alliancerank);

	SyncPacket::buddyOnline(ChannelServer::Instance()->getWorldConnection(), getId(), getBuddyList()->getBuddyIds(), true);
}

void Player::setMap(int32_t mapid, int8_t pointid, const Pos &spawnpoint, int16_t fh) {
	if (!Maps::getMap(mapid)) {
		MapPacket::portalBlocked(this);
		return;
	}

	Map *oldmap = Maps::getMap(map);
	Map *newmap = Maps::getMap(mapid);

	if (!instance) {
		// Only trigger the message for natural map changes not caused by moveAllPlayers, etc.
		int32_t ispartyleader = (getParty() != nullptr ? (getParty()->isLeader(getId()) ? 1 : 0) : 0);
		if (Instance *ii = oldmap->getInstance()) {
			ii->sendMessage(PlayerChangeMap, id, mapid, map, ispartyleader);
		}
		if (Instance *ee = newmap->getInstance()) {
			ee->sendMessage(PlayerChangeMap, id, mapid, map, ispartyleader);
		}
	}

	oldmap->removePlayer(this);
	map = mapid;
	map_pos = pointid;
	used_portals.clear();
	setPos(spawnpoint);
	setStance(0);
	setFh(fh);
	setFallCounter(0);

	// Prevent chair Denial of Service
	if (getMapChair() != 0) {
		oldmap->playerSeated(getMapChair(), nullptr);
		setMapChair(0);
	}
	if (getChair() != 0) {
		setChair(0);
	}

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = getPets()->getSummoned(i)) {
			pet->setPos(spawnpoint);
		}
	}

	// Puppets and non-moving summons don't go with you
	if (getSummons()->getPuppet() != nullptr) {
		Summons::removeSummon(this, true, false, SummonMessages::None);
	}
	if (getSummons()->getSummon() != nullptr && getSummons()->getSummon()->getType() == Summon::Static) {
		Summons::removeSummon(this, false, false, SummonMessages::None);
	}

	if (getActiveBuffs()->hasMarkedMonster()) {
		Buffs::endBuff(this, getActiveBuffs()->getHomingBeacon());
	}
	if (!getChalkboard().empty() && !newmap->canChalkboard()) {
		setChalkboard("");
	}

	SyncPacket::updateMap(ChannelServer::Instance()->getWorldConnection(), id, mapid);
	MapPacket::changeMap(this);
	Maps::addPlayer(this, mapid);
}

void Player::setMap(int32_t mapid, PortalInfo *portal, bool instance) {
	if (!Maps::getMap(mapid)) {
		MapPacket::portalBlocked(this);
		return;
	}

	Map *oldmap = Maps::getMap(map);
	Map *newmap = Maps::getMap(mapid);

	if (portal == nullptr)
		portal = newmap->getSpawnPoint();

	if (!instance) {
		// Only trigger the message for natural map changes not caused by moveAllPlayers, etc.
		int32_t ispartyleader = (getParty() != nullptr ? (getParty()->isLeader(getId()) ? 1 : 0) : 0);
		if (Instance *ii = oldmap->getInstance()) {
			ii->sendMessage(PlayerChangeMap, id, mapid, map, ispartyleader);
		}
		if (Instance *ee = newmap->getInstance()) {
			ee->sendMessage(PlayerChangeMap, id, mapid, map, ispartyleader);
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

	// Prevent chair Denial of Service
	if (getMapChair() != 0) {
		oldmap->playerSeated(getMapChair(), nullptr);
		setMapChair(0);
	}
	if (getChair() != 0) {
		setChair(0);
	}

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = getPets()->getSummoned(i)) {
			pet->setPos(portal->pos);
		}
	}

	// Puppets and non-moving summons don't go with you
	if (getSummons()->getPuppet() != nullptr) {
		Summons::removeSummon(this, true, false, SummonMessages::None);
	}
	if (getSummons()->getSummon() != nullptr && getSummons()->getSummon()->getType() == Summon::Static) {
		Summons::removeSummon(this, false, false, SummonMessages::None);
	}

	if (getActiveBuffs()->hasMarkedMonster()) {
		Buffs::endBuff(this, getActiveBuffs()->getHomingBeacon());
	}
	if (!getChalkboard().empty() && !newmap->canChalkboard()) {
		setChalkboard("");
	}

	SyncPacket::updateMap(ChannelServer::Instance()->getWorldConnection(), id, mapid);
	MapPacket::changeMap(this);
	Maps::addPlayer(this, mapid);
}

string Player::getMedalName() {
	string ret;
	if (int32_t itemid = getInventory()->getEquippedId(EquipSlots::Medal)) {
		// Check if there's an item at that slot
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

	enum KeyModes {
		ChangeKeys = 0x00,
		AutoHpPotion = 0x01,
		AutoMpPotion = 0x02
	};

	if (mode == ChangeKeys) {
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
	else if (mode == AutoHpPotion) {
		getInventory()->setAutoHpPot(howmany);
	}
	else if (mode == AutoMpPotion) {
		getInventory()->setAutoMpPot(howmany);
	}
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
		if (warnings[i] + 30000 < t) {
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
		<< "mhp = " << stats->getMaxHp(true) << ","
		<< "cmp = " << stats->getMp() << ","
		<< "mmp = " << stats->getMaxMp(true) << ","
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
	int32_t tomap = (Maps::getMap(map) ? Maps::getMap(map)->getReturnMap() : map);
	if (wheel) {
		tomap = this->getMap();
	}
	stats->setHp(50, false);
	getActiveBuffs()->removeBuff();
	setMap(tomap);
}

bool Player::hasGmEquip() const {
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

uint8_t Player::getPortalCount(bool add) {
	if (add) {
		m_portalCount++;
	}
	return m_portalCount;
}

void Player::changeServer(bool cashShop) {
	SyncPacket::playerChangeServer(ChannelServer::Instance()->getWorldConnection(), this, cashShop);
}

void Player::handlePong() {
	// Handle all things like expiring of quests and such
	getInventory()->checkExpiredItems();
	getBuddyList()->checkForPendingBuddy();

	int32_t t = TimeUtilities::getTickCount();
	// Deleting old warnings
	for (size_t i = 0; i < warnings.size(); i++) {
		if (warnings[i] + 30000 < t) {
			warnings.erase(warnings.begin() + i);
			i--;
		}
	}
}

bool Player::updateTickCount(int32_t newValue) {
	/*
	int32_t diff = newValue - tickCount;
	if (tickCount != -1 && diff < 100) {
		std::cout << "Warning: Player: " << getName() << ", Dif: " << diff << std::endl;
		addWarning();
		return false;
	}
	else {
		tickCount = newValue;
		return true;
	}
	*/
	return true;
}