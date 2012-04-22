/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "Characters.h"
#include "Database.h"
#include "EquipDataProvider.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "ItemConstants.h"
#include "LoginPacket.h"
#include "LoginServer.h"
#include "LoginServerAcceptPacket.h"
#include "MiscUtilities.h"
#include "PacketReader.h"
#include "Player.h"
#include "Session.h"
#include "World.h"
#include "Worlds.h"
#include "ValidCharDataProvider.h"
#include <unordered_map>

using std::unordered_map;

void Characters::loadEquips(int32_t id, vector<CharEquip> &vec) {
	soci::rowset<> rs = (Database::getCharDb().prepare << "SELECT i.item_id, i.slot " <<
															"FROM items i " <<
															"WHERE i.character_id = :id " <<
																"AND i.inv = :inv " <<
																"AND i.slot < 0 " <<
															"ORDER BY slot ASC",
																soci::use(id, "id"),
																soci::use(Inventories::EquipInventory, "inv"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		CharEquip equip;
		equip.id = row.get<int32_t>("item_id");
		equip.slot = row.get<int16_t>("slot");
		vec.push_back(equip);
	}
}

void Characters::loadCharacter(Character &charc, const soci::row &row) {
	charc.id = row.get<int32_t>("character_id");
	charc.name = row.get<string>("name");
	charc.gender = row.get<int8_t>("gender");
	charc.skin = row.get<int8_t>("skin");
	charc.eyes = row.get<int32_t>("eyes");
	charc.hair = row.get<int32_t>("hair");
	charc.level = row.get<uint8_t>("level");
	charc.job = row.get<int16_t>("job");
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
	charc.exp = row.get<int32_t>("exp");
	charc.fame = row.get<int16_t>("fame");
	charc.map = row.get<int32_t>("map");
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

void Characters::showAllCharacters(Player *player) {
	soci::rowset<> rs = (Database::getCharDb().prepare << "SELECT * FROM characters c WHERE c.user_id = :user ",
															soci::use(player->getUserId(), "user"));

	typedef unordered_map<uint8_t, vector<Character>> CharsMap;

	CharsMap chars;
	uint32_t charsNum = 0;
	World *world;

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		uint8_t worldId = row.get<uint8_t>("world_id");
		world = Worlds::Instance()->getWorld(worldId);
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
	LoginPacket::showAllCharactersInfo(player, chars.size(), unk);
	for (CharsMap::const_iterator iter = chars.begin(); iter != chars.end(); ++iter) {
		LoginPacket::showCharactersWorld(player, iter->first, iter->second);
	}
}

void Characters::showCharacters(Player *player) {
	soci::session &sql = Database::getCharDb();
	int8_t worldId = player->getWorld();
	int32_t userId = player->getUserId();

	soci::rowset<> rs = (sql.prepare << "SELECT * FROM characters c WHERE c.user_id = :user AND c.world_id = :world ",
										soci::use(userId, "user"),
										soci::use(worldId, "world"));

	vector<Character> chars;
	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		Character charc;
		loadCharacter(charc, row);
		chars.push_back(charc);
	}

	opt_int32_t max;
	sql.once << "SELECT s.char_slots FROM storage s WHERE s.user_id = :user AND s.world_id = :world ",
				soci::use(userId, "user"),
				soci::use(worldId, "world"),
				soci::into(max);

	if (!sql.got_data() || !max.is_initialized()) {
		max = Characters::DefaultCharacterSlots;
	}

	LoginPacket::showCharacters(player, chars, max.get());
}

void Characters::checkCharacterName(Player *player, PacketReader &packet) {
	const string &name = packet.getString();
	if (!MiscUtilities::inRangeInclusive<size_t>(name.size(), Characters::MinNameSize, Characters::MaxNameSize)) {
		return;
	}

	LoginPacket::checkName(player, name, nameIllegal(player, name));
}

void Characters::createItem(int32_t itemId, Player *player, int32_t charId, int32_t slot, int16_t amount) {
	using namespace soci;

	session &sql = Database::getCharDb();
	int16_t inventory = GameLogicUtilities::getInventory(itemId);

	if (inventory == Inventories::EquipInventory) {
		Item equip(itemId, false);
		sql.once << "INSERT INTO items (character_id, inv, slot, location, user_id, world_id, item_id, slots, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, name) " <<
					"VALUES (:char, :inv, :slot, :loc, :user, :world, :itemid, :slots, :str, :dex, :int, :luk, :hp, :mp, :watk, :matk, :wdef, :mdef, :acc, :avo, :hand, :speed, :jump, :name)",
					use(charId, "char"),
					use(inventory, "inv"),
					use(slot, "slot"),
					use(string("inventory"), "loc"),
					use(player->getUserId(), "user"),
					use(player->getWorld(), "world"),
					use(itemId, "itemid"),
					use(equip.getSlots(), "slots"),
					use(equip.getStr(), "str"),
					use(equip.getDex(), "dex"),
					use(equip.getInt(), "int"),
					use(equip.getLuk(), "luk"),
					use(equip.getHp(), "hp"),
					use(equip.getMp(), "mp"),
					use(equip.getWatk(), "watk"),
					use(equip.getMatk(), "matk"),
					use(equip.getWdef(), "wdef"),
					use(equip.getMdef(), "mdef"),
					use(equip.getAccuracy(), "acc"),
					use(equip.getAvoid(), "avo"),
					use(equip.getHands(), "hand"),
					use(equip.getSpeed(), "speed"),
					use(equip.getJump(), "jump"),
					use(string(""), "name");
	}
	else {
		sql.once << "INSERT INTO items (character_id, inv, slot, location, user_id, world_id, item_id, amount, name) " <<
					"VALUES (:char, :inv, :slot, :loc, :user, :world, :itemid, :amount, :name)",
					use(charId, "char"),
					use(inventory, "inv"),
					use(slot, "slot"),
					use(string("inventory"), "loc"),
					use(player->getUserId(), "user"),
					use(player->getWorld(), "world"),
					use(itemId, "itemid"),
					use(amount, "amount"),
					use(string(""), "name");
	}
}

void Characters::createCharacter(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}

	string name = packet.getString();
	if (!MiscUtilities::inRangeInclusive<size_t>(name.size(), Characters::MinNameSize, Characters::MaxNameSize)) {
		return;
	}

	// Let's check our char name again just to be sure
	if (nameIllegal(player, name)) {
		LoginPacket::checkName(player, name, true);
		return;
	}

	int32_t eyes = packet.get<int32_t>();
	int32_t hair = packet.get<int32_t>();
	int32_t hairColor = packet.get<int32_t>();
	int32_t skin = packet.get<int32_t>();
	int32_t top = packet.get<int32_t>();
	int32_t bottom = packet.get<int32_t>();
	int32_t shoes = packet.get<int32_t>();
	int32_t weapon = packet.get<int32_t>();
	int8_t gender = packet.get<int8_t>();

	if (!ValidCharDataProvider::Instance()->isValidCharacter(gender, hair, hairColor, eyes, skin, top, bottom, shoes, weapon, ValidCharDataProvider::Adventurer)) {
		// Hacking
		player->getSession()->disconnect();
		return;
	}

	uint16_t str = 12;
	uint16_t dex = 5;
	uint16_t intt = 4;
	uint16_t luk = 4;

	soci::session &sql = Database::getCharDb();

	sql.once << "INSERT INTO characters (name, user_id, world_id, eyes, hair, skin, gender, str, dex, `int`, luk) " <<
				"VALUES (:name, :user, :world, :eyes, :hair, :skin, :gender, :str, :dex, :int, :luk)",
				soci::use(name, "name"),
				soci::use(player->getUserId(), "user"),
				soci::use(player->getWorld(), "world"),
				soci::use(eyes, "eyes"),
				soci::use(hair + hairColor, "hair"),
				soci::use(skin, "skin"),
				soci::use(gender, "gender"),
				soci::use(str, "str"),
				soci::use(dex, "dex"),
				soci::use(intt, "int"),
				soci::use(luk, "luk");

	int32_t id = 0;
	sql.once << "SELECT LAST_INSERT_ID()", soci::into(id);

	createItem(top, player, id, -EquipSlots::Top);
	createItem(bottom, player, id, -EquipSlots::Bottom);
	createItem(shoes, player, id, -EquipSlots::Shoe);
	createItem(weapon, player, id, -EquipSlots::Weapon);
	createItem(Items::BeginnersGuidebook, player, id, 1);

	soci::row row;
	sql.once << "SELECT * FROM characters c WHERE c.character_id = :id",
				soci::use(id, "id"),
				soci::into(row);

	Character charc;
	loadCharacter(charc, row);
	LoginPacket::showCharacter(player, charc);
}

void Characters::deleteCharacter(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	int32_t data = packet.get<int32_t>();
	int32_t id = packet.get<int32_t>();

	if (!ownerCheck(player, id)) {
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
	opt_int8_t worldId;

	sql.once << "SELECT world_id FROM characters c WHERE c.character_id = :char ",
					soci::use(id, "char"),
					soci::into(worldId);

	if (!sql.got_data() || !worldId.is_initialized()) {
		// ???
		return;
	}

	bool success = false;
	if (data == player->getCharDeletePassword()) {
		Worlds::Instance()->runFunction([&id, &worldId](World *world) -> bool {
			if (world->isConnected() && world->getId() == worldId.get()) {
				// LoginServerAcceptPacket::removeCharacter(world->getConnection(), playerId);
				// For guilds
				return true;
			}
			return false;
		});

		sql.once << "DELETE FROM characters WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM active_quests WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM completed_quests WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM cooldowns WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM teleport_rock_locations WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM buddylist WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM keymap WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM skills WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM skill_macros WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM character_variables WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM monster_book WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM mounts WHERE character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE p FROM pets p INNER JOIN items i ON p.pet_id = i.pet_id WHERE i.character_id = :char ", soci::use(id, "char");
		sql.once << "DELETE FROM items WHERE character_id = :char ", soci::use(id, "char");
	}
	else {
		result = IncorrectBirthday;
	}
	LoginPacket::deleteCharacter(player, id, result);
}

void Characters::connectGame(Player *player, int32_t charId) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	if (!ownerCheck(player, charId)) {
		// Hacking
		return;
	}

	LoginServerAcceptPacket::newPlayer(Worlds::Instance()->getWorld(player->getWorld())->getConnection(), player->getChannel(), charId, player->getIp());
	LoginPacket::connectIp(player, charId);
}

void Characters::connectGame(Player *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();

	connectGame(player, id);
}

void Characters::connectGameWorld(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	int32_t id = packet.get<int32_t>();
	int8_t worldId = (int8_t) packet.get<int32_t>();
	player->setWorld(worldId);

	// Take the player to a random channel
	uint16_t channel = Worlds::Instance()->getWorld(worldId)->getRandomChannel();
	player->setChannel(channel);

	connectGame(player, id);
}

bool Characters::ownerCheck(Player *player, int32_t id) {
	soci::session &sql = Database::getCharDb();
	opt_int32_t exists;
	sql.once << "SELECT 1 FROM characters c WHERE c.character_id = :char AND c.user_id = :user ",
									soci::use(id, "char"),
									soci::use(player->getUserId(), "user"),
									soci::into(exists);
	return sql.got_data() && exists.is_initialized();
}

bool Characters::nameIllegal(Player *player, const string &name) {
	soci::session &sql = Database::getCharDb();
	opt_int32_t exists;
	sql.once << "SELECT 1 FROM characters c WHERE c.name = :name ",
									soci::use(name, "name"),
									soci::into(exists);

	return (sql.got_data() && exists.is_initialized() ? true : ValidCharDataProvider::Instance()->isForbiddenName(name));
}