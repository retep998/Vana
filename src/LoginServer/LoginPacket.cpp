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
#include "LoginPacketHelper.h"
#include "PlayerLogin.h"
#include "PacketCreator.h"
#include "Worlds.h"
#include "Characters.h"
#include "SendHeader.h"

void LoginPacket::loginError(PlayerLogin* player, short errorid) {
	Packet packet;
	packet.addHeader(SEND_LOGIN_INFO_REPLY);
	packet.addShort(errorid);
	packet.addInt(0);
	packet.send(player);
}

void LoginPacket::loginBan(PlayerLogin* player, char reason, int expire) {
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
	Packet packet;
	packet.addHeader(SEND_LOGIN_INFO_REPLY);
	packet.addBytes("020000000000");
	packet.addByte(reason);
	packet.addBytes("00000000");
	packet.addInt(expire); //Ban over: Time, anything >= 00aacb01 (year >= 2011) will cause perma ban
	packet.send(player);
}

void LoginPacket::loginConnect(PlayerLogin* player, const string &username) {
	Packet packet;
	packet.addHeader(SEND_LOGIN_INFO_REPLY);
	packet.addInt(0);
	packet.addShort(0);
	packet.addInt(player->getUserid());
	switch (player->getStatus()) {
		case 5: packet.addByte(0x0a); break; // Gender Select
		case 1: packet.addByte(0x0b); break; // Pin Select
		default: packet.addByte(player->getGender()); break;
	}
	packet.addBytes("0465");
	packet.addString(username);
	packet.addInt(0);
	packet.addInt(0);
	packet.addBytes("000000A6B89C2B4CC701");
	packet.addInt(0);
	packet.send(player);
}

void LoginPacket::loginProcess(PlayerLogin* player, char id) {
	Packet packet;
	packet.addHeader(SEND_LOGIN_PROCESS);
	packet.addByte(id);
	packet.send(player);
}

void LoginPacket::pinAssigned(PlayerLogin* player) {
	Packet packet;
	packet.addHeader(SEND_PIN_ASSIGNED);
	packet.addByte(0);
	packet.send(player);
}

void LoginPacket::genderDone(PlayerLogin* player, char gender) {
	Packet packet;
	packet.addHeader(SEND_GENDER_DONE);
	packet.addByte(gender);
	packet.addByte(1);
	packet.send(player);
}

void LoginPacket::showWorld(PlayerLogin* player, World *world) {
	Packet packet;
	packet.addHeader(SEND_SHOW_WORLD);
	packet.addByte(world->id);
	packet.addShort(world->name.size());
	packet.addString(world->name.c_str(), world->name.size());
	packet.addByte(world->ribbon);
	packet.addShort(0);
	packet.addShort(100);
	packet.addByte(100);
	packet.addShort(0);
	packet.addByte(world->channels.size());
	for (size_t i=0; i<world->channels.size(); i++) {
		ostringstream cnStream;
		cnStream << world->name << "-" << i+1;
		string channelname = cnStream.str();
		packet.addShort(channelname.size());
		packet.addString(channelname.c_str(), channelname.size());
		packet.addInt(world->channels[i]->population);
		packet.addByte(world->id);
		packet.addShort(i);
	}
	packet.send(player);
}

void LoginPacket::worldEnd(PlayerLogin* player) {
	Packet packet;
	packet.addHeader(SEND_SHOW_WORLD);
	packet.addByte(0xFF);
	packet.send(player);
}

void LoginPacket::showChannels(PlayerLogin* player) {
	Packet packet;
	packet.addHeader(SEND_SHOW_CHANNEL);
	packet.addShort(0x00);
	packet.send(player);
}

void LoginPacket::channelSelect(PlayerLogin* player) {
	Packet packet;
	packet.addHeader(SEND_CHANNEL_SELECT);
	packet.addBytes("000500001040008612340097227400");
	packet.addInt(4);
	packet.addBytes("9F227400");
	packet.addInt(4);
	packet.addBytes("AC227400544D0500F073790028BC0000");
	packet.send(player);
}

void LoginPacket::showCharacters(PlayerLogin* player, vector <Character> chars) {
	Packet packet;
	packet.addHeader(SEND_SHOW_CHARACTERS);
	packet.addByte(0);
	packet.addByte(chars.size());
	for (int i=0; i<(int)chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i]);
	}
	packet.send(player);
}

void LoginPacket::checkName(PlayerLogin* player, char is, const string &name) {
	Packet packet;
	packet.addHeader(SEND_CHECK_NAME);
	packet.addString(name);
	packet.addByte(is);
	packet.send(player);
}

void LoginPacket::showCharacter(PlayerLogin* player, Character charc) {
	Packet packet;
	packet.addHeader(SEND_SHOW_CHARACTER);
	packet.addByte(0);
	LoginPacketHelper::addCharacter(packet, charc);
	packet.send(player);	
}

void LoginPacket::deleteCharacter(PlayerLogin* player, int ID) {
	Packet packet;
	packet.addHeader(SEND_DELETE_CHAR);
	packet.addInt(ID);
	packet.addByte(0);
	packet.send(player);
}

void LoginPacket::connectIP(PlayerLogin* player, int charid) {
	Packet packet;
	packet.addHeader(SEND_CHANNEL_SERVER_INFO);
	packet.addShort(0);
	
	char ip[15];
	strcpy_s(ip, Worlds::worlds[player->getWorld()]->channels[player->getChannel()]->ip.c_str());
	char *next_token;

	packet.addByte(atoi(strtok_s(ip, ".", &next_token))); // IP
	packet.addByte(atoi(strtok_s(NULL, ".", &next_token)));
	packet.addByte(atoi(strtok_s(NULL, ".", &next_token)));
	packet.addByte(atoi(strtok_s(NULL, ".", &next_token)));

	packet.addShort(Worlds::worlds[player->getWorld()]->channels[player->getChannel()]->port);
	packet.addInt(charid);
	packet.addInt(0);
	packet.addByte(0);
	packet.send(player);
}


void LoginPacket::logBack(PlayerLogin* player) {
	Packet packet;
	packet.addHeader(SEND_RETURN_TO_LOGIN);
	packet.addByte(1);
	packet.send(player);
}
