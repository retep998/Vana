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
#include "BufferUtilities.h"
#include "MySQLM.h"

void Characters::showEquips(int id, vector <CharEquip> &vec){
	mysqlpp::Query query = db.query();
	query << "SELECT equipid, type FROM equip WHERE charid = " << mysqlpp::quote << id << " AND pos < 0";
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		CharEquip equip; 
		equip.id = res[i][0];
		equip.type = (unsigned char) res[i][1];
		vec.push_back(equip);
	}	
}

void Characters::showCharacters(PlayerLogin* player){
	mysqlpp::Query query = db.query();
	query << "SELECT * FROM characters WHERE userid = " << mysqlpp::quote << player->getUserid();
	mysqlpp::StoreQueryResult res = query.store();

	vector <Character> chars;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		Character charc;
		charc.id = res[i]["id"];
		strcpy_s(charc.name, res[i]["name"]);
		charc.gender = (unsigned char) res[i]["gender"];
		charc.skin = (unsigned char) res[i]["skin"];
		charc.eyes = res[i]["eyes"];
		charc.hair = res[i]["hair"];
		charc.level = (unsigned char) res[i]["level"];
		charc.job = res[i]["job"];
		charc.str = res[i]["str"];
		charc.dex = res[i]["dex"];
		charc.intt = res[i]["intt"];
		charc.luk = res[i]["luk"];
		charc.hp = res[i]["chp"];
		charc.mhp = res[i]["mhp"];
		charc.mp = res[i]["cmp"];
		charc.mmp = res[i]["mmp"];
		charc.ap = res[i]["ap"];
		charc.sp = res[i]["sp"];
		charc.exp= res[i]["exp"];
		charc.fame = res[i]["fame"];
		charc.map = res[i]["map"];
		charc.pos = (unsigned char) res[i]["pos"];
		showEquips(charc.id, charc.equips);
		chars.push_back(charc);
	}
	LoginPacket::showCharacters(player, chars);
}

void Characters::checkCharacterName(PlayerLogin* player, unsigned char* packet){
	char charactername[15];
	int size = packet[0];
	char is = 0;
	if(size>15){
		return;
	}
	getString(packet+2, size, charactername);
	
	mysqlpp::Query query = db.query();
	query << "SELECT true FROM characters WHERE name = " << mysqlpp::quote << charactername << " LIMIT 1";
	mysqlpp::SimpleResult res = query.execute();

	LoginPacket::checkName(player, res.rows() != 0, charactername);
}

void Characters::createEquip(int equipid, int type, int charid){
	mysqlpp::Query query = db.query();
	if(type==0x05)
		query << "INSERT INTO equip (equipid,charid,type,iwdef,pos) VALUES (" << mysqlpp::quote << equipid << "," << mysqlpp::quote << charid << "," << mysqlpp::quote << type << 3 << "," << mysqlpp::quote << -type << ")";
	else if(type==0x06)
		query << "INSERT INTO equip (equipid,charid,type,iwdef,pos) VALUES (" << mysqlpp::quote << equipid << "," << mysqlpp::quote << charid << "," << mysqlpp::quote << type << 2 << "," << mysqlpp::quote << -type << ")";
	else if(type==0x07)
		query << "INSERT INTO equip (equipid,charid,type,iwdef,slots,pos) VALUES (" << mysqlpp::quote << equipid << "," << mysqlpp::quote << charid << "," << mysqlpp::quote << type << 3 << "," << 5 << "," << mysqlpp::quote << -type << ")";
	else if(type==0x0b)
		query << "INSERT INTO equip (equipid,charid,type,iwatk,pos) VALUES (" << mysqlpp::quote << equipid << "," << mysqlpp::quote << charid << "," << mysqlpp::quote << type << 17 << "," << mysqlpp::quote << -type << ")";
	query.exec();
}

void Characters::createCharacter(PlayerLogin* player, unsigned char* packet){
	Character charc;
	vector <Character> chars;
	char charactername[15];
	int size = packet[0];
	if(size>15){
		return;
	}
	getString(packet+2, size, charactername);
	int pos = 2+size;
	int eyes = getInt(packet+pos);
	pos+=4;
	int hair = getInt(packet+pos);
	pos+=8;
	int skin = getInt(packet+pos);
	pos+=20;
	if(packet[pos+1] + packet[pos+2] + packet[pos+3] + packet[pos+4] != 25){
		// hacking
		return;
	}
	mysqlpp::Query query = db.query();
	query << "INSERT INTO characters (name, eyes, hair, skin, gender, str, dex, intt, luk) VALUES (" 
			<< mysqlpp::quote << charactername << ","
			<< mysqlpp::quote << eyes << ","
			<< mysqlpp::quote << hair << ","
			<< mysqlpp::quote << skin << ","
			<< mysqlpp::quote << packet[pos] << ","
			<< mysqlpp::quote << packet[pos+1] << ","
			<< mysqlpp::quote << packet[pos+2] << ","
			<< mysqlpp::quote << packet[pos+3] << ","
			<< mysqlpp::quote << packet[pos+4] << ")";
	mysqlpp::SimpleResult res = query.execute();
	int id = (int) res.insert_id();

	pos -= 16;
	createEquip(getInt(packet+pos), 0x05, id);
	pos+=4;
	createEquip(getInt(packet+pos), 0x06, id);
	pos+=4;
	createEquip(getInt(packet+pos), 0x07, id);
	pos+=4;
	createEquip(getInt(packet+pos), 0x0b, id);
	pos+=4;

	query << "INSERT INTO items VALUES (4161001, " << mysqlpp::quote << id << ", 4, 1, 1)"; // Beginner Guide
	query.exec();

	query << "INSERT INTO keymap (charid) VALUES (" << mysqlpp::quote << id << ")"; // Default keymap
	query.exec();

	query << "SELECT * FROM characters WHERE id = " << mysqlpp::quote << id << " LIMIT 1"; //TODO: Refactorr
	mysqlpp::StoreQueryResult res2 = query.store();

	charc.id = res2[0]["id"];
	strcpy_s(charc.name, res2[0]["name"]);
	charc.gender = (unsigned char) res2[0]["gender"];
	charc.skin = (unsigned char) res2[0]["skin"];
	charc.eyes = res2[0]["eyes"];
	charc.hair = res2[0]["hair"];
	charc.level = (unsigned char) res2[0]["level"];
	charc.job = res2[0]["job"];
	charc.str = res2[0]["str"];
	charc.dex = res2[0]["dex"];
	charc.intt = res2[0]["intt"];
	charc.luk = res2[0]["luk"];
	charc.hp = res2[0]["chp"];
	charc.mhp = res2[0]["mhp"];
	charc.mp = res2[0]["cmp"];
	charc.mmp = res2[0]["mmp"];
	charc.ap = res2[0]["ap"];
	charc.sp = res2[0]["sp"];
	charc.exp= res2[0]["exp"];
	charc.fame = res2[0]["fame"];
	charc.map = res2[0]["map"];
	charc.pos = (unsigned char) res2[0]["pos"];
	showEquips(charc.id, charc.equips);
	chars.push_back(charc);
}

void Characters::deleteCharacter(PlayerLogin* player, unsigned char *packet){
	int data = getInt(packet);
	int id = getInt(packet+4);
	
	mysqlpp::Query query = db.query();
	query << "SELECT true FROM characters WHERE id = " << mysqlpp::quote << id << " AND userid = " << mysqlpp::quote << player->getUserid();
	mysqlpp::StoreQueryResult res = query.store();

	if(!res.num_rows()){
		// hacking
		return;
	}

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
	int id = getInt(packet);
	
	mysqlpp::Query query = db.query(); //TODO: Refactor
	query << "SELECT true FROM characters WHERE id = " << mysqlpp::quote << id << " AND userid = " << mysqlpp::quote << player->getUserid();
	mysqlpp::StoreQueryResult res = query.store();

	if(!res.num_rows()){
		// hacking
		return;
	}

	LoginPacket::connectIP(player, id);
}
