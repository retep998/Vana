/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Common/Algorithm.hpp"
#include "Common/CurseDataProvider.hpp"
#include "Common/ClientIp.hpp"
#include "Common/Database.hpp"
#include "Common/EquipDataProvider.hpp"
#include "Common/GameConstants.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/ItemConstants.hpp"
#include "Common/MiscUtilities.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Session.hpp"
#include "Common/ValidCharDataProvider.hpp"
#include "LoginServer/LoginPacket.hpp"
#include "LoginServer/LoginServer.hpp"
#include "LoginServer/LoginServerAcceptPacket.hpp"
#include "LoginServer/SyncPacket.hpp"
#include "LoginServer/UserConnection.hpp"
#include "LoginServer/World.hpp"
#include "LoginServer/Worlds.hpp"
#include <unordered_map>

namespace Vana {
namespace LoginServer {

auto Characters::loadEquips(player_id_t id, vector_t<CharEquip> &vec) -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT i.item_id, i.slot "
		<< "FROM " << db.makeTable("items") << " i "
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
	charc.face = row.get<face_id_t>("face");
	charc.hair = row.get<hair_id_t>("hair");
	charc.level = row.get<player_level_t>("level");
	charc.job = row.get<job_id_t>("job");
	charc.str = row.get<stat_t>("str");
	charc.dex = row.get<stat_t>("dex");
	charc.intt = row.get<stat_t>("int");
	charc.luk = row.get<stat_t>("luk");
	charc.hp = row.get<health_t>("chp");
	charc.mhp = row.get<health_t>("mhp");
	charc.mp = row.get<health_t>("cmp");
	charc.mmp = row.get<health_t>("mmp");
	charc.ap = row.get<stat_t>("ap");
	charc.sp = row.get<stat_t>("sp");
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
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT * "
		<< "FROM " << db.makeTable("characters") << " c "
		<< "WHERE c.account_id = :account ",
		soci::use(user->getAccountId(), "account"));

	hash_map_t<world_id_t, vector_t<Character>> chars;
	uint32_t charsNum = 0;
	World *world;

	for (const auto &row : rs) {
		world_id_t worldId = row.get<world_id_t>("world_id");
		world = LoginServer::getInstance().getWorlds().getWorld(worldId);
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
	user->send(Packets::showAllCharactersInfo(static_cast<world_id_t>(chars.size()), unk));
	for (const auto &kvp : chars) {
		user->send(Packets::showViewAllCharacters(kvp.first, kvp.second));
	}
}

auto Characters::showCharacters(UserConnection *user) -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	world_id_t worldId = user->getWorldId();
	account_id_t accountId = user->getAccountId();

	soci::rowset<> rs = (sql.prepare
		<< "SELECT * "
		<< "FROM " << db.makeTable("characters") << " c "
		<< "WHERE c.account_id = :account AND c.world_id = :world ",
		soci::use(accountId, "account"),
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
		<< "FROM " << db.makeTable("storage") << " s "
		<< "WHERE s.account_id = :account AND s.world_id = :world ",
		soci::use(accountId, "account"),
		soci::use(worldId, "world"),
		soci::into(max);

	if (!sql.got_data() || !max.is_initialized()) {
		const WorldConfig &world = LoginServer::getInstance().getWorlds().getWorld(worldId)->getConfig();
		max = world.defaultChars;
	}

	user->send(Packets::showCharacters(chars, max.get()));
}

auto Characters::checkCharacterName(UserConnection *user, PacketReader &reader) -> void {
	string_t name = reader.get<string_t>();
	if (!ext::in_range_inclusive<size_t>(name.size(), Vana::Characters::MinNameSize, Vana::Characters::MaxNameSize)) {
		return;
	}

	if (nameInvalid(name)) {
		user->send(Packets::checkName(name, Packets::CheckNameErrors::Invalid));
	}
	else if (nameTaken(name)) {
		user->send(Packets::checkName(name, Packets::CheckNameErrors::Taken));
	}
	else {
		user->send(Packets::checkName(name, Packets::CheckNameErrors::None));
	}
}

auto Characters::createItem(item_id_t itemId, UserConnection *user, player_id_t charId, inventory_slot_t slot, slot_qty_t amount) -> void {
	auto &db = Database::getCharDb();
	inventory_t inventory = GameLogicUtilities::getInventory(itemId);
	ItemDbInformation info{slot, charId, user->getAccountId(), user->getWorldId(), Item::Inventory};

	if (inventory == Inventories::EquipInventory) {
		Item equip{LoginServer::getInstance().getEquipDataProvider(), itemId, Items::StatVariance::None, false};
		equip.databaseInsert(db, info);
	}
	else {
		Item item{itemId, amount};
		item.databaseInsert(db, info);
	}
}

auto Characters::createCharacter(UserConnection *user, PacketReader &reader) -> void {
	if (user->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}

	string_t name = reader.get<string_t>();
	if (!ext::in_range_inclusive<size_t>(name.size(), Vana::Characters::MinNameSize, Vana::Characters::MaxNameSize)) {
		return;
	}

	// Let's check our char name again just to be sure
	if (nameInvalid(name)) {
		user->send(Packets::checkName(name, Packets::CheckNameErrors::Invalid));
		return;
	}
	if (nameTaken(name)) {
		user->send(Packets::checkName(name, Packets::CheckNameErrors::Taken));
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

	if (!LoginServer::getInstance().getValidCharDataProvider().isValidCharacter(gender, hair, hairColor, face, skin, top, bottom, shoes, weapon, ValidCharDataProvider::Adventurer)) {
		// Hacking
		user->disconnect();
		return;
	}

	stat_t str = 12;
	stat_t dex = 5;
	stat_t intt = 4;
	stat_t luk = 4;

	auto &db = Database::getCharDb();
	auto &sql = db.getSession();

	sql.once
		<< "INSERT INTO " << db.makeTable("characters") << " (name, account_id, world_id, face, hair, skin, gender, str, dex, `int`, luk) "
		<< "VALUES (:name, :account, :world, :face, :hair, :skin, :gender, :str, :dex, :int, :luk)",
		soci::use(name, "name"),
		soci::use(user->getAccountId(), "account"),
		soci::use(user->getWorldId(), "world"),
		soci::use(face, "face"),
		soci::use(hair + hairColor, "hair"),
		soci::use(skin, "skin"),
		soci::use(gender, "gender"),
		soci::use(str, "str"),
		soci::use(dex, "dex"),
		soci::use(intt, "int"),
		soci::use(luk, "luk");

	player_id_t id = db.getLastId<player_id_t>();

	createItem(top, user, id, -EquipSlots::Top);
	createItem(bottom, user, id, -EquipSlots::Bottom);
	createItem(shoes, user, id, -EquipSlots::Shoe);
	createItem(weapon, user, id, -EquipSlots::Weapon);
	createItem(Items::BeginnersGuidebook, user, id, 1);

	soci::row row;
	sql.once
		<< "SELECT * "
		<< "FROM " << db.makeTable("characters") << " c "
		<< "WHERE c.character_id = :id",
		soci::use(id, "id"),
		soci::into(row);

	Character charc;
	loadCharacter(charc, row);
	user->send(Packets::showCharacter(charc));
	LoginServer::getInstance().getWorlds().send(user->getWorldId(), Packets::Interserver::Player::characterCreated(id));
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
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	optional_t<world_id_t> worldId;

	sql.once
		<< "SELECT world_id "
		<< "FROM " << db.makeTable("characters") << " c "
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
		LoginServer::getInstance().getWorlds().runFunction([&id, &worldId](World *world) -> bool {
			if (world->isConnected() && world->getId() == worldId.get()) {
				// Packets::Interserver::removeCharacter(world->getConnection(), playerId);
				// For guilds
				return true;
			}
			return false;
		});

		sql.once << "DELETE p FROM " << db.makeTable("pets") << " p INNER JOIN " << db.makeTable("items") << " i ON p.pet_id = i.pet_id WHERE i.character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM " << db.makeTable("characters") << " WHERE character_id = :char ", soci::use(id, "char");
	}
	else {
		result = IncorrectBirthday;
	}

	user->send(Packets::deleteCharacter(id, result));
	LoginServer::getInstance().getWorlds().send(user->getWorldId(), Packets::Interserver::Player::characterDeleted(id));
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

	auto world = LoginServer::getInstance().getWorlds().getWorld(user->getWorldId());
	world->send(Packets::Interserver::playerConnectingToChannel(user->getChannel(), charId, user->getIp()));

	Ip chanIp{0};
	port_t port = -1;
	if (Channel *channel = world->getChannel(user->getChannel())) {
		chanIp = channel->matchIpToSubnet(user->getIp());
		port = channel->getPort();
	}
	ClientIp ip{chanIp};
	user->send(Packets::connectIp(ip, port, charId));
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

	// We need to do 2 validations here
	// One to ensure that we're picking a character for the account
	// And another to ensure that the world matches the expected world
	optional_t<world_id_t> charWorldId;
	optional_t<account_id_t> accountId;
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	sql.once
		<< "SELECT world_id, account_id "
		<< "FROM " << db.makeTable("characters") << " c "
		<< "WHERE c.character_id = :char ",
		soci::use(id, "char"),
		soci::into(charWorldId),
		soci::into(accountId);

	if (!sql.got_data() || !charWorldId.is_initialized() || !accountId.is_initialized()) {
		// ???
		// Most likely hacking
		return;
	}

	if (accountId != user->getAccountId() || charWorldId != worldId) {
		// Hacking
		return;
	}

	// TODO FIXME packet
	// We need the packet for failure here - it's possible that there are no channels available

	auto world = LoginServer::getInstance().getWorlds().getWorld(worldId);
	if (world->getChannelCount() == 0) {
		// Packet
		return;
	}

	// Take the player to a random channel
	channel_id_t channel = world->getRandomChannel();
	user->setChannel(channel);

	connectGame(user, id);
}

auto Characters::ownerCheck(UserConnection *user, int32_t id) -> bool {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	opt_int32_t exists;

	sql.once
		<< "SELECT 1 "
		<< "FROM " << db.makeTable("characters") << " c "
		<< "WHERE c.character_id = :char AND c.account_id = :account "
		<< "LIMIT 1 ",
		soci::use(id, "char"),
		soci::use(user->getAccountId(), "account"),
		soci::into(exists);

	return sql.got_data() && exists.is_initialized();
}

auto Characters::nameTaken(const string_t &name) -> bool {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	opt_int32_t exists;

	sql.once
		<< "SELECT 1 "
		<< "FROM " << db.makeTable("characters") << " c "
		<< "WHERE c.name = :name "
		<< "LIMIT 1",
		soci::use(name, "name"),
		soci::into(exists);

	return sql.got_data() && exists.is_initialized();
}

auto Characters::nameInvalid(const string_t &name) -> bool {
	return LoginServer::getInstance().getValidCharDataProvider().isForbiddenName(name) ||
		LoginServer::getInstance().getCurseDataProvider().isCurseWord(name);
}

}
}