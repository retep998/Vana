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
#include "Characters.h"
#include "Database.h"
#include "EquipDataProvider.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "LoginPacket.h"
#include "LoginServer.h"
#include "LoginServerAcceptPacket.h"
#include "PacketReader.h"
#include "Player.h"
#include "ValidCharDataProvider.h"
#include "World.h"
#include "Worlds.h"
#include <boost/tr1/unordered_map.hpp>

using std::tr1::unordered_map;

void Characters::loadEquips(int32_t id, vector<CharEquip> &vec) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT itemid, slot FROM items WHERE charid = " << id << " AND inv = 1 AND slot < 0 ORDER BY slot ASC";
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		CharEquip equip;
		equip.id = res[i][0];
		equip.slot = res[i][1];
		vec.push_back(equip);
	}
}

void Characters::loadCharacter(Character &charc, const mysqlpp::Row &row) {
	charc.id = row["id"];
	charc.name = (string) row["name"];
	charc.gender = (uint8_t) row["gender"];
	charc.skin = (uint8_t) row["skin"];
	charc.eyes = row["eyes"];
	charc.hair = row["hair"];
	charc.level = (uint8_t) row["level"];
	charc.job = row["job"];
	charc.str = row["str"];
	charc.dex = row["dex"];
	charc.intt = row["int"];
	charc.luk = row["luk"];
	charc.hp = row["chp"];
	charc.mhp = row["mhp"];
	charc.mp = row["cmp"];
	charc.mmp = row["mmp"];
	charc.ap = row["ap"];
	charc.sp = row["sp"];
	charc.exp= row["exp"];
	charc.fame = row["fame"];
	charc.map = row["map"];
	charc.pos = (uint8_t) row["pos"];

	if (GameLogicUtilities::getJobTrack(charc.job) == Jobs::JobTracks::Gm) { // GMs can't have their rank sent otherwise the client will crash
		charc.w_rank = 0;
		charc.w_rankmove = 0;
		charc.j_rank = 0;
		charc.j_rankmove = 0;
	}
	else {
		charc.w_rank = row["world_cpos"];
		charc.w_rankmove = (int32_t) row["world_cpos"] - row["world_opos"];
		charc.j_rank = row["job_cpos"];
		charc.j_rankmove = (int32_t) row["job_cpos"] - row["job_opos"];
	}
	loadEquips(charc.id, charc.equips);
}

void Characters::showAllCharacters(Player *player) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM characters WHERE userid = " << player->getUserId();
	mysqlpp::StoreQueryResult res = query.store();

	typedef unordered_map<uint8_t, vector<Character> > CharsMap;

	CharsMap chars;
	uint32_t charsNum = 0;
	World *world;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		uint8_t worldid = res[i]["world_id"];
		world = Worlds::Instance()->getWorld(worldid);
		if (world == nullptr || !world->isConnected()) {
			// World is not connected
			continue;
		}

		Character charc;
		loadCharacter(charc, res[i]);
		chars[res[i]["world_id"]].push_back(charc);
		charsNum++;
	}

	uint32_t unk = charsNum + (3 - charsNum % 3); // What I've observed
	LoginPacket::showAllCharactersInfo(player, chars.size(), unk);
	for (CharsMap::const_iterator iter = chars.begin(); iter != chars.end(); iter++) {
		LoginPacket::showCharactersWorld(player, iter->first, iter->second);
	}
}

void Characters::showCharacters(Player *player) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM characters WHERE userid = " << player->getUserId() << " AND world_id = " << (int32_t) player->getWorld();
	mysqlpp::StoreQueryResult res = query.store();

	vector<Character> chars;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		Character charc;
		loadCharacter(charc, res[i]);
		chars.push_back(charc);
	}

	query << "SELECT char_slots FROM storage WHERE userid = " << player->getUserId() << " AND world_id = " << (int32_t) player->getWorld();
	res = query.store();

	int32_t max = Characters::DefaultCharacterSlots;
	if (!res.empty()) {
		max = static_cast<int32_t>(res[0][0]);
	}

	LoginPacket::showCharacters(player, chars, max);
}

void Characters::checkCharacterName(Player *player, PacketReader &packet) {
	string name = packet.getString();
	if (name.size() > Characters::MaxNameSize || name.size() < Characters::MinNameSize) {
		return;
	}

	LoginPacket::checkName(player, name, nameIllegal(player, name));
}

void Characters::createItem(int32_t itemid, int32_t charid, int32_t slot, int16_t amount) {
	mysqlpp::Query query = Database::getCharDB().query();
	int16_t inventory = GameLogicUtilities::getInventory(itemid);
	if (inventory == Inventories::EquipInventory) {
		Item equip(itemid, false);
		query << "INSERT INTO items (charid, inv, slot, itemid, slots, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, name) VALUES ("
			<< charid << ", "
			<< inventory << ", "
			<< slot << ", "
			<< itemid << ", "
			<< (int16_t) equip.getSlots() << ", "
			<< equip.getStr() << ", "
			<< equip.getDex() << ", "
			<< equip.getInt() << ", "
			<< equip.getLuk() << ", "
			<< equip.getHp() << ", "
			<< equip.getMp() << ", "
			<< equip.getWatk() << ", "
			<< equip.getMatk() << ", "
			<< equip.getWdef() << ", "
			<< equip.getMdef() << ", "
			<< equip.getAccuracy() << ", "
			<< equip.getAvoid() << ", "
			<< equip.getHands() << ", "
			<< equip.getSpeed() << ", "
			<< equip.getJump() << ", "
			<< "\"\")";
	}
	else {
		query << "INSERT INTO items (charid, inv, slot, itemid, amount, name) VALUES (" << charid << ", " << inventory << ", " << slot << ", " << itemid  << ", " << amount << ", \"\")";
	}
	query.exec();
}

void Characters::createCharacter(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	Character charc;
	string name = packet.getString();
	if (name.size() > Characters::MaxNameSize || name.size() < Characters::MinNameSize) {
		return;
	}

	// Let's check our char name again just to be sure
	if (nameIllegal(player, name)) {
		LoginPacket::checkName(player, name, true);
		return;
	}

	int32_t eyes = packet.get<int32_t>();
	int32_t hair = packet.get<int32_t>();
	int32_t haircolor = packet.get<int32_t>();
	int32_t skin = packet.get<int32_t>();
	int32_t top = packet.get<int32_t>();
	int32_t bottom = packet.get<int32_t>();
	int32_t shoes = packet.get<int32_t>();
	int32_t weapon = packet.get<int32_t>();
	int8_t gender = packet.get<int8_t>();

	if (!ValidCharDataProvider::Instance()->isValidCharacter(gender, hair, haircolor, eyes, skin, top, bottom, shoes, weapon, ValidCharDataProvider::Adventurer)) {
		// Hacking
		player->getSession()->disconnect();
		return;
	}

	uint16_t str = 12;
	uint16_t dex = 5;
	uint16_t intt = 4;
	uint16_t luk = 4;

	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO characters (name, userid, world_id, eyes, hair, skin, gender, str, dex, `int`, luk) VALUES ("
			<< mysqlpp::quote << name << ","
			<< player->getUserId() << ","
			<< (int32_t) player->getWorld() << ","
			<< eyes << ","
			<< hair + haircolor << ","
			<< skin << ","
			<< (int16_t) gender << ","
			<< str << ","
			<< dex << ","
			<< intt << ","
			<< luk << ")";

	mysqlpp::SimpleResult res = query.execute();
	int32_t id = (int32_t) res.insert_id();

	createItem(top, id, -EquipSlots::Top);
	createItem(bottom, id, -EquipSlots::Bottom);
	createItem(shoes, id, -EquipSlots::Shoe);
	createItem(weapon, id, -EquipSlots::Weapon);
	createItem(Items::BeginnersGuidebook, id, 1);

	query << "SELECT * FROM characters WHERE id = " << id << " LIMIT 1";
	mysqlpp::StoreQueryResult res2 = query.store();

	loadCharacter(charc, res2[0]);
	LoginPacket::showCharacter(player, charc);
}

namespace Functors {
	struct CharDeleteFunctor {
		bool operator()(World *world) {
			if (world->isConnected() && world->getId() == worldId) {
				LoginServerAcceptPacket::removeCharacter(world->getConnection(), playerId);
				return true;
			}
			return false;
		}
		int32_t playerId;
		int32_t worldId;
	};
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
	enum DeletionConstants {
		Success = 0x00,
		IncorrectBirthday = 0x12,
		NoGuildMaster = 0x16,
		NoProposingChumps = 0x18,
		NoWorldTransfers = 0x1A
	};

	uint8_t result = Success;

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT guildid, guildrank, world_id FROM characters WHERE id = " << id << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();

	if ((int32_t) res[0]["guildrank"] == 1) {
		result = NoGuildMaster;
	}
	else if (data == player->getCharDeletePassword()) {
		if ((int32_t) res[0]["guildid"] != 0) {
			Functors::CharDeleteFunctor func = {id, (int32_t) res[0]["world_id"]};
			Worlds::Instance()->runFunction(func);
		}

		query << "DELETE FROM characters WHERE id = " << id;
		query.exec();

		query << "DELETE FROM active_quests WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM completed_quests WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM cooldowns WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM teleport_rock_locations WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM buddylist WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM keymap WHERE charid = " << id;
		query.exec();

		query << "DELETE pets, items FROM pets LEFT JOIN items ON pets.id = items.petid WHERE items.charid = " << id;
		query.exec();

		query << "DELETE FROM items WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM skills WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM skillmacros WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM character_variables WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM monsterbook WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM mounts WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM character_wishlist WHERE charid = " << id;
		query.exec();

		query << "DELETE FROM character_cashshop_gifs WHERE charid = " << id;
		query.exec();
	}
	else {
		result = IncorrectBirthday;
	}

	LoginPacket::deleteCharacter(player, id, result);
}

void Characters::connectGame(Player *player, int32_t charid) {
	if (!ownerCheck(player, charid)) {
		// Hacking
		return;
	}

	LoginServerAcceptPacket::newPlayer(Worlds::Instance()->getWorld(player->getWorld())->getConnection(), player->getChannel(), charid, player->getIp());
	LoginPacket::connectIp(player, charid);
}

void Characters::connectGame(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	int32_t id = packet.get<int32_t>();

	connectGame(player, id);
}

void Characters::connectGameWorld(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::LoggedIn) {
		// Hacking
		return;
	}
	int32_t id = packet.get<int32_t>();
	int8_t worldid = (int8_t) packet.get<int32_t>();
	player->setWorld(worldid);

	// Take the player to a random channel
	uint16_t channel = Worlds::Instance()->getWorld(worldid)->getRandomChannel();
	player->setChannel(channel);

	connectGame(player, id);
}

bool Characters::ownerCheck(Player *player, int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT true FROM characters WHERE id = " << id << " AND userid = " << player->getUserId();
	mysqlpp::StoreQueryResult res = query.store();

	return (res.num_rows() == 1);
}

bool Characters::nameIllegal(Player *player, const string &name) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT true FROM characters WHERE name = " << mysqlpp::quote << name << " AND world_id = " << (int32_t) player->getWorld() << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();

	return ((res.num_rows() == 1) ? true : ValidCharDataProvider::Instance()->isForbiddenName(name));
}
