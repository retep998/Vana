/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Player.hpp"
#include "BuddyListHandler.hpp"
#include "BuddyListPacket.hpp"
#include "ChannelServer.hpp"
#include "ChatHandler.hpp"
#include "CmsgHeader.hpp"
#include "CommandHandler.hpp"
#include "Connectable.hpp"
#include "Database.hpp"
#include "DropHandler.hpp"
#include "Fame.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "Instance.hpp"
#include "Inventory.hpp"
#include "InventoryHandler.hpp"
#include "InventoryPacket.hpp"
#include "ItemDataProvider.hpp"
#include "KeyMaps.hpp"
#include "LevelsPacket.hpp"
#include "Map.hpp"
#include "MapPacket.hpp"
#include "Maps.hpp"
#include "MobHandler.hpp"
#include "MonsterBookPacket.hpp"
#include "Npc.hpp"
#include "NpcHandler.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "Party.hpp"
#include "PartyHandler.hpp"
#include "Pet.hpp"
#include "PetHandler.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerHandler.hpp"
#include "PlayerPacket.hpp"
#include "Quests.hpp"
#include "Randomizer.hpp"
#include "ReactorHandler.hpp"
#include "ServerPacket.hpp"
#include "Session.hpp"
#include "SkillMacros.hpp"
#include "Skills.hpp"
#include "SplitPacketBuilder.hpp"
#include "StringUtilities.hpp"
#include "Summon.hpp"
#include "SummonHandler.hpp"
#include "SyncPacket.hpp"
#include "TimeUtilities.hpp"
#include "TradeHandler.hpp"
#include "WorldServerConnection.hpp"
#include <array>
#include <stdexcept>

Player::Player() :
	MovableLife(0, Pos(), 0)
{
}

Player::~Player() {
	if (m_isConnect) {
		Map *curMap = Maps::getMap(m_map);
		if (getMapChair() != 0) {
			curMap->playerSeated(getMapChair(), nullptr);
		}

		curMap->removePlayer(this);
		m_isConnect = false;

		if (isTrading()) {
			TradeHandler::cancelTrade(this);
		}

		int32_t isLeader = 0;
		if (Party *party = getParty()) {
			isLeader = party->isLeader(getId()) ? 1 : 0;
		}
		if (Instance *instance = getInstance()) {
			instance->removePlayer(getId());
			instance->sendMessage(InstanceMessage::PlayerDisconnect, getId(), isLeader);
		}
		//if (this->getStats()->isDead()) {
		//	this->acceptDeath();
		//}
		// "Bug" in global, would be fixed here:
		// When disconnecting and dead, you actually go back to forced return map before the death return map
		// (that means that it's parsed while logging in, not while logging out)
		if (PortalInfo *closest = curMap->getNearestSpawnPoint(getPos())) {
			m_mapPos = closest->id;
		}

		if (m_saveOnDc) {
			saveAll(true);
			setOnline(false);
		}

		if (ChannelServer::getInstance().isConnected()) {
			if (!isChangingChannel()) {
				ChannelServer::getInstance().sendWorld(SyncPacket::BuddyPacket::buddyOnline(getId(), getBuddyList()->getBuddyIds(), false));
			}
			// Do not connect to worldserver if the worldserver has disconnected
			ChannelServer::getInstance().sendWorld(SyncPacket::PlayerPacket::disconnect(getId()));
		}
		PlayerDataProvider::getInstance().removePlayer(this);
	}
}

auto Player::handleRequest(PacketReader &reader) -> void {
	try {
		header_t header = reader.getHeader();
		if (!m_isConnect) {
			// We don't want to accept any other packet than the one for loading the character
			if (header == CMSG_PLAYER_LOAD) {
				playerConnect(reader);
			}
		}
		else {
			switch (header) {
				case CMSG_ADMIN_COMMAND: CommandHandler::handleAdminCommand(this, reader); break;
				case CMSG_ADMIN_MESSENGER: PlayerHandler::handleAdminMessenger(this, reader); break;
				case CMSG_ATTACK_ENERGY_CHARGE: PlayerHandler::useEnergyChargeAttack(this, reader); break;
				case CMSG_ATTACK_MAGIC: PlayerHandler::useSpellAttack(this, reader); break;
				case CMSG_ATTACK_MELEE: PlayerHandler::useMeleeAttack(this, reader); break;
				case CMSG_ATTACK_RANGED: PlayerHandler::useRangedAttack(this, reader); break;
				case CMSG_BUDDY: BuddyListHandler::handleBuddyList(this, reader); break;
				case CMSG_CASH_ITEM_USE: InventoryHandler::useCashItem(this, reader); break;
				case CMSG_CASH_SHOP: send(PlayerPacket::sendBlockedMessage(PlayerPacket::BlockMessages::NoCashShop)); break;
				case CMSG_CHAIR: InventoryHandler::handleChair(this, reader); break;
				case CMSG_CHALKBOARD: sendMap(InventoryPacket::sendChalkboardUpdate(m_id, "")); setChalkboard(""); break;
				case CMSG_CHANNEL_CHANGE: changeChannel(reader.get<int8_t>()); break;
				case CMSG_COMMAND: CommandHandler::handleCommand(this, reader); break;
				case CMSG_DROP_MESOS: DropHandler::dropMesos(this, reader); break;
				case CMSG_EMOTE: PlayerHandler::handleFacialExpression(this, reader); break;
				case CMSG_FAME: Fame::handleFame(this, reader); break;
				case CMSG_FRIENDLY_MOB_DAMAGE: MobHandler::friendlyDamaged(this, reader); break;
				case CMSG_HAMMER: InventoryHandler::handleHammerTime(this); break;
				case CMSG_ITEM_CANCEL: InventoryHandler::cancelItem(this, reader); break;
				case CMSG_ITEM_EFFECT: InventoryHandler::useItemEffect(this, reader); break;
				case CMSG_ITEM_LOOT: DropHandler::lootItem(this, reader); break;
				case CMSG_ITEM_MOVE: InventoryHandler::itemMove(this, reader); break;
				case CMSG_ITEM_USE: InventoryHandler::useItem(this, reader); break;
				case CMSG_KEYMAP: changeKey(reader); break;
				case CMSG_MACRO_LIST: changeSkillMacros(reader); break;
				case CMSG_MAP_CHANGE: Maps::usePortal(this, reader); break;
				case CMSG_MAP_CHANGE_SPECIAL: Maps::useScriptedPortal(this, reader); break;
				case CMSG_MESSAGE_GROUP: ChatHandler::handleGroupChat(this, reader); break;
				case CMSG_MOB_CONTROL: MobHandler::monsterControl(this, reader); break;
				case CMSG_MOB_EXPLOSION: MobHandler::handleBomb(this, reader); break;
				case CMSG_MOB_TURNCOAT_DAMAGE: MobHandler::handleTurncoats(this, reader); break;
				case CMSG_MONSTER_BOOK: PlayerHandler::handleMonsterBook(this, reader); break;
				case CMSG_MTS: send(PlayerPacket::sendBlockedMessage(PlayerPacket::BlockMessages::MtsUnavailable)); break;
				case CMSG_MULTI_STAT_ADDITION: getStats()->addStatMulti(reader); break;
				case CMSG_MYSTIC_DOOR_ENTRY: PlayerHandler::handleDoorUse(this, reader); break;
				case CMSG_NPC_ANIMATE: NpcHandler::handleNpcAnimation(this, reader); break;
				case CMSG_NPC_TALK: NpcHandler::handleNpc(this, reader); break;
				case CMSG_NPC_TALK_CONT: NpcHandler::handleNpcIn(this, reader); break;
				case CMSG_PARTY: PartyHandler::handleRequest(this, reader); break;
				case CMSG_PET_CHAT: PetHandler::handleChat(this, reader); break;
				case CMSG_PET_COMMAND: PetHandler::handleCommand(this, reader); break;
				case CMSG_PET_CONSUME_POTION: PetHandler::handleConsumePotion(this, reader); break;
				case CMSG_PET_FOOD_USE: PetHandler::handleFeed(this, reader); break;
				case CMSG_PET_LOOT: DropHandler::petLoot(this, reader); break;
				case CMSG_PET_MOVEMENT: PetHandler::handleMovement(this, reader); break;
				case CMSG_PET_SUMMON: PetHandler::handleSummon(this, reader); break;
				case CMSG_PLAYER_CHAT: ChatHandler::handleChat(this, reader); break;
				case CMSG_PLAYER_DAMAGE: PlayerHandler::handleDamage(this, reader); break;
				case CMSG_PLAYER_HEAL: PlayerHandler::handleHeal(this, reader); break;
				case CMSG_PLAYER_INFO: PlayerHandler::handleGetInfo(this, reader); break;
				case CMSG_PLAYER_MOVE: PlayerHandler::handleMoving(this, reader); break;
				case CMSG_PLAYER_ROOM: TradeHandler::tradeHandler(this, reader); break;
				case CMSG_QUEST_OBTAIN: Quests::getQuest(this, reader); break;
				case CMSG_REACTOR_HIT: ReactorHandler::hitReactor(this, reader); break;
				case CMSG_REACTOR_TOUCH: ReactorHandler::touchReactor(this, reader); break;
				case CMSG_REVIVE_EFFECT: InventoryHandler::useItemEffect(this, reader); break;
				case CMSG_SCROLL_USE: InventoryHandler::useScroll(this, reader); break;
				case CMSG_SHOP: NpcHandler::useShop(this, reader); break;
				case CMSG_SKILL_ADD: Skills::addSkill(this, reader); break;
				case CMSG_SKILL_CANCEL: Skills::cancelSkill(this, reader); break;
				case CMSG_SKILL_USE: Skills::useSkill(this, reader); break;
				case CMSG_SKILLBOOK_USE: InventoryHandler::useSkillbook(this, reader); break;
				case CMSG_SPECIAL_SKILL: PlayerHandler::handleSpecialSkills(this, reader); break;
				case CMSG_STAT_ADDITION: getStats()->addStat(reader); break;
				case CMSG_STORAGE: NpcHandler::useStorage(this, reader); break;
				case CMSG_SUMMON_ATTACK: PlayerHandler::useSummonAttack(this, reader); break;
				case CMSG_SUMMON_BAG_USE: InventoryHandler::useSummonBag(this, reader); break;
				case CMSG_SUMMON_DAMAGE: SummonHandler::damageSummon(this, reader); break;
				case CMSG_SUMMON_MOVEMENT: SummonHandler::moveSummon(this, reader); break;
				case CMSG_TELEPORT_ROCK: InventoryHandler::handleRockFunctions(this, reader); break;
				case CMSG_TELEPORT_ROCK_USE: InventoryHandler::handleRockTeleport(this, Items::SpecialTeleportRock, reader); break;
				case CMSG_TOWN_SCROLL_USE: InventoryHandler::useReturnScroll(this, reader); break;
				case CMSG_USE_CHAIR: InventoryHandler::useChair(this, reader); break;
				case CMSG_USE_REWARD_ITEM: InventoryHandler::handleRewardItem(this, reader); break;
				case CMSG_USE_SCRIPT_ITEM: InventoryHandler::handleScriptItem(this, reader); break;
			}
		}
	}
	catch (const PacketContentException &e) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly

		reader.reset();
		ChannelServer::getInstance().log(LogType::MalformedPacket, [&](out_stream_t &log) {
			log << "Player ID: " << getId()
				<< "; Packet: " << reader
				<< "; Error: " << e.what();
		});
		disconnect();
	}
}

auto Player::playerConnect(PacketReader &reader) -> void {
	int32_t id = reader.get<int32_t>();
	if (Connectable::getInstance().checkPlayer(id, getIp()) == Result::Failure) {
		// Hacking
		disconnect();
		return;
	}

	m_id = id;
	soci::session &sql = Database::getCharDb();
	soci::row row;
	sql.once
		<< "SELECT c.*, u.gm_level, u.admin "
		<< "FROM characters c "
		<< "INNER JOIN user_accounts u ON c.user_id = u.user_id "
		<< "WHERE c.character_id = :char",
		soci::use(id, "char"),
		soci::into(row);

	if (!sql.got_data()) {
		// Hacking
		disconnect();
		return;
	}

	m_name = row.get<string_t>("name");
	m_userId = row.get<int32_t>("user_id");
	m_map = row.get<int32_t>("map");
	m_gmLevel = row.get<int32_t>("gm_level");
	m_admin = row.get<bool>("admin");
	m_eyes = row.get<int32_t>("eyes");
	m_hair = row.get<int32_t>("hair");
	m_worldId = row.get<world_id_t>("world_id");
	m_gender = row.get<int8_t>("gender");
	m_skin = row.get<int8_t>("skin");
	m_mapPos = row.get<int8_t>("pos");
	m_buddylistSize = row.get<uint8_t>("buddylist_size");

	// Stats
	m_stats.reset(
		new PlayerStats(
			this,
			row.get<uint8_t>("level"),
			row.get<int16_t>("job"),
			row.get<int16_t>("fame"),
			row.get<int16_t>("str"),
			row.get<int16_t>("dex"),
			row.get<int16_t>("int"),
			row.get<int16_t>("luk"),
			row.get<int16_t>("ap"),
			row.get<uint16_t>("hpmp_ap"),
			row.get<int16_t>("sp"),
			row.get<int16_t>("chp"),
			row.get<int16_t>("mhp"),
			row.get<int16_t>("cmp"),
			row.get<int16_t>("mmp"),
			row.get<int32_t>("exp")
		)
	);

	// Inventory
	m_mounts = make_owned_ptr<PlayerMounts>(this);
	m_pets = make_owned_ptr<PlayerPets>(this);
	array_t<uint8_t, Inventories::InventoryCount> maxSlots;
	maxSlots[0] = row.get<uint8_t>("equip_slots");
	maxSlots[1] = row.get<uint8_t>("use_slots");
	maxSlots[2] = row.get<uint8_t>("setup_slots");
	maxSlots[3] = row.get<uint8_t>("etc_slots");
	maxSlots[4] = row.get<uint8_t>("cash_slots");
	m_inventory = make_owned_ptr<PlayerInventory>(this, maxSlots, row.get<int32_t>("mesos"));
	m_storage = make_owned_ptr<PlayerStorage>(this);

	// Skills
	m_skills = make_owned_ptr<PlayerSkills>(this);

	// Buffs/summons
	m_activeBuffs = make_owned_ptr<PlayerActiveBuffs>(this);
	m_summons = make_owned_ptr<PlayerSummons>(this);

	bool firstConnect = true;
	if (PacketReader *pack = Connectable::getInstance().getPacket(id)) {
		// Packet transferring on channel switch
		firstConnect = false;
		setConnectionTime(pack->get<int64_t>());
		int32_t followId = pack->get<int32_t>();
		if (followId != 0) {
			if (Player *follow = PlayerDataProvider::getInstance().getPlayer(followId)) {
				PlayerDataProvider::getInstance().addFollower(this, follow);
			}
		}

		m_gmChat = pack->get<bool>();

		PlayerActiveBuffs *buffs = getActiveBuffs();
		buffs->read(*pack);
		if (buffs->hasHyperBody()) {
			int32_t skillId = buffs->getHyperBody();
			uint8_t hbLevel = buffs->getActiveSkillLevel(skillId);
			auto hb = SkillDataProvider::getInstance().getSkill(skillId, hbLevel);
			getStats()->setHyperBody(hb->x, hb->y);
		}

		getSummons()->read(*pack);
	}
	else {
		// No packet, that means that they're connecting for the first time
		setConnectionTime(time(nullptr));
		m_gmChat = true;
	}

	Connectable::getInstance().playerEstablished(id);

	// The rest
	m_variables = make_owned_ptr<PlayerVariables>(this);
	m_buddyList = make_owned_ptr<PlayerBuddyList>(this);
	m_quests = make_owned_ptr<PlayerQuests>(this);
	m_monsterBook = make_owned_ptr<PlayerMonsterBook>(this);

	opt_int32_t bookCover = row.get<opt_int32_t>("book_cover");
	getMonsterBook()->setCover(bookCover.getOrDefault(0));

	// Key Maps and Macros
	KeyMaps keyMaps;
	keyMaps.load(id);

	SkillMacros skillMacros;
	skillMacros.load(id);
	
	// Adjust down HP or MP if necessary
	getStats()->checkHpMp();

	if (isGm() || isAdmin()) {
		if (firstConnect) {
			m_map = Maps::GmMap;
			m_mapPos = -1;
		}
	}
	else if (Maps::getMap(m_map)->getForcedReturn() != Maps::NoMap) {
		m_map = Maps::getMap(m_map)->getForcedReturn();
		m_mapPos = -1;
	}
	else if (getStats()->isDead()) {
		m_map = Maps::getMap(m_map)->getReturnMap();
		m_mapPos = -1;
	}
	m_lastMap = m_map;

	m_pos = Maps::getMap(m_map)->getSpawnPoint(m_mapPos)->pos;
	m_stance = 0;
	m_foothold = 0;

	send(PlayerPacket::connectData(this));

	auto &config = ChannelServer::getInstance().getConfig();
	if (config.scrollingHeader.size() > 0) {
		send(ServerPacket::showScrollingHeader(config.scrollingHeader));
	}

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = getPets()->getSummoned(i)) {
			pet->setPos(Maps::getMap(m_map)->getSpawnPoint(m_mapPos)->pos);
		}
	}

	send(PlayerPacket::showKeys(&keyMaps));

	send(BuddyListPacket::update(this, BuddyListPacket::ActionTypes::Add));
	getBuddyList()->checkForPendingBuddy();

	send(PlayerPacket::showSkillMacros(&skillMacros));

	PlayerDataProvider::getInstance().addPlayer(this);
	Maps::addPlayer(this, m_map);

	ChannelServer::getInstance().log(LogType::Info, [&](out_stream_t &log) { log << m_name << " (" << m_id << ") connected from " << getIp(); });

	setOnline(true);
	m_isConnect = true;

	PlayerData data;
	const PlayerData * const existingData = PlayerDataProvider::getInstance().getPlayerData(m_id);
	bool firstConnectionSinceServerStarted = firstConnect && !existingData->initialized;

	if (firstConnectionSinceServerStarted) {
		data.admin = m_admin;
		data.level = getStats()->getLevel();
		data.job = getStats()->getJob();
		data.gmLevel = m_gmLevel;
		data.name = m_name;
	}

	data.channel = ChannelServer::getInstance().getChannelId();
	data.map = m_map;
	data.id = m_id;
	data.ip = getIp();

	ChannelServer::getInstance().sendWorld(SyncPacket::PlayerPacket::connect(data, firstConnectionSinceServerStarted));
	ChannelServer::getInstance().sendWorld(SyncPacket::BuddyPacket::buddyOnline(getId(), getBuddyList()->getBuddyIds(), true));
}

auto Player::getMap() const -> Map * {
	return Maps::getMap(getMapId());
}

auto Player::setMap(int32_t mapId, PortalInfo *portal, bool instance) -> void {
	if (!Maps::getMap(mapId)) {
		send(MapPacket::portalBlocked());
		return;
	}
	Map *oldMap = Maps::getMap(m_map);
	Map *newMap = Maps::getMap(mapId);

	if (portal == nullptr) {
		portal = newMap->getSpawnPoint();
	}

	if (!instance) {
		// Only trigger the message for natural map changes not caused by moveAllPlayers, etc.
		int32_t isPartyLeader = (getParty() != nullptr && getParty()->isLeader(getId()) ? 1 : 0);
		if (Instance *i = oldMap->getInstance()) {
			i->sendMessage(InstanceMessage::PlayerChangeMap, getId(), mapId, m_map, isPartyLeader);
		}
		if (Instance *i = newMap->getInstance()) {
			i->sendMessage(InstanceMessage::PlayerChangeMap, getId(), mapId, m_map, isPartyLeader);
		}
	}

	oldMap->removePlayer(this);
	m_lastMap = m_map;
	m_map = mapId;
	m_mapPos = portal->id;
	m_usedPortals.clear();
	setPos(Pos(portal->pos.x, portal->pos.y - 40));
	setStance(0);
	setFoothold(0);
	setFallCounter(0);

	// Prevent chair Denial of Service
	if (getMapChair() != 0) {
		oldMap->playerSeated(getMapChair(), nullptr);
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
		SummonHandler::removeSummon(this, true, false, SummonMessages::None);
	}
	if (getSummons()->getSummon() != nullptr && getSummons()->getSummon()->getType() == Summon::Static) {
		SummonHandler::removeSummon(this, false, false, SummonMessages::None);
	}

	if (getActiveBuffs()->hasMarkedMonster()) {
		Buffs::endBuff(this, getActiveBuffs()->getHomingBeacon());
	}
	if (!getChalkboard().empty() && !newMap->canChalkboard()) {
		setChalkboard("");
	}

	send(MapPacket::changeMap(this));
	Maps::addPlayer(this, mapId);

	PlayerDataProvider::getInstance().updatePlayerMap(this);
}

auto Player::getMedalName() -> string_t {
	out_stream_t ret;
	if (int32_t itemId = getInventory()->getEquippedId(EquipSlots::Medal)) {
		// Check if there's an item at that slot
		ret << "<" << ItemDataProvider::getInstance().getItemInfo(itemId)->name << "> ";
	}
	ret << getName();
	return ret.str();
}

auto Player::changeChannel(channel_id_t channel) -> void {
	ChannelServer::getInstance().sendWorld(SyncPacket::PlayerPacket::changeChannel(this, channel));
}

auto Player::changeKey(PacketReader &reader) -> void {
	int32_t mode = reader.get<int32_t>();
	int32_t howMany = reader.get<int32_t>();

	enum KeyModes : int32_t {
		ChangeKeys = 0x00,
		AutoHpPotion = 0x01,
		AutoMpPotion = 0x02
	};

	if (mode == ChangeKeys) {
		if (howMany == 0) {
			return;
		}

		KeyMaps keyMaps; // We don't need old values here because it is only used to save the new values
		for (int32_t i = 0; i < howMany; i++) {
			int32_t pos = reader.get<int32_t>();
			int8_t type = reader.get<int8_t>();
			int32_t action = reader.get<int32_t>();
			keyMaps.add(pos, KeyMaps::KeyMap(type, action));
		}

		// Update to MySQL
		keyMaps.save(getId());
	}
	else if (mode == AutoHpPotion) {
		getInventory()->setAutoHpPot(howMany);
	}
	else if (mode == AutoMpPotion) {
		getInventory()->setAutoMpPot(howMany);
	}
}

auto Player::changeSkillMacros(PacketReader &reader) -> void {
	uint8_t num = reader.get<int8_t>();
	if (num == 0) {
		return;
	}
	SkillMacros skillMacros;
	for (uint8_t i = 0; i < num; i++) {
		string_t name = reader.getString();
		bool shout = reader.get<bool>();
		int32_t skill1 = reader.get<int32_t>();
		int32_t skill2 = reader.get<int32_t>();
		int32_t skill3 = reader.get<int32_t>();

		skillMacros.add(i, new SkillMacros::SkillMacro(name, shout, skill1, skill2, skill3));
	}
	skillMacros.save(getId());
}

auto Player::setHair(int32_t id) -> void {
	m_hair = id;
	send(PlayerPacket::updateStat(Stats::Hair, id));
}

auto Player::setEyes(int32_t id) -> void {
	m_eyes = id;
	send(PlayerPacket::updateStat(Stats::Eyes, id));
}

auto Player::setSkin(int8_t id) -> void {
	m_skin = id;
	send(PlayerPacket::updateStat(Stats::Skin, id));
}

auto Player::saveStats() -> void {
	PlayerStats *s = getStats();
	PlayerInventory *i = getInventory();
	// Need local bindings
	// Stats
	uint8_t level = s->getLevel();
	int16_t job = s->getJob();
	int16_t str = s->getStr();
	int16_t dex = s->getDex();
	int16_t intt = s->getInt();
	int16_t luk = s->getLuk();
	int16_t hp = s->getHp();
	int16_t maxHp = s->getMaxHp(true);
	int16_t mp = s->getMp();
	int16_t maxMp = s->getMaxMp(true);
	uint16_t hpMpAp = s->getHpMpAp();
	int16_t ap = s->getAp();
	int16_t sp = s->getSp();
	int16_t fame = s->getFame();
	int32_t exp = s->getExp();
	// Inventory
	uint8_t equip = i->getMaxSlots(Inventories::EquipInventory);
	uint8_t use = i->getMaxSlots(Inventories::UseInventory);
	uint8_t setup = i->getMaxSlots(Inventories::SetupInventory);
	uint8_t etc = i->getMaxSlots(Inventories::EtcInventory);
	uint8_t cash = i->getMaxSlots(Inventories::CashInventory);
	int32_t money = i->getMesos();
	// Other
	int32_t rawCover = getMonsterBook()->getCover();
	opt_int32_t cover;
	if (rawCover != 0) {
		cover = rawCover;
	}

	Database::getCharDb().once
		<< "UPDATE characters "
		<< "SET "
		<< "	level = :level, "
		<< "	job = :job, "
		<< "	str = :str, "
		<< "	dex = :dex, "
		<< "	`int` = :int, "
		<< "	luk = :luk, "
		<< "	chp = :hp, "
		<< "	mhp = :maxhp, "
		<< "	cmp = :mp, "
		<< "	mmp = :maxmp, "
		<< "	hpmp_ap = :hpmpap, "
		<< "	ap = :ap, "
		<< "	sp = :sp, "
		<< "	exp = :exp, "
		<< "	fame = :fame, "
		<< "	map = :map, "
		<< "	pos = :pos, "
		<< "	gender = :gender, "
		<< "	skin = :skin, "
		<< "	eyes = :eyes, "
		<< "	hair = :hair, "
		<< "	mesos = :money, "
		<< "	equip_slots = :equip, "
		<< "	use_slots = :use, "
		<< "	setup_slots = :setup, "
		<< "	etc_slots = :etc, "
		<< "	cash_slots = :cash, "
		<< "	buddylist_size = :buddylist, "
		<< "	book_cover = :cover "
		<< "WHERE character_id = :char",
		soci::use(m_id, "char"),
		soci::use(level, "level"),
		soci::use(job, "job"),
		soci::use(str, "str"),
		soci::use(dex, "dex"),
		soci::use(intt, "int"),
		soci::use(luk, "luk"),
		soci::use(hp, "hp"),
		soci::use(maxHp, "maxhp"),
		soci::use(mp, "mp"),
		soci::use(maxMp, "maxmp"),
		soci::use(hpMpAp, "hpmpap"),
		soci::use(ap, "ap"),
		soci::use(sp, "sp"),
		soci::use(exp, "exp"),
		soci::use(fame, "fame"),
		soci::use(m_map, "map"),
		soci::use(m_mapPos, "pos"),
		soci::use(m_gender, "gender"),
		soci::use(m_skin, "skin"),
		soci::use(m_eyes, "eyes"),
		soci::use(m_hair, "hair"),
		soci::use(money, "money"),
		soci::use(equip, "equip"),
		soci::use(use, "use"),
		soci::use(setup, "setup"),
		soci::use(etc, "etc"),
		soci::use(cash, "cash"),
		soci::use(m_buddylistSize, "buddylist"),
		soci::use(cover, "cover");
}

auto Player::saveAll(bool saveCooldowns) -> void {
	saveStats();
	getInventory()->save();
	getStorage()->save();
	getMonsterBook()->save();
	getMounts()->save();
	getPets()->save();
	getQuests()->save();
	getSkills()->save(saveCooldowns);
	getVariables()->save();
}

auto Player::setOnline(bool online) -> void {
	int32_t onlineId = online ? ChannelServer::getInstance().getOnlineId() : 0;
	Database::getCharDb().once
		<< "UPDATE user_accounts u "
		<< "INNER JOIN characters c ON u.user_id = c.user_id "
		<< "SET "
		<< "	u.online = :onlineId, "
		<< "	c.online = :online "
		<< "WHERE c.character_id = :char",
		soci::use(m_id, "char"),
		soci::use(online, "online"),
		soci::use(onlineId, "onlineId");
}

auto Player::setLevelDate() -> void {
	Database::getCharDb().once << "UPDATE characters c SET c.time_level = NOW() WHERE c.character_id = :char", soci::use(m_id, "char");
}

auto Player::acceptDeath(bool wheel) -> void {
	int32_t toMap = Maps::getMap(m_map) ? Maps::getMap(m_map)->getReturnMap() : m_map;
	if (wheel) {
		toMap = getMapId();
	}
	getActiveBuffs()->removeBuffs();
	getStats()->checkHpMp();
	getStats()->setHp(50, false);
	setMap(toMap);
}

auto Player::hasGmEquip() const -> bool {
	auto equippedUtility = [this](int16_t slot, int32_t itemId) -> bool {
		return this->getInventory()->getEquippedId(slot) == itemId;
	};

	if (equippedUtility(EquipSlots::Helm, Items::GmHat)) {
		return true;
	}
	if (equippedUtility(EquipSlots::Top, Items::GmTop)) {
		return true;
	}
	if (equippedUtility(EquipSlots::Bottom, Items::GmBottom)) {
		return true;
	}
	if (equippedUtility(EquipSlots::Weapon, Items::GmWeapon)) {
		return true;
	}
	return false;
}

auto Player::isUsingGmHide() const -> bool {
	return m_activeBuffs->isUsingGmHide();
}

auto Player::hasGmBenefits() const -> bool {
	return isUsingGmHide() || hasGmEquip();
}

auto Player::setBuddyListSize(uint8_t size) -> void {
	m_buddylistSize = size;
	send(BuddyListPacket::showSize(this));
}

auto Player::getPortalCount(bool add) -> uint8_t {
	if (add) {
		m_portalCount++;
	}
	return m_portalCount;
}

auto Player::initializeRng(PacketBuilder &packet) -> void {
	uint32_t seed1 = Randomizer::rand<uint32_t>();
	uint32_t seed2 = Randomizer::rand<uint32_t>();
	uint32_t seed3 = Randomizer::rand<uint32_t>();

	m_randStream = make_owned_ptr<TauswortheGenerator>(seed1, seed2, seed3);

	packet.add<uint32_t>(seed1);
	packet.add<uint32_t>(seed2);
	packet.add<uint32_t>(seed3);
}

auto Player::send(const PacketBuilder &builder) -> void {
	AbstractConnection::send(builder);
}

auto Player::send(const SplitPacketBuilder &builder) -> void {
	send(builder.player);
}

auto Player::sendMap(const PacketBuilder &builder, bool excludeSelf) -> void {
	getMap()->send(builder, excludeSelf ? this : nullptr);
}

auto Player::sendMap(const SplitPacketBuilder &builder) -> void {
	getMap()->send(builder, this);
}