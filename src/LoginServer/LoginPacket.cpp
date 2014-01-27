/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "LoginPacket.hpp"
#include "Channel.hpp"
#include "Characters.hpp"
#include "ClientIp.hpp"
#include "LoginPacketHelper.hpp"
#include "PacketCreator.hpp"
#include "Player.hpp"
#include "PlayerStatus.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "World.hpp"
#include "Worlds.hpp"

auto LoginPacket::loginError(Player *player, int16_t errorId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_AUTHENTICATION);
	packet.add<int16_t>(errorId);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

auto LoginPacket::loginBan(Player *player, int8_t reason, int32_t expire) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_AUTHENTICATION);
	packet.add<int16_t>(2);
	packet.add<int32_t>(0);
	packet.add<int8_t>(reason);
	packet.add<int32_t>(0);
	packet.add<int32_t>(expire); // Ban over: Time, anything >= 00aacb01 (year >= 2011) will cause perma ban
	player->getSession()->send(packet);
}

auto LoginPacket::loginConnect(Player *player, const string_t &username) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_AUTHENTICATION);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int32_t>(player->getUserId());
	switch (player->getStatus()) {
		case PlayerStatus::SetGender: packet.add<int8_t>(PlayerStatus::SetGender); break; // Gender Select
		case PlayerStatus::SetPin: packet.add<int8_t>(PlayerStatus::PinSelect); break; // Pin Select
		default: packet.add<int8_t>(player->getGender()); break;
	}
	packet.add<bool>(player->isAdmin()); // Admin byte. Enables commands like /c, /ch, /m, /h... but disables trading.
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

auto LoginPacket::loginProcess(Player *player, int8_t id) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PIN);
	packet.add<int8_t>(id);
	player->getSession()->send(packet);
}

auto LoginPacket::pinAssigned(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PIN_ASSIGNED);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

auto LoginPacket::genderDone(Player *player, int8_t gender) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_ACCOUNT_GENDER_DONE);
	packet.add<int8_t>(gender);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

auto LoginPacket::showWorld(Player *player, World *world) -> void {
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
		out_stream_t cnStream;
		cnStream << world->getName() << "-" << i + 1;
		const string_t &channelName = cnStream.str();
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
	player->getSession()->send(packet);
}

auto LoginPacket::worldEnd(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_WORLD_LIST);
	packet.add<int8_t>(-1);
	player->getSession()->send(packet);
}

auto LoginPacket::showChannels(Player *player, int8_t status) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_WORLD_STATUS);
	packet.add<int16_t>(status);
	player->getSession()->send(packet);
}

auto LoginPacket::channelSelect(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHANNEL_SELECT);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

auto LoginPacket::showCharacters(Player *player, const vector_t<Character> &chars, int32_t maxChars) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_LIST);
	packet.add<int8_t>(0);
	packet.add<uint8_t>(chars.size());
	for (size_t i = 0; i < chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i]);
	}
	packet.add<int32_t>(maxChars);
	player->getSession()->send(packet);
}

auto LoginPacket::channelOffline(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_LIST);
	packet.add<int8_t>(8);
	player->getSession()->send(packet);
}

auto LoginPacket::checkName(Player *player, const string_t &name, uint8_t message) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_NAME_CHECK);
	packet.addString(name);
	packet.add<uint8_t>(message);
	player->getSession()->send(packet);
}

auto LoginPacket::showAllCharactersInfo(Player *player, uint32_t worlds, uint32_t unk) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_GLOBAL_LIST);
	packet.add<int8_t>(1);
	packet.add<uint32_t>(worlds);
	packet.add<uint32_t>(unk);
	player->getSession()->send(packet);
}

auto LoginPacket::showViewAllCharacters(Player *player, uint8_t worldId, const vector_t<Character> &chars) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_GLOBAL_LIST);
	packet.add<int8_t>(0);
	packet.add<uint8_t>(worldId);
	packet.add<uint8_t>(chars.size());
	for (size_t i = 0; i < chars.size(); i++) {
		LoginPacketHelper::addCharacter(packet, chars[i]);
	}
	player->getSession()->send(packet);
}

auto LoginPacket::showCharacter(Player *player, const Character &charc) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_CREATE);
	packet.add<int8_t>(0);
	LoginPacketHelper::addCharacter(packet, charc);
	player->getSession()->send(packet);
}

auto LoginPacket::deleteCharacter(Player *player, int32_t id, uint8_t result) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_DELETE);
	packet.add<int32_t>(id);
	packet.add<uint8_t>(result);
	player->getSession()->send(packet);
}

auto LoginPacket::connectIp(Player *player, int32_t charId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHANNEL_CONNECT);
	packet.add<int16_t>(0);

	Ip chanIp(0);
	port_t port = -1;

	if (Channel *channel = Worlds::getInstance().getWorld(player->getWorldId())->getChannel(player->getChannel())) {
		chanIp = channel->matchIpToSubnet(player->getIp());
		port = channel->getPort();
	}

	packet.addClass<ClientIp>(ClientIp(chanIp));
	packet.add<port_t>(port);
	packet.add<int32_t>(charId);
	packet.add<int32_t>(0);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

auto LoginPacket::relogResponse(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_LOGIN_RETURN);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}