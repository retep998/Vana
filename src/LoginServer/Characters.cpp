/*
Copyright (C) 2008 Vana Development Team

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
#include "PlayerLogin.h"
#include "LoginPacket.h"
#include "MySQLM.h"
#include "Worlds.h"
#include "LoginServer.h"
#include "LoginServerAcceptPlayerPacket.h"

void Characters::showEquips(int id, vector<CharEquip> &vec) {
	mysqlpp::Query query = Database::chardb.query();
	query << "SELECT itemid, type, slot FROM items WHERE charid = " << mysqlpp::quote << id << " AND inv = 1 AND slot < 0 ORDER BY type ASC, slot ASC";
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		CharEquip equip;
		equip.id = res[i][0];
		equip.type = (unsigned char) res[i][1];
		equip.slot = res[i][2];
		vec.push_back(equip);
	}	
}

void Characters::loadCharacter(Character &charc, mysqlpp::Row &row) {
	charc.id = row["id"];
	charc.name = row["name"];
	charc.gender = (unsigned char) row["gender"];
	charc.skin = (unsigned char) row["skin"];
	charc.eyes = row["eyes"];
	charc.hair = row["hair"];
	charc.level = (unsigned char) row["level"];
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
	charc.pos = (unsigned char) row["pos"];
	showEquips(charc.id, charc.equips);
}

void Characters::showCharacters(PlayerLogin *player) {
	mysqlpp::Query query = Database::chardb.query();
	query << "SELECT * FROM characters WHERE userid = " << mysqlpp::quote << player->getUserid() << " AND world_id = " << mysqlpp::quote << (int) player->getWorld();
	mysqlpp::StoreQueryResult res = query.store();

	vector <Character> chars;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		Character charc;
		loadCharacter(charc, res[i]);
		chars.push_back(charc);
	}
	LoginPacket::showCharacters(player, chars);
}

void Characters::checkCharacterName(PlayerLogin *player, ReadPacket *packet) {
	string name = packet->getString();
	if (name.size() > 15) {
		return;
	}
	
	LoginPacket::checkName(player, nameTaken(player, name), name);
}

void Characters::createEquip(int equipid, int type, int charid) {
	mysqlpp::Query query = Database::chardb.query();
	switch (type) {
		case 0x05: query << "INSERT INTO items (charid, inv, slot, itemid, type, iwdef) VALUES (" << charid << "," << 1 << "," << -type << "," << equipid << "," << type << "," << 3 << ")"; break;
		case 0x06: query << "INSERT INTO items (charid, inv, slot, itemid, type, iwdef) VALUES (" << charid << "," << 1 << "," << -type << "," << equipid << "," << type << "," << 2 << ")"; break;
		case 0x07: query << "INSERT INTO items (charid, inv, slot, itemid, type, iwdef, slots) VALUES (" << charid << "," << 1 << "," << -type << "," << equipid << "," << type << "," << 3 << "," << 5 << ")"; break;
		case 0x0b: query << "INSERT INTO items (charid, inv, slot, itemid, type, iwatk) VALUES (" << charid << "," << 1 << "," << -type << "," << equipid << "," << type << "," << 17 << ")"; break;
	}
	query.exec();
}

void Characters::createCharacter(PlayerLogin *player, ReadPacket *packet) {
	Character charc;
	string name = packet->getString();
	if (name.size() > 15) {
		return;
	}

	// Let's check our char name again just to be sure
	if (nameTaken(player, name)) {
		LoginPacket::checkName(player, 1, name);
		return;
	}

	int eyes = packet->getInt();
	int hair = packet->getInt() + packet->getInt(); // Hair+hair colour
	int skin = packet->getInt();
	packet->skipBytes(16);

	char gender = packet->getByte();
	char str = packet->getByte();
	char dex = packet->getByte();
	char intt = packet->getByte();
	char luk = packet->getByte();

	if (str + dex + intt + luk != 25) {
		// hacking
		return;
	}
	mysqlpp::Query query = Database::chardb.query();
	query << "INSERT INTO characters (userid, world_id, name, eyes, hair, skin, gender, str, dex, `int`, luk) VALUES (" 
			<< mysqlpp::quote << player->getUserid() << ","
			<< mysqlpp::quote << (int) player->getWorld() << ","
			<< mysqlpp::quote << name << ","
			<< mysqlpp::quote << eyes << ","
			<< mysqlpp::quote << hair << ","
			<< mysqlpp::quote << skin << ","
			<< mysqlpp::quote << (int) gender << ","
			<< mysqlpp::quote << (int) str << ","
			<< mysqlpp::quote << (int) dex << ","
			<< mysqlpp::quote << (int) intt << ","
			<< mysqlpp::quote << (int) luk << ")";
	mysqlpp::SimpleResult res = query.execute();
	int id = (int) res.insert_id();

	packet->skipBytes(-21);
	createEquip(packet->getInt(), 0x05, id);
	createEquip(packet->getInt(), 0x06, id);
	createEquip(packet->getInt(), 0x07, id);
	createEquip(packet->getInt(), 0x0b, id);

	query << "INSERT INTO items (charid, inv, slot, itemid, amount) VALUES (" << mysqlpp::quote << id << ", 4, 1, 4161001, 1)"; // Beginner Guide
	query.exec();

	query << "SELECT * FROM characters WHERE id = " << mysqlpp::quote << id << " LIMIT 1";
	mysqlpp::StoreQueryResult res2 = query.store();

	loadCharacter(charc, res2[0]);
	LoginPacket::showCharacter(player, charc);
}

void Characters::deleteCharacter(PlayerLogin *player, ReadPacket *packet) {
	int data = packet->getInt();
	int id = packet->getInt();
	
	if (!ownerCheck(player, id)) {
		// hacking
		return;
	}

	mysqlpp::Query query = Database::chardb.query();

	query << "DELETE FROM characters WHERE id = " << mysqlpp::quote << id;
	query.exec();

	query << "DELETE FROM keymap WHERE charid = " << mysqlpp::quote << id;
	query.exec();

	query << "DELETE FROM items WHERE charid = " << mysqlpp::quote << id;
	query.exec();

	query << "DELETE FROM skills WHERE charid = " << mysqlpp::quote << id;
	query.exec();

	query << "DELETE FROM character_variables WHERE charid = " << mysqlpp::quote << id;
	query.exec();

	LoginPacket::deleteCharacter(player, id);
}


void Characters::connectGame(PlayerLogin *player, ReadPacket *packet) {
	int id = packet->getInt();

	if (!ownerCheck(player, id)) {
		// hacking
		return;
	}

	LoginServerAcceptPlayerPacket::newPlayer(Worlds::worlds[player->getWorld()]->player, player->getChannel(), id);
	LoginPacket::connectIP(player, id);
}

bool Characters::ownerCheck(PlayerLogin *player, int id) {
	mysqlpp::Query query = Database::chardb.query();
	query << "SELECT true FROM characters WHERE id = " << mysqlpp::quote << id << " AND userid = " << mysqlpp::quote << player->getUserid();
	mysqlpp::StoreQueryResult res = query.store();

	return (res.num_rows() == 1) ? 1 : 0 ;
}

bool Characters::nameTaken(PlayerLogin *player, const string &name) {
	mysqlpp::Query query = Database::chardb.query();
	query << "SELECT true FROM characters WHERE name = " << mysqlpp::quote << name  << " AND world_id = " << mysqlpp::quote << (int) player->getWorld() << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();

	return (res.num_rows() == 1) ? 1 : 0 ;
}
