/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "PlayerStatus.hpp"
#include "SmsgHeader.hpp"
#include "UserConnection.hpp"
#include "World.hpp"

namespace LoginPacket {

PACKET_IMPL(loginError, int16_t errorId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_AUTHENTICATION)
		.add<int16_t>(errorId)
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(loginBan, int8_t reason, int64_t expire) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_AUTHENTICATION)
		.add<int16_t>(2)
		.unk<int32_t>()
		.add<int8_t>(reason)
		.add<int64_t>(expire);
	// Expiration will be a permanent ban if it's more than 2 years over the current year
	// However, the cutoff is Jan1
	// So if it's June, 2008 on the client's system, sending Jan 1 2011 = permanent ban
	return builder;
}

PACKET_IMPL(loginConnect, UserConnection *user, const string_t &username) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_AUTHENTICATION)
		.add<int16_t>(0)
		.unk<int32_t>()
		.add<account_id_t>(user->getUserId());

	switch (user->getStatus()) {
		case PlayerStatus::SetGender:
			builder.add<int8_t>(PlayerStatus::SetGender);
			break;
		case PlayerStatus::SetPin:
			builder.add<int8_t>(PlayerStatus::PinSelect);
			break;
		default:
			builder.add<gender_id_t>(user->getGender());
			break;
	}

	builder.add<bool>(user->isAdmin()); // Enables commands like /c, /ch, /m, /h... but disables trading

	// Seems like 0x80 is a "MWLB" account - I doubt it... it disables attacking and allows GM fly
	// 0x40, 0x20 (and probably 0x10, 0x8, 0x4, 0x2, and 0x1) don't appear to confer any particular benefits, restrictions, or functionality
	// (Although I didn't test client GM commands or anything of the sort)

	builder
		.add<uint8_t>(user->isAdmin() ? 0x80 : 0x00)
		.add<bool>(user->getGmLevel() > 0);

	builder
		.add<string_t>(username)
		.unk<int8_t>()
		.add<int8_t>(user->getQuietBanReason())
		.add<int64_t>(user->getQuietBanTime())
		.add<int64_t>(user->getCreationTime())
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(loginProcess, int8_t id) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PIN)
		.add<int8_t>(id);
	return builder;
}

PACKET_IMPL(pinAssigned) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PIN_ASSIGNED)
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(genderDone, gender_id_t gender) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_ACCOUNT_GENDER_DONE)
		.add<gender_id_t>(gender)
		.unk<int8_t>(1);
	return builder;
}

PACKET_IMPL(showWorld, World *world) {
	PacketBuilder builder;

	// Modifying this will show the event message
	int16_t expRatePercentage = 100;

	builder
		.add<header_t>(SMSG_WORLD_LIST)
		.add<world_id_t>(world->getId())
		.add<string_t>(world->getName())
		.add<int8_t>(world->getRibbon())
		.add<string_t>(world->getEventMessage())
		.add<int16_t>(expRatePercentage) 
		.unk<int16_t>(100)
		.unk<int8_t>();

	builder.add<channel_id_t>(world->getMaxChannels());
	for (channel_id_t i = 0; i < world->getMaxChannels(); i++) {
		out_stream_t cnStream;
		cnStream << world->getName() << "-" << static_cast<int32_t>(i + 1);
		builder.add<string_t>(cnStream.str());

		if (Channel *channel = world->getChannel(i)) {
			builder.add<int32_t>(channel->getPopulation());
		}
		else {
			// Channel doesn't exist
			builder.add<int32_t>(0);
		}

		builder
			.add<world_id_t>(world->getId())
			.add<uint8_t>(i)
			.unk<uint8_t>(); // Some sort of state
	}

	int16_t messageCount = 0;
	builder.add<int16_t>(messageCount);
	for (int16_t i = 0; i < messageCount; ++i) {
		// When you set a pos of (0, 0), the message will be on the first world tab
		builder.add<Point>(Point{i * 10, 0});
		builder.add<string_t>("message");
	}

	return builder;
}

PACKET_IMPL(worldEnd) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_WORLD_LIST)
		.add<world_id_t>(-1);
	return builder;
}

PACKET_IMPL(showChannels, int8_t status) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_WORLD_STATUS)
		.add<int16_t>(status);
	return builder;
}

PACKET_IMPL(channelSelect) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CHANNEL_SELECT)
		.unk<int16_t>()
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(showCharacters, const vector_t<Character> &chars, int32_t maxChars) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_LIST)
		.unk<int8_t>();

	builder.add<uint8_t>(static_cast<uint8_t>(chars.size()));
	for (const auto &elem : chars) {
		builder.addBuffer(LoginPacketHelper::addCharacter(elem));
	}

	builder.add<int32_t>(maxChars);
	return builder;
}

PACKET_IMPL(channelOffline) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_LIST)
		.add<int8_t>(8);
	return builder;
}

PACKET_IMPL(checkName, const string_t &name, uint8_t message) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_NAME_CHECK)
		.add<string_t>(name)
		.add<uint8_t>(message);
	return builder;
}

PACKET_IMPL(showAllCharactersInfo, world_id_t worldCount, uint32_t unk) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_GLOBAL_LIST)
		.add<bool>(true) // Indicates the world-specific character list, true means it's the world-specific character list
		.add<int32_t>(worldCount)
		.unk<uint32_t>(unk);
	return builder;
}

PACKET_IMPL(showViewAllCharacters, world_id_t worldId, const vector_t<Character> &chars) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_GLOBAL_LIST)
		.add<bool>(false) // Indicates the world-specific character list, false means it's view all characters
		.add<world_id_t>(worldId);

	builder.add<uint8_t>(static_cast<uint8_t>(chars.size()));
	for (const auto &elem : chars) {
		builder.addBuffer(LoginPacketHelper::addCharacter(elem));
	}
	return builder;
}

PACKET_IMPL(showCharacter, const Character &charc) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_CREATE)
		.unk<int8_t>()
		.addBuffer(LoginPacketHelper::addCharacter(charc));
	return builder;
}

PACKET_IMPL(deleteCharacter, player_id_t id, uint8_t result) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_DELETE)
		.add<player_id_t>(id)
		.add<uint8_t>(result);
	return builder;
}

PACKET_IMPL(connectIp, const ClientIp &ip, port_t port, player_id_t charId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CHANNEL_CONNECT)
		.unk<int16_t>()
		.add<ClientIp>(ip)
		.add<port_t>(port)
		.add<player_id_t>(charId)
		.unk<int32_t>()
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(relogResponse) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_LOGIN_RETURN)
		.unk<int8_t>(1);
	return builder;
}

}