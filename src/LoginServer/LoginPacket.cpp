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
#include "Characters.h"
#include "LoginPacketHelper.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PlayerLogin.h"
#include "SendHeader.h"
#include "Worlds.h"

void LoginPacket::loginError(PlayerLogin *player, int16_t errorid) {
	PacketCreator packet;
	packet.addShort(SEND_LOGIN_INFO_REPLY);
	packet.addShort(errorid);
	packet.addInt(0);
	player->getSession()->send(packet);
}

void LoginPacket::loginBan(PlayerLogin *player, int8_t reason, int32_t expire) {
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
	PacketCreator packet;
	packet.addShort(SEND_LOGIN_INFO_REPLY);
	packet.addBytes("020000000000");
	packet.addByte(reason);
	packet.addBytes("00000000");
	packet.addInt(expire); // Ban over: Time, anything >= 00aacb01 (year >= 2011) will cause perma ban
	player->getSession()->send(packet);
}

void LoginPacket::loginConnect(PlayerLogin *player, const string &username) {
	PacketCreator packet;
	packet.addShort(SEND_LOGIN_INFO_REPLY);
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
	player->getSession()->send(packet);
}

void LoginPacket::loginProcess(PlayerLogin *player, int8_t id) {
	PacketCreator packet;
	packet.addShort(SEND_LOGIN_PROCESS);
	packet.addByte(id);
	player->getSession()->send(packet);
}

void LoginPacket::pinAssigned(PlayerLogin *player) {
	PacketCreator packet;
	packet.addShort(SEND_PIN_ASSIGNED);
	packet.addByte(0);
	player->getSession()->send(packet);
}

void LoginPacket::genderDone(PlayerLogin *player, int8_t gender) {
	PacketCreator packet;
	packet.addShort(SEND_GENDER_DONE);
	packet.addByte(gender);
	packet.addByte(1);
	player->getSession()->send(packet);
}

void LoginPacket::showWorld(PlayerLogin *player, World *world) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_WORLD);
	packet.addByte(world->id);
	packet.addString(world->name);
	packet.addByte(world->ribbon);
	packet.addString(world->eventMsg);
	packet.addShort(100);
	packet.addByte(100);
	packet.addShort(0);
	packet.addByte(world->maxChannels);
	for (size_t i = 0; i < world->maxChannels; i++) {
		std::ostringstream cnStream;
		cnStream << world->name << "-" << i+1;
		string channelname = cnStream.str();
		packet.addString(channelname);

		if (world->channels.find(i) != world->channels.end()) {
			packet.addInt(world->channels[i]->population);
		}
		else { // Channel doesn't exist
			packet.addInt(0);
		}
		packet.addByte(1); // Char creation (the part that is server-decided)
		packet.addShort(i);
	}
	packet.addShort(0);
	player->getSession()->send(packet);
}

void LoginPacket::worldEnd(PlayerLogin *player) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_WORLD);
	packet.addByte(0xFF);
	player->getSession()->send(packet);
}

void LoginPacket::showChannels(PlayerLogin *player) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_CHANNEL);
	packet.addShort(0x00);
	player->getSession()->send(packet);
}

void LoginPacket::channelSelect(PlayerLogin *player) {
	PacketCreator packet;
	packet.addShort(SEND_CHANNEL_SELECT);
	packet.addShort(0);
	packet.addByte(0);
	player->getSession()->send(packet);
}

void LoginPacket::showCharacters(PlayerLogin *player, vector<Character> chars) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_CHARACTERS);
	packet.addByte(0);
	packet.addByte(chars.size());
	for (size_t i = 0; i < chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i]);
	}
	packet.addInt(3); // Max char you have have?
	player->getSession()->send(packet);
}

void LoginPacket::checkName(PlayerLogin *player, const string &name, bool taken) {
	PacketCreator packet;
	packet.addShort(SEND_CHECK_NAME);
	packet.addString(name);
	packet.addByte(taken);
	player->getSession()->send(packet);
}

void LoginPacket::showCharacter(PlayerLogin *player, Character charc) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_CHARACTER);
	packet.addByte(0);
	LoginPacketHelper::addCharacter(packet, charc);
	player->getSession()->send(packet);	
}

void LoginPacket::deleteCharacter(PlayerLogin *player, int32_t ID) {
	PacketCreator packet;
	packet.addShort(SEND_DELETE_CHAR);
	packet.addInt(ID);
	packet.addByte(0);
	player->getSession()->send(packet);
}

void LoginPacket::connectIP(PlayerLogin *player, int32_t charid) {
	PacketCreator packet;
	packet.addShort(SEND_CHANNEL_SERVER_INFO);
	packet.addShort(0);

	World *world = Worlds::worlds[player->getWorld()];
	if (world->channels.find(player->getChannel()) != world->channels.end()) {
		shared_ptr<Channel> channel = world->channels[player->getChannel()];
		packet.addIP(channel->ip);
		packet.addShort(channel->port);
	}
	else { // Channel does not exist, let's be mean and send something non-existent
		packet.addIP("255.255.255.255");
		packet.addShort(-1);
	}
	packet.addInt(charid);
	packet.addInt(0);
	packet.addByte(0);
	player->getSession()->send(packet);
}

void LoginPacket::relogResponse(PlayerLogin *player) {
	PacketCreator packet;
	packet.addShort(SEND_RELOG_RESPONSE);
	packet.addByte(1);
	player->getSession()->send(packet);
}
