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
#include "Worlds.h"
#include "LoginServer.h"
#include "LoginServerAcceptPlayerPacket.h"

void Characters::showEquips(int id, vector <CharEquip> &vec){
	mysqlpp::Query query = db.query();
	query << "SELECT equipid, type, pos FROM equip WHERE charid = " << mysqlpp::quote << id << " AND pos < 0 ORDER BY type ASC, pos ASC";
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		CharEquip equip; 
		equip.id = res[i][0];
		equip.type = (unsigned char) res[i][1];
		equip.pos = res[i][2];
		vec.push_back(equip);
	}	
}

void Characters::loadCharacter(Character &charc, mysqlpp::Row row) {
	charc.id = row["id"];
	strcpy_s(charc.name, row["name"]);
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

void Characters::showCharacters(PlayerLogin* player){
	mysqlpp::Query query = db.query();
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

void Characters::checkCharacterName(PlayerLogin* player, unsigned char* packet){
	char charactername[15];
	int size = packet[0];
	if(size>15){
		return;
	}
	BufferUtilities::getString(packet+2, size, charactername);
	
	LoginPacket::checkName(player, nameTaken(player, charactername), charactername);
}

void Characters::createEquip(int equipid, int type, int charid){
	mysqlpp::Query query = db.query();
	if(type==0x05)
		query << "INSERT INTO equip (equipid,charid,type,iwdef,pos) VALUES (" << mysqlpp::quote << equipid << "," << mysqlpp::quote << charid << "," << mysqlpp::quote << type << "," << mysqlpp::quote << 3 << "," << mysqlpp::quote << -type << ")";
	else if(type==0x06)
		query << "INSERT INTO equip (equipid,charid,type,iwdef,pos) VALUES (" << mysqlpp::quote << equipid << "," << mysqlpp::quote << charid << "," << mysqlpp::quote << type << "," << mysqlpp::quote << 2 << "," << mysqlpp::quote << -type << ")";
	else if(type==0x07)
		query << "INSERT INTO equip (equipid,charid,type,iwdef,slots,pos) VALUES (" << mysqlpp::quote << equipid << "," << mysqlpp::quote << charid << "," << mysqlpp::quote << type << "," << mysqlpp::quote << 3 << "," << 5 << "," << mysqlpp::quote << -type << ")";
	else if(type==0x0b)
		query << "INSERT INTO equip (equipid,charid,type,iwatk,pos) VALUES (" << mysqlpp::quote << equipid << "," << mysqlpp::quote << charid << "," << mysqlpp::quote << type << "," << mysqlpp::quote << 17 << "," << mysqlpp::quote << -type << ")";
	query.exec();
}

void Characters::createCharacter(PlayerLogin* player, unsigned char* packet){
	Character charc;
	char charactername[15];
	int size = packet[0];
	if(size>15){
		return;
	}
	BufferUtilities::getString(packet+2, size, charactername);
	// Let's check our char name again just to be sure
	if(nameTaken(player, charactername)) {
		LoginPacket::checkName(player, 1, charactername);
		return;
	}
	int pos = 2+size;
	int eyes = BufferUtilities::getInt(packet+pos);
	pos+=4;
	int hair = BufferUtilities::getInt(packet+pos);
	pos+=8;
	int skin = BufferUtilities::getInt(packet+pos);
	pos+=20;
	if(packet[pos+1] + packet[pos+2] + packet[pos+3] + packet[pos+4] != 25){
		// hacking
		return;
	}
	mysqlpp::Query query = db.query();
	query << "INSERT INTO characters (userid, world_id, name, eyes, hair, skin, gender, str, dex, `int`, luk) VALUES (" 
			<< mysqlpp::quote << player->getUserid() << ","
			<< mysqlpp::quote << (int) player->getWorld() << ","
			<< mysqlpp::quote << charactername << ","
			<< mysqlpp::quote << eyes << ","
			<< mysqlpp::quote << hair << ","
			<< mysqlpp::quote << skin << ","
			<< mysqlpp::quote << packet[pos] << ","
			<< mysqlpp::quote << packet[pos+1] << ","
			<< mysqlpp::quote << packet[pos+2] << ","
			<< mysqlpp::quote << packet[pos+3] << ","
			<< mysqlpp::quote << packet[pos+4] << ")";
	std::cout << query.str();
	mysqlpp::SimpleResult res = query.execute();
	int id = (int) res.insert_id();

	pos -= 16;
	createEquip(BufferUtilities::getInt(packet+pos), 0x05, id);
	pos+=4;
	createEquip(BufferUtilities::getInt(packet+pos), 0x06, id);
	pos+=4;
	createEquip(BufferUtilities::getInt(packet+pos), 0x07, id);
	pos+=4;
	createEquip(BufferUtilities::getInt(packet+pos), 0x0b, id);
	pos+=4;

	query << "INSERT INTO items VALUES (4161001, " << mysqlpp::quote << id << ", 4, 1, 1)"; // Beginner Guide
	query.exec();

	query << "INSERT INTO keymap (charid) VALUES (" << mysqlpp::quote << id << ")"; // Default keymap
	query.exec();

	query << "SELECT * FROM characters WHERE id = " << mysqlpp::quote << id << " LIMIT 1";
	mysqlpp::StoreQueryResult res2 = query.store();

	loadCharacter(charc, res2[0]);
	LoginPacket::showCharacter(player, charc);
}

void Characters::deleteCharacter(PlayerLogin* player, unsigned char *packet){
	int data = BufferUtilities::getInt(packet);
	int id = BufferUtilities::getInt(packet+4);
	
	if(!ownerCheck(player, id)){
		// hacking
		return;
	}

	mysqlpp::Query query = db.query();

	query << "DELETE FROM characters WHERE id = " << mysqlpp::quote << id;
	query.exec();

	query << "DELETE FROM equip WHERE charid = " << mysqlpp::quote << id;
	query.exec();

	query << "DELETE FROM keymap WHERE charid = " << mysqlpp::quote << id;
	query.exec();

	query << "DELETE FROM items WHERE charid = " << mysqlpp::quote << id;
	query.exec();

	query << "DELETE FROM skills WHERE charid = " << mysqlpp::quote << id;
	query.exec();

	LoginPacket::deleteCharacter(player, id);
}


void Characters::connectGame(PlayerLogin* player, unsigned char *packet){
	int id = BufferUtilities::getInt(packet);

	if(!ownerCheck(player, id)){
		// hacking
		return;
	}

	LoginServerAcceptPlayerPacket::newPlayer(Worlds::worlds[player->getWorld()]->player, player->getChannel(), id);
	LoginPacket::connectIP(player, id);
}

bool Characters::ownerCheck(PlayerLogin* player, int id) {
	mysqlpp::Query query = db.query();
	query << "SELECT true FROM characters WHERE id = " << mysqlpp::quote << id << " AND userid = " << mysqlpp::quote << player->getUserid();
	mysqlpp::StoreQueryResult res = query.store();

	return (res.num_rows() == 1) ? 1 : 0 ;
}

bool Characters::nameTaken(PlayerLogin* player, char *name) {
	mysqlpp::Query query = db.query();
	query << "SELECT true FROM characters WHERE name = " << mysqlpp::quote << name  << " AND world_id = " << mysqlpp::quote << (int) player->getWorld() << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();

	return (res.num_rows() == 1) ? 1 : 0 ;
}
