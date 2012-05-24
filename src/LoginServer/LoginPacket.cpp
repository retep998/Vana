/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "IpUtilities.h"
#include "LoginPacketHelper.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerStatus.h"
#include "Session.h"
#include "SmsgHeader.h"
#include "World.h"
#include "Worlds.h"

void LoginPacket::loginError(Player *player, int16_t errorId) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_AUTHENTICATION);
	packet.add<int16_t>(errorId);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::loginBan(Player *player, int8_t reason, int32_t expire) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_AUTHENTICATION);
	packet.add<int16_t>(2);
	packet.add<int32_t>(0);
	packet.add<int8_t>(reason);
	packet.add<int32_t>(0);
	packet.add<int32_t>(expire); // Ban over: Time, anything >= 00aacb01 (year >= 2011) will cause perma ban
	player->getSession()->send(packet);
}

void LoginPacket::loginConnect(Player *player, const string &username) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_AUTHENTICATION);
	packet.add<int16_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(player->getUserId());
	switch (player->getStatus()) {
		case PlayerStatus::SetGender: packet.add<int8_t>(PlayerStatus::SetGender); break; // Gender Select
		case PlayerStatus::SetPin: packet.add<int8_t>(PlayerStatus::PinSelect); break; // Pin Select
		default: packet.add<int8_t>(player->getGender()); break;
	}
	packet.add<uint8_t>(0xFF); // Admin byte. Enables commands like /c, /ch, /m, /h... but disables trading.
	packet.add<int16_t>(0);
	packet.add<int8_t>(0x95); // V.109 update
	packet.addString(username);
	packet.add<int8_t>(3); // 0x03?
	packet.add<int8_t>(player->getQuietBanReason());
	packet.add<int64_t>(player->getQuietBanTime());
	packet.add<int8_t>(1); // V.109 update
	packet.add<int64_t>(player->getCreationTime());
	packet.add<int32_t>(75); // 75?
	packet.add<int8_t>(1); // V.83+, PIC is set
	packet.add<int8_t>(1); // V.83+, PIC modus
	packet.add<int64_t>(player->getConnectKey());
	player->getSession()->send(packet);

	
	packet = PacketCreator();
	packet.add<header_t>(0x00BD);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::accountInfo(Player *player) { // V.106+
	PacketCreator packet;
	packet.add<header_t>(SMSG_AUTHENTICATION_2);
	packet.add<int8_t>(0);
	packet.add<int32_t>(player->getUserId());
	switch (player->getStatus()) {
		case PlayerStatus::SetGender: packet.add<int8_t>(PlayerStatus::SetGender); break; // Gender Select
		case PlayerStatus::SetPin: packet.add<int8_t>(PlayerStatus::PinSelect); break; // Pin Select
		default: packet.add<int8_t>(player->getGender()); break;
	}
	packet.add<uint8_t>(0xFF); // Admin byte. Enables commands like /c, /ch, /m, /h... but disables trading.
	packet.add<int16_t>(0);
	packet.add<int8_t>(0x95);
	packet.addString(player->getUsername());
	packet.add<int8_t>(3); // 0x03?
	packet.add<int8_t>(player->getQuietBanReason());
	packet.add<int64_t>(player->getQuietBanTime());
	packet.add<int64_t>(player->getCreationTime());
	packet.add<int32_t>(75); // 75?
	packet.add<int64_t>(player->getConnectKey());
	packet.add<int8_t>(1); // ???
	player->getSession()->send(packet);
}

void LoginPacket::loginProcess(Player *player, int8_t id) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PIN);
	packet.add<int8_t>(id);
	player->getSession()->send(packet);
}

void LoginPacket::pinAssigned(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PIN_ASSIGNED);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::genderDone(Player *player, int8_t gender) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_ACCOUNT_GENDER_DONE);
	packet.add<int8_t>(gender);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void LoginPacket::showWorld(Player *player, World *world) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_WORLD_LIST);
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
		const string &channelName = cnStream.str();
		packet.addString(channelName);

		if (Channel *channel = world->getChannel(i)) {
			packet.add<int32_t>(channel->getPopulation());
		}
		else {
			// Channel doesn't exist
			packet.add<int32_t>(0);
		}

		packet.add<int8_t>(world->getId());
		packet.add<uint8_t>(i);
		packet.add<uint8_t>(0); // Some sort of state
	}
	packet.add<int16_t>(0); // Amount of messages
	// packet.addPos(); // Pos of message
	// packet.addString("message"); // message
	// When you set a pos of (0, 0), the message will be on the Scania/first world tab.

	packet.add<int32_t>(0); // V.93+
	player->getSession()->send(packet);
}

void LoginPacket::worldEnd(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_WORLD_LIST);
	packet.add<int8_t>(-1);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<header_t>(0x001B); // Last World
	packet.add<int32_t>(0);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<header_t>(0x001C); // Recommended Worlds
	packet.add<int8_t>(1);
	packet.add<int32_t>(0);
	packet.addString("Hallo");
	player->getSession()->send(packet);
}

void LoginPacket::showChannels(Player *player, int8_t status) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_WORLD_STATUS);
	packet.add<int16_t>(status);
	player->getSession()->send(packet);
}

void LoginPacket::channelSelect(Player *player) {
	accountInfo(player);
}

void LoginPacket::showCharacters(Player *player, const vector<Character> &chars, int32_t maxChars) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_LIST);
	packet.add<int8_t>(0);
	packet.add<uint8_t>(chars.size());
	for (size_t i = 0; i < chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i], LoginPacketHelper::Normal);
	}
	packet.add<int8_t>(1); // PIC
	packet.add<int8_t>(0); // ???
	packet.add<int32_t>(maxChars);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::channelOffline(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_LIST);
	packet.add<int8_t>(9);
	player->getSession()->send(packet);
}

void LoginPacket::checkName(Player *player, const string &name, bool taken) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_NAME_CHECK);
	packet.addString(name);
	packet.addBool(taken);
	player->getSession()->send(packet);
}

void LoginPacket::showAllCharactersInfo(Player *player, uint32_t worlds, uint32_t unk) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_GLOBAL_LIST);
	packet.add<int8_t>(1);
	packet.add<uint32_t>(worlds);
	packet.add<uint32_t>(unk);
	player->getSession()->send(packet);
}

void LoginPacket::showCharactersWorld(Player *player, uint8_t worldId, const vector<Character> &chars) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_GLOBAL_LIST);
	packet.add<int8_t>(0);
	packet.add<uint8_t>(worldId);
	packet.add<uint8_t>(chars.size());
	for (size_t i = 0; i < chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i], LoginPacketHelper::ViewAllCharacters);
	}
	packet.add<int8_t>(1); // PIC
	player->getSession()->send(packet);
}

void LoginPacket::showCharacter(Player *player, const Character &charc) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_CREATE);
	packet.add<int8_t>(0);
	LoginPacketHelper::addCharacter(packet, charc, LoginPacketHelper::AddedNewCharacter);
	player->getSession()->send(packet);
}

void LoginPacket::deleteCharacter(Player *player, int32_t id, uint8_t result) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_DELETE);
	packet.add<int32_t>(id);
	packet.add<uint8_t>(result);
	player->getSession()->send(packet);
}

void LoginPacket::connectIp(Player *player, int32_t charId) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHANNEL_CONNECT);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);

	if (Channel *channel = Worlds::Instance()->getWorld(player->getWorld())->getChannel(player->getChannel())) {
		ip_t chanIp = IpUtilities::matchIpSubnet(player->getIp(), channel->getExternalIps(), channel->getIp());
		packet.add<ip_t>(htonl(chanIp)); // MapleStory accepts IP addresses in big-endian
		packet.add<port_t>(channel->getPort());
	}
	else {
		// Channel does not exist, let's be mean and send something non-existent
		packet.add<ip_t>(0); // ip
		packet.add<port_t>(-1); // port
	}
	packet.add<int32_t>(charId);
	packet.add<int8_t>(0); // ??
	packet.add<int32_t>(0);
	packet.add<int8_t>(0);
	packet.add<int16_t>(0);
	packet.add<int16_t>(0);
	player->getSession()->send(packet);
}

void LoginPacket::relogResponse(Player *player) { // Is this even used anymore?
	PacketCreator packet;
	packet.add<header_t>(SMSG_LOGIN_RETURN);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void LoginPacket::specialCharacterCreation(Player *player, bool enabled) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_CREATE_SPECIAL_ENABLE);
	packet.add<int32_t>(player->getUserId());
	packet.addBool(enabled);
	player->getSession()->send(packet);
}