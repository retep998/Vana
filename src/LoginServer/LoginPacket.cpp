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
#include "LoginPacket.h"
#include "Characters.h"
#include "LoginPacketHelper.h"
#include "IpUtilities.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PlayerLogin.h"
#include "PlayerStatus.h"
#include "SendHeader.h"
#include "Worlds.h"

void LoginPacket::loginError(PlayerLogin *player, int16_t errorid) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_AUTHENTICATION);
	packet.add<int16_t>(errorid);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::loginBan(PlayerLogin *player, int8_t reason, int32_t expire) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_AUTHENTICATION);
	packet.add<int16_t>(2);
	packet.add<int32_t>(0);
	packet.add<int8_t>(reason);
	packet.add<int32_t>(0);
	packet.add<int32_t>(expire); // Ban over: Time, anything >= 00aacb01 (year >= 2011) will cause perma ban
	player->getSession()->send(packet);
}

void LoginPacket::loginConnect(PlayerLogin *player, const string &username) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_AUTHENTICATION);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int32_t>(player->getUserId());
	switch (player->getStatus()) {
		case PlayerStatus::SetGender: packet.add<int8_t>(0x0a); break; // Gender Select
		case PlayerStatus::SetPin: packet.add<int8_t>(0x0b); break; // Pin Select
		default: packet.add<int8_t>(player->getGender()); break;
	}
	packet.addBool(player->isAdmin()); // Admin byte. Enables commands like /c, /ch, /m, /h... but disables trading.
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.addString(username);
	packet.add<int8_t>(0);
	packet.add<int8_t>(player->getQuietBanReason());
	packet.add<int64_t>(player->getQuietBanTime());
	packet.add<int64_t>(player->getCreationTime());
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::loginProcess(PlayerLogin *player, int8_t id) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PIN);
	packet.add<int8_t>(id);
	player->getSession()->send(packet);
}

void LoginPacket::pinAssigned(PlayerLogin *player) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PIN_ASSIGNED);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::genderDone(PlayerLogin *player, int8_t gender) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_GENDER_DONE);
	packet.add<int8_t>(gender);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void LoginPacket::showWorld(PlayerLogin *player, World *world) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_WORLD_LIST);
	packet.add<int8_t>(world->id);
	packet.addString(world->name);
	packet.add<int8_t>(world->ribbon);
	packet.addString(world->eventMsg);
	packet.add<int16_t>(100); // EXP rate. x/100. Changing this will show event message.
	packet.add<int16_t>(100);
	packet.add<int8_t>(0);
	packet.add<int8_t>(world->maxChannels);
	for (size_t i = 0; i < world->maxChannels; i++) {
		std::ostringstream cnStream;
		cnStream << world->name << "-" << i+1;
		string channelname = cnStream.str();
		packet.addString(channelname);

		if (world->channels.find(i) != world->channels.end()) {
			packet.add<int32_t>(world->channels[i]->population);
		}
		else { // Channel doesn't exist
			packet.add<int32_t>(0);
		}

		packet.add<int8_t>(world->id);
		packet.add<int16_t>(i);
	}
	packet.add<int16_t>(0); // Amount of messages
	// packet.addPos(); // Pos of message
	// packet.addString("message"); // message
	// When you set a pos of (0, 0), the message will be on the Scania/first world tab.
	player->getSession()->send(packet);
}

void LoginPacket::worldEnd(PlayerLogin *player) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_WORLD_LIST);
	packet.add<uint8_t>(0xFF);
	player->getSession()->send(packet);
}

void LoginPacket::showChannels(PlayerLogin *player) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_WORLD_STATUS);
	/*	Byte/short types:
		0x00 = no message
		0x01 = "Since There Are Many Concurrent Users in This World, You May Encounter Some Difficulties During the Game Play."
		0x02 = "The Concurrent Users in This World Have Reached the Max. Please Try Again Later."
	*/
	packet.add<int16_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::channelSelect(PlayerLogin *player) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_CHANNEL_SELECT);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::showCharacters(PlayerLogin *player, const vector<Character> &chars, int32_t maxchars) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PLAYER_LIST);
	packet.add<int8_t>(0);
	packet.add<int8_t>(chars.size());
	for (size_t i = 0; i < chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i]);
	}
	packet.add<int32_t>(maxchars);
	player->getSession()->send(packet);
}

void LoginPacket::channelOffline(PlayerLogin *player) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PLAYER_LIST);
	packet.add<int8_t>(8);
	player->getSession()->send(packet);
}

void LoginPacket::checkName(PlayerLogin *player, const string &name, bool taken) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PLAYER_NAME_CHECK);
	packet.addString(name);
	packet.addBool(taken);
	player->getSession()->send(packet);
}

void LoginPacket::showAllCharactersInfo(PlayerLogin *player, uint32_t worlds, uint32_t unk) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PLAYER_GLOBAL_LIST);
	packet.add<int8_t>(1);
	packet.add<int32_t>(worlds);
	packet.add<int32_t>(unk);
	player->getSession()->send(packet);
}

void LoginPacket::showCharactersWorld(PlayerLogin *player, uint8_t worldid, const vector<Character> &chars) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PLAYER_GLOBAL_LIST);
	packet.add<int8_t>(0);
	packet.add<int8_t>(worldid);
	packet.add<int8_t>(chars.size());
	for (size_t i = 0; i < chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i]);
	}
	player->getSession()->send(packet);
}

void LoginPacket::showCharacter(PlayerLogin *player, const Character &charc) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PLAYER_CREATE);
	packet.add<int8_t>(0);
	LoginPacketHelper::addCharacter(packet, charc);
	player->getSession()->send(packet);	
}

void LoginPacket::deleteCharacter(PlayerLogin *player, int32_t id, bool success) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PLAYER_DELETE);
	packet.add<int32_t>(id);
	packet.add<int8_t>((success ? 0x00 : 0x12));
	player->getSession()->send(packet);
}

void LoginPacket::connectIp(PlayerLogin *player, int32_t charid) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_CHANNEL_CONNECT);
	packet.add<int16_t>(0);

	World *world = Worlds::worlds[player->getWorld()];
	if (world->channels.find(player->getChannel()) != world->channels.end()) {
		shared_ptr<Channel> channel = world->channels[player->getChannel()];
		uint32_t chanIp = IpUtilities::matchIpSubnet(player->getIp(), channel->external_ip, channel->ip);
		packet.add<uint32_t>(htonl(chanIp)); // MapleStory accepts IP addresses in big-endian
		packet.add<int16_t>(channel->port);
	}
	else { // Channel does not exist, let's be mean and send something non-existent
		packet.add<uint32_t>(0); // ip
		packet.add<int16_t>(-1); // port
	}
	packet.add<int32_t>(charid);
	packet.add<int32_t>(0);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::relogResponse(PlayerLogin *player) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_LOGIN_RETURN);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}
