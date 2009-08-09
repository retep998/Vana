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
#include "NPCs.h"
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
#include "WorldServerConnectPacket.h"
#include "WorldServerConnectPlayer.h"
#include <boost/array.hpp>

Player::Player() :
fall_counter(0),
shop(0),
item_effect(0),
chair(0),
mapchair(0),
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
			WorldServerConnectPacket::removePlayer(ChannelServer::Instance()->getWorldPlayer(), id);	
		}
		Maps::getMap(map)->removePlayer(this);
		Players::Instance()->removePlayer(this);
	}


void Player::realHandleRequest(PacketReader &packet) {
	switch (packet.get<int16_t>()) {
		case RECV_ADD_SKILL: Skills::addSkill(this, packet); break;
		case RECV_ADD_STAT: Levels::addStat(this, packet); break;
		case RECV_ANIMATE_NPC: NPCs::handleNPCAnimation(this, packet); break;
		case RECV_ATTACK_ENERGYCHARGE: PlayerHandler::useEnergyChargeAttack(this, packet); break;
		case RECV_ATTACK_MELEE: PlayerHandler::useMeleeAttack(this, packet); break;
		case RECV_ATTACK_RANGED: PlayerHandler::useRangedAttack(this, packet); break;
		case RECV_ATTACK_SPELL: PlayerHandler::useSpellAttack(this, packet); break;
		case RECV_ATTACK_SUMMON: PlayerHandler::useSummonAttack(this, packet); break;
		case RECV_BUDDYLIST: BuddyListHandler::handleBuddyList(this, packet); break;
		case RECV_CANCEL_ITEM: InventoryHandler::cancelItem(this, packet); break;
		case RECV_CANCEL_SKILL: Skills::cancelSkill(this, packet); break;
		case RECV_CASH_BUTTON: unlock("Cash Shop"); break;
		case RECV_CHAIR_ACTION: InventoryHandler::handleChair(this, packet); break;
		case RECV_CHANGE_CHANNEL: changeChannel(packet.get<int8_t>()); break;
		case RECV_CHANGE_MAP: Maps::usePortal(this, packet); break;
		case RECV_CHANGE_MAP_SPECIAL: Maps::useScriptedPortal(this, packet); break;
		case RECV_CHANNEL_LOGIN: playerConnect(packet); break;
		case RECV_CHAT: ChatHandler::handleChat(this, packet); break;
		case RECV_COMMAND: CommandHandler::handleCommand(this, packet); break;
		case RECV_CONTROL_MOB: MobHandler::monsterControl(this, packet); break;
		case RECV_DAMAGE_PLAYER: PlayerHandler::handleDamage(this, packet); break;
		case RECV_DAMAGE_SUMMON: Summons::damageSummon(this, packet); break;
		case RECV_DROP_MESO: DropHandler::dropMesos(this, packet); break;
		case RECV_FACE_EXPRESSION: PlayerHandler::handleFacialExpression(this, packet); break;
		case RECV_FAME: Fame::handleFame(this, packet); break;
		case RECV_FRIENDLY_MOB_DAMAGED: MobHandler::friendlyDamaged(this, packet); break;
		case RECV_GET_PLAYER_INFO: PlayerHandler::handleGetInfo(this, packet); break;
		case RECV_GET_QUEST: Quests::getQuest(this, packet); break;
		case RECV_GROUP_CHAT: ChatHandler::handleGroupChat(this, packet); break;
		case RECV_HEAL_PLAYER: PlayerHandler::handleHeal(this, packet); break;
		case RECV_HIT_REACTOR: Reactors::hitReactor(this, packet); break;
		case RECV_KEYMAP: changeKey(packet); break;
		case RECV_LOOT_ITEM: DropHandler::playerLoot(this, packet); break;
		case RECV_MOB_BOMB_EXPLOSION: MobHandler::handleBomb(this, packet); break;
		case RECV_MOB_DAMAGE_MOB: MobHandler::handleTurncoats(this, packet); break;
		case RECV_MOVE_ITEM: InventoryHandler::itemMove(this, packet); break;
		case RECV_MOVE_PLAYER: PlayerHandler::handleMoving(this, packet); break;
		case RECV_MOVE_SUMMON: Summons::moveSummon(this, packet); break;
		case RECV_MTS_BUTTON: unlock("MTS"); break;
		case RECV_NPC_TALK: NPCs::handleNPC(this, packet); break;
		case RECV_NPC_TALK_CONT: NPCs::handleNPCIn(this, packet); break;
		case RECV_PARTY_ACTION: PartyFunctions::handleRequest(this, packet); break;
		case RECV_PET_CHAT: Pets::handleChat(this, packet); break;
		case RECV_PET_COMMAND: Pets::handleCommand(this, packet); break;
		case RECV_PET_FEED: Pets::handleFeed(this, packet); break;
		case RECV_PET_LOOT: DropHandler::petLoot(this, packet); break;
		case RECV_PET_MOVE: Pets::handleMovement(this, packet); break;
		case RECV_PET_SUMMON: Pets::handleSummon(this, packet); break;
		case RECV_PLAYER_ROOM_ACTION: TradeHandler::tradeHandler(this, packet); break;
		case RECV_SHOP_ENTER: InventoryHandler::useShop(this, packet); break;
		case RECV_SKILL_MACRO: changeSkillMacros(packet); break;
		case RECV_SPECIAL_SKILL: PlayerHandler::handleSpecialSkills(this, packet); break;
		case RECV_TELEPORT_ROCK_FUNCTION: InventoryHandler::handleRockFunctions(this, packet); break;
		case RECV_USE_CASH_ITEM: InventoryHandler::useCashItem(this, packet); break;
		case RECV_USE_CHAIR: InventoryHandler::useChair(this, packet); break;
		case RECV_USE_ITEM: InventoryHandler::useItem(this, packet); break;
		case RECV_USE_ITEM_EFFECT: InventoryHandler::useItemEffect(this, packet); break;
		case RECV_USE_RETURN_SCROLL: InventoryHandler::useReturnScroll(this, packet); break;
		case RECV_USE_SCROLL: InventoryHandler::useScroll(this, packet); break;
		case RECV_USE_SKILL: Skills::useSkill(this, packet); break;
		case RECV_USE_SKILLBOOK: InventoryHandler::useSkillbook(this, packet); break;
		case RECV_USE_STORAGE: InventoryHandler::useStorage(this, packet); break;
		case RECV_USE_SUMMON_BAG: InventoryHandler::useSummonBag(this, packet); break;
		case RECV_USE_TELEPORT_ROCK: packet.skipBytes(5); InventoryHandler::handleRockTeleport(this, packet.get<int8_t>(), Items::SpecialTeleportRock, packet); break;
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
	user_id = res[0]["userid"];
	map = res[0]["map"];
	gm_level = res[0]["gm"];
	world_id = static_cast<int8_t>(res[0]["world_id"]);
	map_pos = static_cast<int8_t>(res[0]["pos"]);
	buddylist_size = static_cast<uint8_t>(res[0]["buddylist_size"]);

	// Stats
	stats.reset(new PlayerStats(this));

	if (Maps::getMap(map)->getInfo()->forcedReturn != Maps::NoMap) {
		map = Maps::getMap(map)->getInfo()->forcedReturn;
		map_pos = 0;
		if (getStats()->getHp() == 0)
			getStats()->setHp(50, false);
	}
	else if (getStats()->getHp() == 0) {
		getStats()->setHp(50, false);
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

	// getInventory()->countAllGear(true); // Commenting for now, causes problems

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
			getStats()->setHyperBody(hb->x, hb->y);
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

	// Key Maps and Macros
	KeyMaps keyMaps;
	keyMaps.load(id);

	SkillMacros skillMacros;
	skillMacros.load(id);

	if (getStats()->getHp() > getStats()->getMHp())
		getStats()->setHp(getStats()->getMHp(), false);
	if (getStats()->getMp() > getStats()->getMMp())
		getStats()->setMp(getStats()->getMMp());

	PlayerPacket::connectData(this);

	if (ChannelServer::Instance()->getScrollingHeader().size() > 0)
		ServerPacket::showScrollingHeader(this, ChannelServer::Instance()->getScrollingHeader());

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
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
	WorldServerConnectPacket::registerPlayer(ChannelServer::Instance()->getWorldPlayer(), ip, id, name, map, getStats()->getJob(), getStats()->getLevel());
}

void Player::unlock(const string &source) {
	PlayerPacket::showMessage(this, "The " + source + " is unavailable.", 5);
	InventoryPacket::blankUpdate(this);
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

	WorldServerConnectPacket::updateMap(ChannelServer::Instance()->getWorldPlayer(), id, mapid);
	MapPacket::changeMap(this);
	Maps::newMap(this, mapid);
}

void Player::changeChannel(int8_t channel) {
	ChannelServer::Instance()->getWorldPlayer()->playerChangeChannel(this, channel);
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

void Player::saveInformation() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET "
		<< "mesos = " << inv->getMesos() << ","
		<< "map = " << map << ","
		<< "pos = " << static_cast<int16_t>(map_pos) << ","
		<< "equip_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::EquipInventory)) << ","
		<< "use_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::UseInventory)) << ","
		<< "setup_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::SetupInventory)) << ","
		<< "etc_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::EtcInventory)) << ","
		<< "cash_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::CashInventory)) << ","
		<< "buddylist_size = " << static_cast<int16_t>(buddylist_size)
		<< " WHERE id = " << id;
	query.exec();
}

void Player::saveAll(bool savecooldowns) {
	saveInformation();
	getInventory()->save();
	getMounts()->save();
	getPets()->save();
	getQuests()->save();
	getSkills()->save(savecooldowns);
	getStats()->save();
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
	getStats()->setHp(50, false);
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
	if (!GameLogicUtilities::isBeginnerJob(getStats()->getJob()) && getStats()->getLevel() < Stats::PlayerLevels) {
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
			switch (GameLogicUtilities::getJobTrack(getStats()->getJob())) {
				case Jobs::JobTracks::Magician:
					exploss = 7;
					break;
				case Jobs::JobTracks::Thief:
					exploss = 5;
					break;
			}
		}
		int32_t exp = getStats()->getExp();
		exp -= static_cast<int32_t>(static_cast<int64_t>(Levels::getExp(getStats()->getLevel())) * exploss / 100);
		getStats()->setExp(exp);
	}
}
