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
#include "Characters.h"
#include "Database.h"
#include "LoginPacket.h"
#include "LoginServer.h"
#include "LoginServerAcceptPacket.h"
#include "PacketReader.h"
#include "PlayerLogin.h"
#include "Randomizer.h"
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

	if (charc.job < 900) {
		charc.w_rank = row["world_cpos"];
		charc.w_rankmove = (int32_t) row["world_cpos"] - row["world_opos"];
		charc.j_rank = row["job_cpos"];
		charc.j_rankmove = (int32_t) row["world_cpos"] - row["world_opos"];
	}
	else { // GMs can't have their rank sent other wise the client will crash
		charc.w_rank = 0;
		charc.w_rankmove = 0;
		charc.j_rank = 0;
		charc.j_rankmove = 0;
	}
	loadEquips(charc.id, charc.equips);
}

void Characters::showAllCharacters(PlayerLogin *player) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM characters WHERE userid = " << player->getUserid();
	mysqlpp::StoreQueryResult res = query.store();

	typedef unordered_map<uint8_t, vector<Character> > CharsMap;

	CharsMap chars;
	uint32_t charsNum = 0; // I want to reference this later
	for (size_t i = 0; i < res.num_rows(); ++i) {
		uint8_t worldid = res[i]["world_id"];
		if (!Worlds::worlds[worldid]->connected) {
			// World is not connected
			continue;
		}

		Character charc;
		loadCharacter(charc, res[i]);
		chars[res[i]["world_id"]].push_back(charc);
		charsNum++;
	}

	uint32_t unk = charsNum + (3 - charsNum%3); // What I've observed O_o
	LoginPacket::showAllCharactersInfo(player, chars.size(), unk);
	for (CharsMap::const_iterator iter = chars.begin(); iter != chars.end(); iter++) {
		LoginPacket::showCharactersWorld(player, iter->first, iter->second);
	}
}

void Characters::showCharacters(PlayerLogin *player) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM characters WHERE userid = " << player->getUserid() << " AND world_id = " << (int32_t) player->getWorld();
	mysqlpp::StoreQueryResult res = query.store();

	vector<Character> chars;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		Character charc;
		loadCharacter(charc, res[i]);
		chars.push_back(charc);
	}
	LoginPacket::showCharacters(player, chars);
}

void Characters::checkCharacterName(PlayerLogin *player, PacketReader &packet) {
	string name = packet.getString();
	if (name.size() > 15 || name.size() < 4) {
		return;
	}

	LoginPacket::checkName(player, name, nameTaken(player, name));
}

void Characters::createEquip(int32_t equipid, int32_t type, int32_t charid) {
	mysqlpp::Query query = Database::getCharDB().query();
	switch (type) {
		case 0x05: query << "INSERT INTO items (charid, inv, slot, itemid, iwdef, name) VALUES (" << charid << "," << 1 << "," << -type << "," << equipid << "," << 3 << ",\"\")"; break;
		case 0x06: query << "INSERT INTO items (charid, inv, slot, itemid, iwdef, name) VALUES (" << charid << "," << 1 << "," << -type << "," << equipid << "," << 2 << ",\"\")"; break;
		case 0x07: query << "INSERT INTO items (charid, inv, slot, itemid, iwdef, slots, name) VALUES (" << charid << "," << 1 << "," << -type << "," << equipid << "," << 3 << "," << 5 << ",\"\")"; break;
		case 0x0b: query << "INSERT INTO items (charid, inv, slot, itemid, iwatk, name) VALUES (" << charid << "," << 1 << "," << -type << "," << equipid << "," << 17 << ",\"\")"; break;
	}
	query.exec();
}

void Characters::createCharacter(PlayerLogin *player, PacketReader &packet) {
	Character charc;
	string name = packet.getString();
	if (name.size() > 15 || name.size() < 4) {
		return;
	}

	// Let's check our char name again just to be sure
	if (nameTaken(player, name)) {
		LoginPacket::checkName(player, name, true);
		return;
	}

	int32_t eyes = packet.get<int32_t>();
	int32_t hair = packet.get<int32_t>() + packet.get<int32_t>(); // Hair+hair colour
	int32_t skin = packet.get<int32_t>();
	packet.skipBytes(16);

	uint16_t gender = packet.get<int8_t>();
	uint16_t str = 12;
	uint16_t dex = 5;
	uint16_t intt = 4;
	uint16_t luk = 4;

	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO characters (name, userid, world_id, eyes, hair, skin, gender, str, dex, `int`, luk) VALUES ("
			<< mysqlpp::quote << name << ","
			<< player->getUserid() << ","
			<< (int32_t) player->getWorld() << ","
			<< eyes << ","
			<< hair << ","
			<< skin << ","
			<< gender << ","
			<< str << ","
			<< dex << ","
			<< intt << ","
			<< luk << ")";
	mysqlpp::SimpleResult res = query.execute();
	int32_t id = (int32_t) res.insert_id();

	packet.skipBytes(-17);
	createEquip(packet.get<int32_t>(), 0x05, id);
	createEquip(packet.get<int32_t>(), 0x06, id);
	createEquip(packet.get<int32_t>(), 0x07, id);
	createEquip(packet.get<int32_t>(), 0x0b, id);

	query << "INSERT INTO items (charid, inv, slot, itemid, amount, name) VALUES (" << id << ", 4, 1, 4161001, 1, \"\")"; // Beginner Guide
	query.exec();

	query << "SELECT * FROM characters WHERE id = " << id << " LIMIT 1";
	mysqlpp::StoreQueryResult res2 = query.store();

	loadCharacter(charc, res2[0]);
	LoginPacket::showCharacter(player, charc);
}

void Characters::deleteCharacter(PlayerLogin *player, PacketReader &packet) {
	int32_t data = packet.get<int32_t>();
	int32_t id = packet.get<int32_t>();

	if (!ownerCheck(player, id)) {
		// hacking
		return;
	}

	bool success = false;
	if (data == player->getCharDeletePassword()) {
		mysqlpp::Query query = Database::getCharDB().query();

		query << "DELETE FROM characters WHERE id = " << id;
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

		success = true;
	}
	LoginPacket::deleteCharacter(player, id, success);
}

void Characters::connectGame(PlayerLogin *player, int32_t charid) {
	if (!ownerCheck(player, charid)) {
		// hacking
		return;
	}

	LoginServerAcceptPacket::newPlayer(Worlds::worlds[player->getWorld()]->player, player->getChannel(), charid);
	LoginPacket::connectIp(player, charid);
}

void Characters::connectGame(PlayerLogin *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();

	connectGame(player, id);
}

void Characters::connectGameWorld(PlayerLogin *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int8_t worldid = (int8_t) packet.get<int32_t>();
	player->setWorld(worldid);

	// Take the player to a random channel
	uint16_t channel = Randomizer::Instance()->randShort((uint16_t)(Worlds::worlds[worldid]->maxChannels - 1));
	player->setChannel(channel);

	connectGame(player, id);
}

bool Characters::ownerCheck(PlayerLogin *player, int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT true FROM characters WHERE id = " << id << " AND userid = " << player->getUserid();
	mysqlpp::StoreQueryResult res = query.store();

	return (res.num_rows() == 1) ? 1 : 0 ;
}

bool Characters::nameTaken(PlayerLogin *player, const string &name) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT true FROM characters WHERE name = " << mysqlpp::quote << name  << " AND world_id = " << (int32_t) player->getWorld() << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();

	return (res.num_rows() == 1) ? true : false ;
}
