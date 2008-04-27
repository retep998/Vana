#include "Characters.h"
#include "PlayerLogin.h"
#include "LoginPacket.h"
#include "BufferUtilities.h"
#include "MySQLM.h"

void Characters::showEquips(int id, vector <CharEquip> &vec){
	int equips[30][2];
	int many = MySQL::showEquips(id, equips);
	for(int i=0; i<many; i++){
		CharEquip equip; 
		equip.id = equips[i][0];
		equip.type = equips[i][1];
		vec.push_back(equip);
	}	
}

void Characters::showCharacters(PlayerLogin* player){
	int IDs[3];
	int charnum = MySQL::getCharactersIDs(player->getUserid(), IDs);
	player->setIDs(IDs, charnum);
	vector <Character> chars;
	for(int i=0; i<charnum; i++){
		Character charc;
		charc.id = IDs[i];
		MySQL::getStringI("characters", "ID", IDs[i], "name", charc.name);
		charc.gender = MySQL::getInt("characters", IDs[i], "gender");
		charc.skin = MySQL::getInt("characters", IDs[i], "skin");
		charc.eyes = MySQL::getInt("characters", IDs[i], "eyes");
		charc.hair = MySQL::getInt("characters", IDs[i], "hair");
		charc.level = MySQL::getInt("characters", IDs[i], "level");
		charc.job = MySQL::getInt("characters", IDs[i], "job");
		charc.str = MySQL::getInt("characters", IDs[i], "str");
		charc.dex = MySQL::getInt("characters", IDs[i], "dex");
		charc.intt = MySQL::getInt("characters", IDs[i], "intt");
		charc.luk = MySQL::getInt("characters", IDs[i], "luk");
		charc.hp = MySQL::getInt("characters", IDs[i], "chp");
		charc.mhp = MySQL::getInt("characters", IDs[i], "mhp");
		charc.mp = MySQL::getInt("characters", IDs[i], "cmp");
		charc.mmp = MySQL::getInt("characters", IDs[i], "mmp");
		charc.ap = MySQL::getInt("characters", IDs[i], "ap");
		charc.sp = MySQL::getInt("characters", IDs[i], "sp");
		charc.exp= MySQL::getInt("characters", IDs[i], "exp");
		charc.fame = MySQL::getInt("characters", IDs[i], "fame");
		charc.map = MySQL::getInt("characters", IDs[i], "map");
		charc.pos = MySQL::getInt("characters", IDs[i], "pos");
		showEquips(IDs[i], charc.equips);
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
	if(MySQL::isString("characters", "name", charactername))
		is = 1;
	LoginPacket::checkName(player, is, charactername);
}

void Characters::createEquip(int equipid, int type, int charid){
	char query[255]; 
	if(type==0x05)
		sprintf_s(query, 255, "insert into equip(equipid,charid,type,iwdef,pos) values(%d, %d, %d, 3, %d);", equipid, charid, type, -type);
	else if(type==0x06)
		sprintf_s(query, 255, "insert into equip(equipid,charid,type,iwdef,pos) values(%d, %d, %d, 2, %d);", equipid, charid, type, -type);
	else if(type==0x07)
		sprintf_s(query, 255, "insert into equip(equipid,charid,type,iwdef,slots,pos) values(%d, %d, %d, 2, 5, %d);", equipid, charid, type, -type);
	else if(type==0x0b)
		sprintf_s(query, 255, "insert into equip(equipid,charid,type,iwatk,pos) values(%d, %d, %d, 17, %d);", equipid, charid, type ,-type);
	MySQL::insert(query);
}

void Characters::createCharacter(PlayerLogin* player, unsigned char* packet){
	Character charc;
	char charactername[15];
	int id = MySQL::setChar(player->getUserid());
	charc.id = id;
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
	pos+=4;
	createEquip(getInt(packet+pos), 0x05, id);
	pos+=4;
	createEquip(getInt(packet+pos), 0x06, id);
	pos+=4;
	createEquip(getInt(packet+pos), 0x07, id);
	pos+=4;
	createEquip(getInt(packet+pos), 0x0b, id);
	pos+=4;
	if(packet[pos+1] + packet[pos+2] + packet[pos+3] + packet[pos+4] != 25){
		// hacking
		return;
	}
	char query[255]; 
	sprintf_s(query, 255, "insert into items values(4161001, %d, 4, 1, 1);", id);
	MySQL::insert(query);
    sprintf_s(query, 255, "name='%s', eyes=%d, hair=%d, skin=%d, gender=%d, str=%d, dex=%d, intt=%d, luk=%d", charactername, eyes, hair, skin, packet[pos], packet[pos+1],packet[pos+2], packet[pos+3], packet[pos+4]);
	MySQL::charInfo(query, id);
    sprintf_s(query, 255, "insert into keymap(charid) values(%d);", id);
	MySQL::insert(query);
	MySQL::getStringI("characters", "ID", id, "name", charc.name);
	charc.gender = MySQL::getInt("characters", id, "gender");
	charc.skin = MySQL::getInt("characters", id, "skin");
	charc.eyes = MySQL::getInt("characters", id, "eyes");
	charc.hair = MySQL::getInt("characters", id, "hair");
	charc.level = MySQL::getInt("characters", id, "level");
	charc.job = MySQL::getInt("characters", id, "job");
	charc.str = MySQL::getInt("characters", id, "str");
	charc.dex = MySQL::getInt("characters", id, "dex");
	charc.intt = MySQL::getInt("characters", id, "intt");
	charc.luk = MySQL::getInt("characters", id, "luk");
	charc.hp = MySQL::getInt("characters", id, "chp");
	charc.mhp = MySQL::getInt("characters", id, "mhp");
	charc.mp = MySQL::getInt("characters", id, "cmp");
	charc.mmp = MySQL::getInt("characters", id, "mmp");
	charc.ap = MySQL::getInt("characters", id, "ap");
	charc.sp = MySQL::getInt("characters", id, "sp");
	charc.exp= MySQL::getInt("characters", id, "exp");
	charc.fame = MySQL::getInt("characters", id, "fame");
	charc.map = MySQL::getInt("characters", id, "map");
	charc.pos = MySQL::getInt("characters", id, "pos");
	showEquips(id, charc.equips);
	LoginPacket::showCharacter(player, charc);
	int IDs[3];
	int charnum = MySQL::getCharactersIDs(player->getUserid(), IDs);
	player->setIDs(IDs, charnum);
}

void Characters::deleteCharacter(PlayerLogin* player, unsigned char *packet){
	int data = getInt(packet);
	int ID = getInt(packet+4);
	bool check=false;
	int ids[3];
	int num = player->getIDs(ids);
	for(int i=0; i<num; i++){
		if(ids[i] == ID){
			check=true;
			break;
		}
	}
	if(!check){
		// hacking
		return;
	}
	MySQL::deleteRow("characters", ID);
	char sql[200];
	sprintf_s(sql, 200, "delete from equip where charid=%d;", ID);
	MySQL::insert(sql);
	sprintf_s(sql, 200, "delete from keymap where charid=%d;", ID);
	MySQL::insert(sql);
	sprintf_s(sql, 200, "delete from items where charid=%d;", ID);
	MySQL::insert(sql);
	sprintf_s(sql, 200, "delete from skills where charid=%d;", ID);
	MySQL::insert(sql);
	LoginPacket::deleteCharacter(player, ID);
	int IDs[3];
	int charnum = MySQL::getCharactersIDs(player->getUserid(), IDs);
	player->setIDs(IDs, charnum);
}


void Characters::connectGame(PlayerLogin* player, unsigned char *packet){
	int charid = getInt(packet);
	bool check=false;
	int ids[3];
	int num = player->getIDs(ids);
	for(int i=0; i<num; i++){
		if(ids[i] == charid){
			check=true;
			break;
		}
	}
	if(!check){
		// hacking
		return;
	}
	LoginPacket::connectIP(player, charid);
}
