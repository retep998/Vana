/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "Channel.h"
#include "Characters.h"
#include "LoginPacketHelper.h"
#include "IpUtilities.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerStatus.h"
#include "SendHeader.h"
#include "World.h"
#include "Worlds.h"

void LoginPacket::loginError(Player *player, int16_t errorid) {
	PacketCreator packet;
	packet.addHeader(SMSG_AUTHENTICATION);
	packet.add<int16_t>(errorid);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::loginBan(Player *player, int8_t reason, int32_t expire) {
	PacketCreator packet;
	packet.addHeader(SMSG_AUTHENTICATION);
	packet.add<int16_t>(2);
	packet.add<int32_t>(0);
	packet.add<int8_t>(reason);
	packet.add<int32_t>(0);
	packet.add<int32_t>(expire); // Ban over: Time, anything >= 00aacb01 (year >= 2011) will cause perma ban
	player->getSession()->send(packet);
}

void LoginPacket::loginConnect(Player *player, const string &username) {
	PacketCreator packet;
	packet.addHeader(SMSG_AUTHENTICATION);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int32_t>(player->getUserId());
	switch (player->getStatus()) {
		case PlayerStatus::SetGender: packet.add<int8_t>(PlayerStatus::SetGender); break;
		case PlayerStatus::SetPin: packet.add<int8_t>(PlayerStatus::PinSelect); break;
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

void LoginPacket::loginProcess(Player *player, int8_t id) {
	PacketCreator packet;
	packet.addHeader(SMSG_PIN);
	packet.add<int8_t>(id);
	player->getSession()->send(packet);
}

void LoginPacket::pinAssigned(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_PIN_ASSIGNED);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::genderDone(Player *player, int8_t gender) {
	PacketCreator packet;
	packet.addHeader(SMSG_ACCOUNT_GENDER_DONE);
	packet.add<int8_t>(gender);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void LoginPacket::showWorld(Player *player, World *world) {
	PacketCreator packet;
	packet.addHeader(SMSG_WORLD_LIST);
	packet.add<int8_t>(world->getId());
	packet.addString(world->getName());
	packet.add<int8_t>(world->getRibbon());
	packet.addString(world->getEventMessage());
	packet.add<int16_t>(100); // EXP rate. x/100. Changing this will show event message.
	packet.add<int16_t>(100);
	packet.add<int8_t>(0);
	packet.add<uint8_t>(world->getMaxChannels());
	for (size_t i = 0; i < world->getMaxChannels(); i++) {
		std::ostringstream cnStream;
		cnStream << world->getName() << "-" << i + 1;
		string channelname = cnStream.str();
		packet.addString(channelname);

		if (Channel *channel = world->getChannel(i)) {
			packet.add<int32_t>(channel->getPopulation());
		}
		else { // Channel doesn't exist
			packet.add<int32_t>(0);
		}

		packet.add<int8_t>(world->getId());
		packet.add<uint8_t>(i);
		packet.add<uint8_t>(0); // Some sort of state O.o
	}
	packet.add<int16_t>(0); // Amount of messages
	// packet.addPos(); // Pos of message
	// packet.addString("message"); // message
	// When you set a pos of (0, 0), the message will be on the Scania/first world tab.
	player->getSession()->send(packet);
}

void LoginPacket::worldEnd(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_WORLD_LIST);
	packet.add<int8_t>(-1);
	player->getSession()->send(packet);
}

void LoginPacket::showChannels(Player *player, int8_t status) {
	PacketCreator packet;
	packet.addHeader(SMSG_WORLD_STATUS);
	packet.add<int16_t>(status);
	player->getSession()->send(packet);
}

void LoginPacket::channelSelect(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_CHANNEL_SELECT);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::showCharacters(Player *player, const vector<Character> &chars, int32_t maxchars) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_LIST);
	packet.add<int8_t>(0);
	packet.add<uint8_t>(chars.size());
	for (size_t i = 0; i < chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i]);
	}
	packet.add<int32_t>(maxchars);
	player->getSession()->send(packet);
}

void LoginPacket::channelOffline(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_LIST);
	packet.add<int8_t>(8);
	player->getSession()->send(packet);
}

void LoginPacket::checkName(Player *player, const string &name, bool taken) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_NAME_CHECK);
	packet.addString(name);
	packet.addBool(taken);
	player->getSession()->send(packet);
}

void LoginPacket::showAllCharactersInfo(Player *player, uint32_t worlds, uint32_t unk) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_GLOBAL_LIST);
	packet.add<int8_t>(1);
	packet.add<uint32_t>(worlds);
	packet.add<uint32_t>(unk);
	player->getSession()->send(packet);
}

void LoginPacket::showCharactersWorld(Player *player, uint8_t worldid, const vector<Character> &chars) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_GLOBAL_LIST);
	packet.add<int8_t>(0);
	packet.add<uint8_t>(worldid);
	packet.add<uint8_t>(chars.size());
	for (size_t i = 0; i < chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i]);
	}
	player->getSession()->send(packet);
}

void LoginPacket::showCharacter(Player *player, const Character &charc) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_CREATE);
	packet.add<int8_t>(0);
	LoginPacketHelper::addCharacter(packet, charc);
	player->getSession()->send(packet);
}

void LoginPacket::deleteCharacter(Player *player, int32_t id, uint8_t result) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_DELETE);
	packet.add<int32_t>(id);
	packet.add<uint8_t>(result);
	player->getSession()->send(packet);
}

void LoginPacket::connectIp(Player *player, int32_t charid) {
	PacketCreator packet;
	packet.addHeader(SMSG_CHANNEL_CONNECT);
	packet.add<int16_t>(0);

	if (Channel *channel = Worlds::Instance()->getWorld(player->getWorld())->getChannel(player->getChannel())) {
		uint32_t chanIp = IpUtilities::matchIpSubnet(player->getIp(), channel->getExternalIps(), channel->getIp());
		packet.add<uint32_t>(htonl(chanIp)); // MapleStory accepts IP addresses in big-endian
		packet.add<uint16_t>(channel->getPort());
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

void LoginPacket::relogResponse(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_LOGIN_RETURN);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}
