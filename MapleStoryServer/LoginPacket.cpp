#include "LoginPacket.h"
#include "PlayerLogin.h"
#include "PacketCreator.h"
#include "Worlds.h"
#include "Characters.h" 

void LoginPacket::loginError(PlayerLogin* player, short errorid){
	Packet packet = Packet();
	packet.addHeader(0x00);
	packet.addShort(errorid);
	packet.addInt(0);
	packet.packetSendLogin(player);
}

void LoginPacket::loginConnect(PlayerLogin* player, char* username, int size){
	Packet packet = Packet();
	packet.addHeader(0x00);
	packet.addInt(0);
	packet.addShort(0);
	packet.addBytes("2A7949");
	packet.addByte(0);
	if(player->getStatus() == 1)
		packet.addByte(0x0A);
	else
		packet.addByte(player->getGender());
	packet.addBytes("0465");
	packet.addShort(size);
	packet.addString(username, size);
	packet.addInt(0);
	packet.addInt(0);
	packet.addBytes("000000A6B89C2B4CC701");
	packet.packetSendLogin(player);
}

void LoginPacket::loginProcess(PlayerLogin* player, char id){
	Packet packet = Packet();
	packet.addHeader(0x0d);
	packet.addByte(id);
	packet.packetSendLogin(player);
}

void LoginPacket::processOk(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(0x06);
	packet.addByte(0);
	packet.packetSendLogin(player);

}

void LoginPacket::showWorld(PlayerLogin* player, World world){
	Packet packet = Packet();
	packet.addHeader(0x05);
	packet.addByte(world.id);
	packet.addShort(strlen(world.name));
	packet.addString(world.name, strlen(world.name));
	packet.addByte(0); //Type 2-new
	packet.addShort(0);
	packet.addShort(100);
	packet.addByte(100);
	packet.addShort(0);
	packet.addByte(world.channels);
	for(int i=0; i<world.channels; i++){
		char channelname[15];
		strcpy_s(channelname, 15, world.name);
		strcat_s(channelname, 15, "-");
		char cid[15]={0};
		cid[0] = i+'1';
		strcat_s(channelname, 15, cid);
		packet.addShort(strlen(channelname));
		packet.addString(channelname, strlen(channelname));
		packet.addInt(0x0); // Pop
		packet.addByte(world.id);
		packet.addShort(i);
	}
	packet.packetSendLogin(player);
}

void LoginPacket::worldEnd(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(0x05);
	packet.addByte(0xFF);
	packet.packetSendLogin(player);
}

void LoginPacket::showChannels(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(0x12);
	packet.addShort(0x00);
	packet.packetSendLogin(player);
}

void LoginPacket::channelSelect(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(0x10);
	packet.addBytes("000500001040008612340097227400");
	packet.addInt(4);
	packet.addBytes("9F227400");
	packet.addInt(4);
	packet.addBytes("AC227400544D0500F073790028BC0000");
	packet.packetSendLogin(player);
}

void LoginPacket::showCharacters(PlayerLogin* player, vector <Character> chars){
	Packet packet = Packet();
	packet.addHeader(0x13);
	packet.addByte(0);
	packet.addByte(chars.size());
	for(int i=0; i<(int)chars.size(); i++){
		packet.addInt(chars[i].id);
		packet.addString(chars[i].name, 12);
		packet.addByte(0);
		packet.addByte(chars[i].gender);
		packet.addByte(chars[i].skin);
		packet.addInt(chars[i].eyes);
		packet.addInt(chars[i].hair);
		packet.addInt(0);
		packet.addInt(0);
		packet.addByte(chars[i].level);
		packet.addShort(chars[i].job);
		packet.addShort(chars[i].str);
		packet.addShort(chars[i].dex);
		packet.addShort(chars[i].intt);
		packet.addShort(chars[i].luk);
		packet.addShort(chars[i].hp);
		packet.addShort(chars[i].mhp);
		packet.addShort(chars[i].mp);
		packet.addShort(chars[i].mmp);
		packet.addShort(chars[i].ap);
		packet.addShort(chars[i].sp);
		packet.addInt(chars[i].exp);
		packet.addShort(chars[i].fame);
		packet.addInt(chars[i].map);
		packet.addByte(chars[i].pos);
		packet.addByte(chars[i].gender);
		packet.addByte(chars[i].skin);
		packet.addInt(chars[i].eyes);
		packet.addByte(1);
		packet.addInt(chars[i].hair);
		for(int j=0; j<(int)chars[i].equips.size(); j++){
			packet.addByte(chars[i].equips[j].type);
			packet.addInt(chars[i].equips[j].id);
		}
		packet.addShort(-1);
		packet.addInt(0);
		packet.addInt(0);
		packet.addByte(0);
	}
	packet.packetSendLogin(player);
}

void LoginPacket::checkName(PlayerLogin* player, char is, char* name){
	Packet packet = Packet();
	packet.addHeader(0x06);
	packet.addShort(strlen(name));
	packet.addString(name, strlen(name));
	packet.addByte(is);
	packet.packetSendLogin(player);
}

void LoginPacket::showCharacter(PlayerLogin* player, Character charc){
	Packet packet = Packet();
	packet.addHeader(0x07);
	packet.addByte(0);
	packet.addInt(charc.id);
	packet.addString(charc.name, 12);
	packet.addByte(0);
	packet.addByte(charc.gender);
	packet.addByte(charc.skin);
	packet.addInt(charc.eyes);
	packet.addInt(charc.hair);
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(charc.level);
	packet.addShort(charc.job);
	packet.addShort(charc.str);
	packet.addShort(charc.dex);
	packet.addShort(charc.intt);
	packet.addShort(charc.luk);
	packet.addShort(charc.hp);
	packet.addShort(charc.mhp);
	packet.addShort(charc.mp);
	packet.addShort(charc.mmp);
	packet.addShort(charc.ap);
	packet.addShort(charc.sp);
	packet.addInt(charc.exp);
	packet.addShort(charc.fame);
	packet.addInt(charc.map);
	packet.addByte(charc.pos);
	packet.addByte(charc.gender);
	packet.addByte(charc.skin);
	packet.addInt(charc.eyes);
	packet.addByte(1);
	packet.addInt(charc.hair);
	for(int j=0; j<(int)charc.equips.size(); j++){
		packet.addByte(charc.equips[j].type);
		packet.addInt(charc.equips[j].id);
	}
	packet.addShort(-1);
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(0);
	packet.packetSendLogin(player);	
}

void LoginPacket::deleteCharacter(PlayerLogin* player, int ID){
	Packet packet = Packet();
	packet.addHeader(0x08);
	packet.addInt(ID);
	packet.addByte(0);
	packet.packetSendLogin(player);
}

void LoginPacket::connectIP(PlayerLogin* player, int charid){
	Packet packet = Packet();
	packet.addHeader(0x04);
	packet.addShort(0); // IP
	
	packet.addByte(127); // IP
	packet.addByte(0);
	packet.addByte(0);
	packet.addByte(1);

	packet.addShort(8888 + player->getChannel());
	packet.addInt(charid);
	packet.addInt(0);
	packet.addByte(0);
	packet.packetSendLogin(player);
}


void LoginPacket::logBack(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(0x13);
	packet.addByte(1);
	packet.packetSendLogin(player);
}