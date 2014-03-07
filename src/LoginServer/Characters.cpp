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
#include "Characters.hpp"
#include "Algorithm.hpp"
#include "CurseDataProvider.hpp"
#include "ClientIp.hpp"
#include "Database.hpp"
#include "EquipDataProvider.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "ItemConstants.hpp"
#include "LoginPacket.hpp"
#include "LoginServer.hpp"
#include "LoginServerAcceptPacket.hpp"
#include "MiscUtilities.hpp"
#include "PacketReader.hpp"
#include "Session.hpp"
#include "SyncPacket.hpp"
#include "UserConnection.hpp"
#include "World.hpp"
#include "Worlds.hpp"
#include "ValidCharDataProvider.hpp"
#include <unordered_map>

auto Characters::loadEquips(player_id_t id, vector_t<CharEquip> &vec) -> void {
	soci::rowset<> rs = (Database::getCharDb().prepare
		<< "SELECT i.item_id, i.slot "
		<< "FROM " << Database::makeCharTable("items") << " i "
		<< "WHERE "
		<< "	i.character_id = :id "
		<< "	AND i.inv = :inv "
		<< "	AND i.slot < 0 "
		<< "ORDER BY slot ASC",
		soci::use(id, "id"),
		soci::use(Inventories::EquipInventory, "inv"));

	for (const auto &row : rs) {
		CharEquip equip;
		equip.id = row.get<item_id_t>("item_id");
		equip.slot = row.get<inventory_slot_t>("slot");
		vec.push_back(equip);
	}
}

auto Characters::loadCharacter(Character &charc, const soci::row &row) -> void {
	charc.id = row.get<player_id_t>("character_id");
	charc.name = row.get<string_t>("name");
	charc.gender = row.get<gender_id_t>("gender");
	charc.skin = row.get<skin_id_t>("skin");
	charc.eyes = row.get<face_id_t>("eyes");
	charc.hair = row.get<hair_id_t>("hair");
	charc.level = row.get<player_level_t>("level");
	charc.job = row.get<job_id_t>("job");
	charc.str = row.get<int16_t>("str");
	charc.dex = row.get<int16_t>("dex");
	charc.intt = row.get<int16_t>("int");
	charc.luk = row.get<int16_t>("luk");
	charc.hp = row.get<int16_t>("chp");
	charc.mhp = row.get<int16_t>("mhp");
	charc.mp = row.get<int16_t>("cmp");
	charc.mmp = row.get<int16_t>("mmp");
	charc.ap = row.get<int16_t>("ap");
	charc.sp = row.get<int16_t>("sp");
	charc.exp = row.get<experience_t>("exp");
	charc.fame = row.get<fame_t>("fame");
	charc.map = row.get<map_id_t>("map");
	charc.pos = row.get<int8_t>("pos");

	if (GameLogicUtilities::getJobTrack(charc.job) == Jobs::JobTracks::Gm) {
		// GMs can't have their rank sent otherwise the client will crash
		charc.worldRank = 0;
		charc.worldRankChange = 0;
		charc.jobRank = 0;
		charc.jobRankChange = 0;
	}
	else {
		charc.worldRank = row.get<int32_t>("world_cpos");
		charc.worldRankChange = charc.worldRank - row.get<int32_t>("world_opos");
		charc.jobRank = row.get<int32_t>("job_cpos");
		charc.jobRankChange = charc.jobRank - row.get<int32_t>("job_opos");
	}
	loadEquips(charc.id, charc.equips);
}

auto Characters::showAllCharacters(UserConnection *user) -> void {
	soci::rowset<> rs = (Database::getCharDb().prepare
		<< "SELECT * "
		<< "FROM " << Database::makeCharTable("characters") << " c "
		<< "WHERE c.user_id = :user ",
		soci::use(user->getUserId(), "user"));

	hash_map_t<world_id_t, vector_t<Character>> chars;
	uint32_t charsNum = 0;
	World *world;

	for (const auto &row : rs) {
		world_id_t worldId = row.get<world_id_t>("world_id");
		world = Worlds::getInstance().getWorld(worldId);
		if (world == nullptr || !world->isConnected()) {
			// World is not connected
			continue;
		}

		Character charc;
		loadCharacter(charc, row);
		chars[worldId].push_back(charc);
		charsNum++;
	}

	uint32_t unk = charsNum + (3 - charsNum % 3); // What I've observed
	user->send(LoginPacket::showAllCharactersInfo(chars.size(), unk));
	for (const auto &kvp : chars) {
		user->send(LoginPacket::showViewAllCharacters(kvp.first, kvp.second));
	}
}

auto Characters::showCharacters(UserConnection *user) -> void {
	soci::session &sql = Database::getCharDb();
	world_id_t worldId = user->getWorldId();
	account_id_t userId = user->getUserId();

	soci::rowset<> rs = (sql.prepare
		<< "SELECT * "
		<< "FROM " << Database::makeCharTable("characters") << " c "
		<< "WHERE c.user_id = :user AND c.world_id = :world ",
		soci::use(userId, "user"),
		soci::use(worldId, "world"));

	vector_t<Character> chars;
	for (const auto &row : rs) {
		Character charc;
		loadCharacter(charc, row);
		chars.push_back(charc);
	}

	opt_int32_t max;
	sql.once
		<< "SELECT s.char_slots "
		<< "FROM " << Database::makeCharTable("storage") << " s "
		<< "WHERE s.user_id = :user AND s.world_id = :world ",
		soci::use(userId, "user"),
		soci::use(worldId, "world"),
		soci::into(max);

	if (!sql.got_data() || !max.is_initialized()) {
		const WorldConfig &world = Worlds::getInstance().getWorld(worldId)->getConfig();
		max = world.defaultChars;
	}

	user->send(LoginPacket::showCharacters(chars, max.get()));
}

auto Characters::checkCharacterName(UserConnection *user, PacketReader &reader) -> void {
	string_t name = reader.get<string_t>();
	if (!ext::in_range_inclusive<size_t>(name.size(), Characters::MinNameSize, Characters::MaxNameSize)) {
		return;
	}

	if (nameInvalid(name)) {
		user->send(LoginPacket::checkName(name, LoginPacket::CheckNameErrors::Invalid));
	}
	else if (nameTaken(name)) {
		user->send(LoginPacket::checkName(name, LoginPacket::CheckNameErrors::Taken));
	}
	else {
		user->send(LoginPacket::checkName(name, LoginPacket::CheckNameErrors::None));
	}
}

auto Characters::createItem(item_id_t itemId, UserConnection *user, player_id_t charId, inventory_slot_t slot, slot_qty_t amount) -> void {
	soci::session &sql = Database::getCharDb();
	inventory_t inventory = GameLogicUtilities::getInventory(itemId);
	ItemDbInformation info(slot, charId, user->getUserId(), user->getWorldId(), Item::Inventory);

	if (inventory == Inventories::EquipInventory) {
		Item equip(itemId, Items::StatVariance::None, false);
		equip.databaseInsert(sql, info);
	}
	else {
		Item item(itemId, amount);
		item.databaseInsert(sql, info);
	}
}

auto Characters::createCharacter(UserConnection *user, PacketReader &reader) -> void {
	if (user->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}

	string_t name = reader.get<string_t>();
	if (!ext::in_range_inclusive<size_t>(name.size(), Characters::MinNameSize, Characters::MaxNameSize)) {
		return;
	}

	// Let's check our char name again just to be sure
	if (nameInvalid(name)) {
		user->send(LoginPacket::checkName(name, LoginPacket::CheckNameErrors::Invalid));
		return;
	}
	if (nameTaken(name)) {
		user->send(LoginPacket::checkName(name, LoginPacket::CheckNameErrors::Taken));
		return;
	}

	face_id_t face = reader.get<face_id_t>();
	hair_id_t hair = reader.get<hair_id_t>();
	hair_id_t hairColor = reader.get<hair_id_t>();
	skin_id_t skin = static_cast<skin_id_t>(reader.get<int32_t>());
	item_id_t top = reader.get<item_id_t>();
	item_id_t bottom = reader.get<item_id_t>();
	item_id_t shoes = reader.get<item_id_t>();
	item_id_t weapon = reader.get<item_id_t>();
	gender_id_t gender = reader.get<gender_id_t>();

	if (!ValidCharDataProvider::getInstance().isValidCharacter(gender, hair, hairColor, face, skin, top, bottom, shoes, weapon, ValidCharDataProvider::Adventurer)) {
		// Hacking
		user->disconnect();
		return;
	}

	uint16_t str = 12;
	uint16_t dex = 5;
	uint16_t intt = 4;
	uint16_t luk = 4;

	soci::session &sql = Database::getCharDb();

	sql.once
		<< "INSERT INTO " << Database::makeCharTable("characters") << " (name, user_id, world_id, eyes, hair, skin, gender, str, dex, `int`, luk) "
		<< "VALUES (:name, :user, :world, :eyes, :hair, :skin, :gender, :str, :dex, :int, :luk)",
		soci::use(name, "name"),
		soci::use(user->getUserId(), "user"),
		soci::use(user->getWorldId(), "world"),
		soci::use(face, "eyes"),
		soci::use(hair + hairColor, "hair"),
		soci::use(skin, "skin"),
		soci::use(gender, "gender"),
		soci::use(str, "str"),
		soci::use(dex, "dex"),
		soci::use(intt, "int"),
		soci::use(luk, "luk");

	player_id_t id = Database::getLastId<player_id_t>(sql);

	createItem(top, user, id, -EquipSlots::Top);
	createItem(bottom, user, id, -EquipSlots::Bottom);
	createItem(shoes, user, id, -EquipSlots::Shoe);
	createItem(weapon, user, id, -EquipSlots::Weapon);
	createItem(Items::BeginnersGuidebook, user, id, 1);

	soci::row row;
	sql.once
		<< "SELECT * "
		<< "FROM " << Database::makeCharTable("characters") << " c "
		<< "WHERE c.character_id = :id",
		soci::use(id, "id"),
		soci::into(row);

	Character charc;
	loadCharacter(charc, row);
	user->send(LoginPacket::showCharacter(charc));
	Worlds::getInstance().send(user->getWorldId(), SyncPacket::PlayerPacket::characterCreated(id));
}

auto Characters::deleteCharacter(UserConnection *user, PacketReader &reader) -> void {
	if (user->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	int32_t password = reader.get<int32_t>();
	player_id_t id = reader.get<player_id_t>();

	if (!ownerCheck(user, id)) {
		// Hacking
		return;
	}
	enum DeletionConstants : int8_t {
		Success = 0x00,
		IncorrectBirthday = 0x12,
		NoGuildMaster = 0x16,
		NoProposingChumps = 0x18,
		NoWorldTransfers = 0x1A
	};

	uint8_t result = Success;
	soci::session &sql = Database::getCharDb();
	optional_t<world_id_t> worldId;

	sql.once
		<< "SELECT world_id "
		<< "FROM " << Database::makeCharTable("characters") << " c "
		<< "WHERE c.character_id = :char ",
		soci::use(id, "char"),
		soci::into(worldId);

	if (!sql.got_data() || !worldId.is_initialized()) {
		// ???
		return;
	}

	bool success = false;
	opt_int32_t delPassword = user->getCharDeletePassword();
	if (!delPassword.is_initialized() || delPassword.get() == password) {
		Worlds::getInstance().runFunction([&id, &worldId](World *world) -> bool {
			if (world->isConnected() && world->getId() == worldId.get()) {
				// LoginServerAcceptPacket::removeCharacter(world->getConnection(), playerId);
				// For guilds
				return true;
			}
			return false;
		});

		sql.once << "DELETE p FROM " << Database::makeCharTable("pets") << " p INNER JOIN " << Database::makeCharTable("items") << " i ON p.pet_id = i.pet_id WHERE i.character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM " << Database::makeCharTable("characters") << " WHERE character_id = :char ", soci::use(id, "char");
	}
	else {
		result = IncorrectBirthday;
	}

	user->send(LoginPacket::deleteCharacter(id, result));
	Worlds::getInstance().send(user->getWorldId(), SyncPacket::PlayerPacket::characterDeleted(id));
}

auto Characters::connectGame(UserConnection *user, player_id_t charId) -> void {
	if (user->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	if (!ownerCheck(user, charId)) {
		// Hacking
		return;
	}

	auto world = Worlds::getInstance().getWorld(user->getWorldId());
	world->send(LoginServerAcceptPacket::playerConnectingToChannel(user->getChannel(), charId, user->getIp()));

	Ip chanIp(0);
	port_t port = -1;
	if (Channel *channel = world->getChannel(user->getChannel())) {
		chanIp = channel->matchIpToSubnet(user->getIp());
		port = channel->getPort();
	}
	ClientIp ip(chanIp);
	user->send(LoginPacket::connectIp(ip, port, charId));
}

auto Characters::connectGame(UserConnection *user, PacketReader &reader) -> void {
	player_id_t id = reader.get<player_id_t>();

	connectGame(user, id);
}

auto Characters::connectGameWorldFromViewAllCharacters(UserConnection *user, PacketReader &reader) -> void {
	if (user->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	player_id_t id = reader.get<player_id_t>();
	world_id_t worldId = static_cast<world_id_t>(reader.get<int32_t>());
	user->setWorldId(worldId);

	// Take the player to a random channel
	channel_id_t channel = Worlds::getInstance().getWorld(worldId)->getRandomChannel();
	user->setChannel(channel);

	connectGame(user, id);
}

auto Characters::ownerCheck(UserConnection *user, int32_t id) -> bool {
	soci::session &sql = Database::getCharDb();
	opt_int32_t exists;

	sql.once
		<< "SELECT 1 "
		<< "FROM " << Database::makeCharTable("characters") << " c "
		<< "WHERE c.character_id = :char AND c.user_id = :user "
		<< "LIMIT 1 ",
		soci::use(id, "char"),
		soci::use(user->getUserId(), "user"),
		soci::into(exists);

	return sql.got_data() && exists.is_initialized();
}

auto Characters::nameTaken(const string_t &name) -> bool {
	soci::session &sql = Database::getCharDb();
	opt_int32_t exists;

	sql.once
		<< "SELECT 1 "
		<< "FROM " << Database::makeCharTable("characters") << " c "
		<< "WHERE c.name = :name "
		<< "LIMIT 1",
		soci::use(name, "name"),
		soci::into(exists);

	return sql.got_data() && exists.is_initialized();
}

auto Characters::nameInvalid(const string_t &name) -> bool {
	return ValidCharDataProvider::getInstance().isForbiddenName(name) || CurseDataProvider::getInstance().isCurseWord(name);
}