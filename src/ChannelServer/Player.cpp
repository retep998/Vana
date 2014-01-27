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
#include "PacketReader.hpp"
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
			party->setMember(getId(), nullptr);
			isLeader = getParty()->isLeader(getId()) ? 1 : 0;
		}
		if (Instance *instance = getInstance()) {
			instance->removePlayer(getId());
			instance->sendMessage(PlayerDisconnect, getId(), isLeader);
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
				SyncPacket::BuddyPacket::buddyOnline(getId(), getBuddyList()->getBuddyIds(), false);
			}
			// Do not connect to worldserver if the worldserver has disconnected
			SyncPacket::PlayerPacket::disconnect(getId());
		}
		PlayerDataProvider::getInstance().removePlayer(this);
	}
}

auto Player::handleRequest(PacketReader &packet) -> void {
	try {
		header_t header = packet.getHeader();
		if (!m_isConnect) {
			// We don't want to accept any other packet than the one for loading the character
			if (header == CMSG_PLAYER_LOAD) {
				playerConnect(packet);
			}
		}
		else {
			switch (header) {
				case CMSG_ADMIN_COMMAND: CommandHandler::handleAdminCommand(this, packet); break;
				case CMSG_ADMIN_MESSENGER: PlayerHandler::handleAdminMessenger(this, packet); break;
				case CMSG_ATTACK_ENERGY_CHARGE: PlayerHandler::useEnergyChargeAttack(this, packet); break;
				case CMSG_ATTACK_MAGIC: PlayerHandler::useSpellAttack(this, packet); break;
				case CMSG_ATTACK_MELEE: PlayerHandler::useMeleeAttack(this, packet); break;
				case CMSG_ATTACK_RANGED: PlayerHandler::useRangedAttack(this, packet); break;
				case CMSG_BUDDY: BuddyListHandler::handleBuddyList(this, packet); break;
				case CMSG_CASH_ITEM_USE: InventoryHandler::useCashItem(this, packet); break;
				case CMSG_CASH_SHOP: PlayerPacket::sendBlockedMessage(this, PlayerPacket::BlockMessages::NoCashShop); break;
				case CMSG_CHAIR: InventoryHandler::handleChair(this, packet); break;
				case CMSG_CHALKBOARD: InventoryPacket::sendChalkboardUpdate(this); setChalkboard(""); break;
				case CMSG_CHANNEL_CHANGE: changeChannel(packet.get<int8_t>()); break;
				case CMSG_COMMAND: CommandHandler::handleCommand(this, packet); break;
				case CMSG_DROP_MESOS: DropHandler::dropMesos(this, packet); break;
				case CMSG_EMOTE: PlayerHandler::handleFacialExpression(this, packet); break;
				case CMSG_FAME: Fame::handleFame(this, packet); break;
				case CMSG_FRIENDLY_MOB_DAMAGE: MobHandler::friendlyDamaged(this, packet); break;
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
				case CMSG_MTS: PlayerPacket::sendBlockedMessage(this, PlayerPacket::BlockMessages::MtsUnavailable); break;
				case CMSG_MULTI_STAT_ADDITION: getStats()->addStatMulti(packet); break;
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
				case CMSG_STAT_ADDITION: getStats()->addStat(packet); break;
				case CMSG_STORAGE: NpcHandler::useStorage(this, packet); break;
				case CMSG_SUMMON_ATTACK: PlayerHandler::useSummonAttack(this, packet); break;
				case CMSG_SUMMON_BAG_USE: InventoryHandler::useSummonBag(this, packet); break;
				case CMSG_SUMMON_DAMAGE: SummonHandler::damageSummon(this, packet); break;
				case CMSG_SUMMON_MOVEMENT: SummonHandler::moveSummon(this, packet); break;
				case CMSG_TELEPORT_ROCK: InventoryHandler::handleRockFunctions(this, packet); break;
				case CMSG_TELEPORT_ROCK_USE: InventoryHandler::handleRockTeleport(this, Items::SpecialTeleportRock, packet); break;
				case CMSG_TOWN_SCROLL_USE: InventoryHandler::useReturnScroll(this, packet); break;
				case CMSG_USE_CHAIR: InventoryHandler::useChair(this, packet); break;
				case CMSG_USE_REWARD_ITEM: InventoryHandler::handleRewardItem(this, packet); break;
				case CMSG_USE_SCRIPT_ITEM: InventoryHandler::handleScriptItem(this, packet); break;
			}
		}
	}
	catch (const PacketContentException &e) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly

		packet.reset();
		out_stream_t x;
		x << "Player ID: " << getId() << "; Packet: " << packet << "; Error: " << e.what();
		ChannelServer::getInstance().log(LogTypes::MalformedPacket, x.str());
		getSession()->disconnect();
	}
}

auto Player::playerConnect(PacketReader &packet) -> void {
	int32_t id = packet.get<int32_t>();
	if (!Connectable::getInstance().checkPlayer(id, getIp())) {
		// Hacking
		getSession()->disconnect();
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
		getSession()->disconnect();
		return;
	}

	m_name = row.get<string_t>("name");
	m_userId = row.get<int32_t>("user_id");
	m_map = row.get<int32_t>("map");
	m_gmLevel = row.get<int32_t>("gm_level");
	m_admin = row.get<bool>("admin");
	m_eyes = row.get<int32_t>("eyes");
	m_hair = row.get<int32_t>("hair");
	m_worldId = row.get<int8_t>("world_id");
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

	bool checked = false;
	if (PacketReader *pack = Connectable::getInstance().getPacket(id)) {
		// Packet transferring on channel switch
		checked = true;
		setConnectionTime(pack->get<int64_t>());

		PlayerActiveBuffs *buffs = getActiveBuffs();
		buffs->read(*pack);
		if (buffs->hasHyperBody()) {
			int32_t skillId = buffs->getHyperBody();
			uint8_t hbLevel = buffs->getActiveSkillLevel(skillId);
			SkillLevelInfo *hb = SkillDataProvider::getInstance().getSkill(skillId, hbLevel);
			getStats()->setHyperBody(hb->x, hb->y);
		}

		getSummons()->read(*pack);
	}
	else {
		// No packet, that means that they're connecting for the first time
		setConnectionTime(time(nullptr));
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
		if (!checked) {
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

	PlayerPacket::connectData(this);

	if (ChannelServer::getInstance().getScrollingHeader().size() > 0) {
		ServerPacket::showScrollingHeader(this, ChannelServer::getInstance().getScrollingHeader());
	}

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = getPets()->getSummoned(i)) {
			pet->setPos(Maps::getMap(m_map)->getSpawnPoint(m_mapPos)->pos);
		}
	}

	PlayerPacket::showKeys(this, &keyMaps);

	BuddyListPacket::update(this, BuddyListPacket::ActionTypes::Add);
	getBuddyList()->checkForPendingBuddy();

	PlayerPacket::showSkillMacros(this, &skillMacros);

	Maps::addPlayer(this, m_map);

	out_stream_t x;
	x << getName() << " (" << getId() << ") connected from " << getIp().toString();
	ChannelServer::getInstance().log(LogTypes::Info, x.str());

	setOnline(true);
	m_isConnect = true;
	SyncPacket::PlayerPacket::connect(this);
	SyncPacket::BuddyPacket::buddyOnline(getId(), getBuddyList()->getBuddyIds(), true);
}

auto Player::getMap() const -> Map * {
	return Maps::getMap(getMapId());
}

auto Player::setMap(int32_t mapId, PortalInfo *portal, bool instance) -> void {
	if (!Maps::getMap(mapId)) {
		MapPacket::portalBlocked(this);
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
			i->sendMessage(PlayerChangeMap, getId(), mapId, m_map, isPartyLeader);
		}
		if (Instance *i = newMap->getInstance()) {
			i->sendMessage(PlayerChangeMap, getId(), mapId, m_map, isPartyLeader);
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
	SyncPacket::PlayerPacket::updateMap(getId(), mapId);
	MapPacket::changeMap(this);
	Maps::addPlayer(this, mapId);
}

auto Player::getMedalName() -> string_t {
	out_stream_t ret;
	if (int32_t itemId = getInventory()->getEquippedId(EquipSlots::Medal)) {
		// Check if there's an item at that slot
		ret << "<" << ItemDataProvider::getInstance().getItemName(itemId) << "> ";
	}
	ret << getName();
	return ret.str();
}

auto Player::changeChannel(int8_t channel) -> void {
	SyncPacket::PlayerPacket::changeChannel(this, channel);
}

auto Player::changeKey(PacketReader &packet) -> void {
	int32_t mode = packet.get<int32_t>();
	int32_t howMany = packet.get<int32_t>();

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
			int32_t pos = packet.get<int32_t>();
			int8_t type = packet.get<int8_t>();
			int32_t action = packet.get<int32_t>();
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

auto Player::changeSkillMacros(PacketReader &packet) -> void {
	uint8_t num = packet.get<int8_t>();
	if (num == 0) {
		return;
	}
	SkillMacros skillMacros;
	for (uint8_t i = 0; i < num; i++) {
		const string_t &name = packet.getString();
		bool shout = packet.get<bool>();
		int32_t skill1 = packet.get<int32_t>();
		int32_t skill2 = packet.get<int32_t>();
		int32_t skill3 = packet.get<int32_t>();

		skillMacros.add(i, new SkillMacros::SkillMacro(name, shout, skill1, skill2, skill3));
	}
	skillMacros.save(getId());
}

auto Player::setHair(int32_t id) -> void {
	m_hair = id;
	PlayerPacket::updateStat(this, Stats::Hair, id);
}

auto Player::setEyes(int32_t id) -> void {
	m_eyes = id;
	PlayerPacket::updateStat(this, Stats::Eyes, id);
}

auto Player::setSkin(int8_t id) -> void {
	m_skin = id;
	PlayerPacket::updateStat(this, Stats::Skin, id);
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
	BuddyListPacket::showSize(this);
}

auto Player::getPortalCount(bool add) -> uint8_t {
	if (add) {
		m_portalCount++;
	}
	return m_portalCount;
}

auto Player::initializeRng(PacketCreator &packet) -> void {
	uint32_t seed1 = Randomizer::rand<uint32_t>();
	uint32_t seed2 = Randomizer::rand<uint32_t>();
	uint32_t seed3 = Randomizer::rand<uint32_t>();

	m_randStream = make_owned_ptr<TauswortheGenerator>(seed1, seed2, seed3);

	packet.add<uint32_t>(seed1);
	packet.add<uint32_t>(seed2);
	packet.add<uint32_t>(seed3);
}