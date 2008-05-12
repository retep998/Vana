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
#include "LoginPacket.h"
#include "PlayerLogin.h"
#include "PacketCreator.h"
#include "Worlds.h"
#include "Characters.h"
#include "SendHeader.h"

void LoginPacket::loginError(PlayerLogin* player, short errorid){
	Packet packet = Packet();
	packet.addHeader(SEND_LOGIN_INFO_REPLY);
	packet.addShort(errorid);
	packet.addInt(0);
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::loginBan(PlayerLogin* player, char reason, int expire){
	/* Reasons:
		00 -> This is an ID that has been deleted or blocked from connection
		01 -> Your account has been blocked for hacking or illegal use of third-party programs
		02 -> Your account has been blocked for using macro/auto-keyboard
		03 -> Your account has been blocked for illicit promotion or advertising
		04 -> Your account has been blocked for harassment
		05 -> Your account has been blocked for using profane language
		06 -> Your account has been blocked for scamming
		07 -> Your account has been blocked for misconduct
		08 -> Your account has been blocked for illegal cash transaction
		09 -> Your account has been blocked for illegal charging/funding. Please contact customer support for further details
		10 -> Your account has been blocked for temporary request. Please contact customer support for further details
		11 -> Your account has been blocked for impersonating GM
		12 -> Your account has been blocked for using illegal programs or violating the game policy
		13 -> Your account has been blocked for one of cursing, scamming, or illegal trading via Megaphones.
	*/
	Packet packet = Packet();
	packet.addHeader(SEND_LOGIN_INFO_REPLY);
	packet.addBytes("020000000000");
	packet.addByte(reason);
	packet.addBytes("00000000");
	packet.addInt(expire); //Ban over: Time, anything >= 00aacb01 (year >= 2011) will cause perma ban
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::loginConnect(PlayerLogin* player, char* username, int size){
	Packet packet = Packet();
	packet.addHeader(SEND_LOGIN_INFO_REPLY);
	packet.addInt(0);
	packet.addShort(0);
	packet.addInt(player->getUserid());
	if(player->getStatus() == 1)
		packet.addByte(0x0B);
	else
		packet.addByte(player->getGender());
	packet.addBytes("0465");
	packet.addShort(size);
	packet.addString(username, size);
	packet.addInt(0);
	packet.addInt(0);
	packet.addBytes("000000A6B89C2B4CC701");
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::loginProcess(PlayerLogin* player, char id){
	Packet packet = Packet();
	packet.addHeader(SEND_LOGIN_PROCESS);
	packet.addByte(id);
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::pinAssigned(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(SEND_PIN_ASSIGNED);
	packet.addByte(0);
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::showWorld(PlayerLogin* player, World world){
	Packet packet = Packet();
	packet.addHeader(SEND_SHOW_WORLD);
	packet.addByte(world.id);
	packet.addShort(strlen(world.name));
	packet.addString(world.name, strlen(world.name));
	packet.addByte(world.ribbon);
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
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::worldEnd(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(SEND_SHOW_WORLD);
	packet.addByte(0xFF);
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::showChannels(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(SEND_SHOW_CHANNEL);
	packet.addShort(0x00);
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::channelSelect(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(SEND_CHANNEL_SELECT);
	packet.addBytes("000500001040008612340097227400");
	packet.addInt(4);
	packet.addBytes("9F227400");
	packet.addInt(4);
	packet.addBytes("AC227400544D0500F073790028BC0000");
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::showCharacters(PlayerLogin* player, vector <Character> chars){
	Packet packet = Packet();
	packet.addHeader(SEND_SHOW_CHARACTERS);
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
		packet.addInt(0);
		packet.addInt(0);
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
		packet.addInt(0);
		packet.addInt(0);
		packet.addByte(0);
	}
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::checkName(PlayerLogin* player, char is, char* name){
	Packet packet = Packet();
	packet.addHeader(SEND_CHECK_NAME);
	packet.addShort(strlen(name));
	packet.addString(name, strlen(name));
	packet.addByte(is);
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::showCharacter(PlayerLogin* player, Character charc){
	Packet packet = Packet();
	packet.addHeader(SEND_SHOW_CHARACTER);
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
	packet.addInt(0);
	packet.addInt(0);
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
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(0);
	packet.packetSend<PlayerLogin>(player);	
}

void LoginPacket::deleteCharacter(PlayerLogin* player, int ID){
	Packet packet = Packet();
	packet.addHeader(SEND_DELETE_CHAR);
	packet.addInt(ID);
	packet.addByte(0);
	packet.packetSend<PlayerLogin>(player);
}

void LoginPacket::connectIP(PlayerLogin* player, int charid){
	Packet packet = Packet();
	packet.addHeader(SEND_CHANNEL_SERVER_INFO);
	packet.addShort(0); // IP
	
	packet.addByte(127); // IP
	packet.addByte(0);
	packet.addByte(0);
	packet.addByte(1);

	packet.addShort(8888 + player->getChannel());
	packet.addInt(charid);
	packet.addInt(0);
	packet.addByte(0);
	packet.packetSend<PlayerLogin>(player);
}


void LoginPacket::logBack(PlayerLogin* player){
	Packet packet = Packet();
	packet.addHeader(SEND_RETURN_TO_LOGIN);
	packet.addByte(1);
	packet.packetSend<PlayerLogin>(player);
}