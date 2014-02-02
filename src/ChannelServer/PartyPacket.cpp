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
#include "PartyPacket.hpp"
#include "ChannelServer.hpp"
#include "GameConstants.hpp"
#include "InterHelper.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace PartyPacket {

PACKET_IMPL(error, int8_t error) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(error);
	return builder;
}

PACKET_IMPL(createParty, Party *party) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x08)
		.add<int32_t>(party->getId())
		.add<int32_t>(Maps::NoMap)
		.add<int32_t>(Maps::NoMap)
		.add<int32_t>(0);
	return builder;
}

PACKET_IMPL(joinParty, int32_t targetMapId, Party *party, const string_t &player) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x0F)
		.add<int32_t>(party->getId())
		.add<string_t>(player)
		.addBuffer(updateParty(targetMapId, party));
	return builder;
}

PACKET_IMPL(leaveParty, int32_t targetMapId, Party *party, int32_t playerId, const string_t &name, bool kicked) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x0C)
		.add<int32_t>(party->getId())
		.add<int32_t>(playerId)
		.add<bool>(true) // Not disbanding
		.add<bool>(kicked)
		.add<string_t>(name)
		.addBuffer(updateParty(targetMapId, party));
	return builder;
}

PACKET_IMPL(invitePlayer, Party *party, const string_t &inviter) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x04)
		.add<int32_t>(party->getId())
		.add<string_t>(inviter)
		.add<int8_t>(0);
	return builder;
}

PACKET_IMPL(disbandParty, Party *party) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x0C)
		.add<int32_t>(party->getId())
		.add<int32_t>(party->getLeaderId())
		.add<bool>(false) // Disbanding
		.add<int32_t>(party->getId());
	return builder;
}

PACKET_IMPL(setLeader, Party *party, int32_t newLeader) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x1B)
		.add<int32_t>(newLeader)
		.add<bool>(false);
	return builder;
}

PACKET_IMPL(silentUpdate, int32_t targetMapId, Party *party) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x07)
		.add<int32_t>(party->getId())
		.addBuffer(updateParty(targetMapId, party));
	return builder;
}

PACKET_IMPL(updateParty, int32_t targetMapId, Party *party) {
	PacketBuilder builder;
	auto &members = party->getMembers();
	size_t offset = Parties::MaxMembers - members.size();
	size_t i = 0;
	channel_id_t channelId = ChannelServer::getInstance().getChannelId();

	// Add party member IDs to packet
	for (const auto &kvp : members) {
		builder.add<int32_t>(kvp.first);
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(0);
	}

	// Add party member names to packet
	for (const auto &kvp : members) {
		auto player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
		builder.add<string_t>(player->name, 13);
	}
	for (i = 0; i < offset; i++) {
		builder.add<string_t>("", 13);
	}

	// Add party member jobs to packet
	for (const auto &kvp : members) {
		auto player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
		builder.add<int32_t>(player->job);
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(0);
	}

	// Add party member levels to packet
	for (const auto &kvp : members) {
		auto player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
		builder.add<int32_t>(player->level);
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(0);
	}

	// Add party member channels to packet
	for (const auto &kvp : members) {
		auto player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
		builder.add<int32_t>(player->channel != -1 ?
			player->channel :
			(player->cashShop ? -1 : -2));
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(-2);
	}

	builder.add<int32_t>(party->getLeaderId());

	// Add party member maps to packet
	for (const auto &kvp : members) {
		auto player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
		if (player->channel == channelId && player->map == targetMapId) {
			builder.add<int32_t>(targetMapId);
		}
		else {
			builder.add<int32_t>(0);
		}
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(-2);
	}

	// Add some portal information (Door?)
	for (const auto &kvp : members) {
		builder
			.add<int32_t>(Maps::NoMap)
			.add<int32_t>(Maps::NoMap)
			.add<int32_t>(-1)
			.add<int32_t>(-1);
	}
	for (i = 0; i < offset; i++) {
		builder
			.add<int32_t>(Maps::NoMap)
			.add<int32_t>(Maps::NoMap)
			.add<int32_t>(-1)
			.add<int32_t>(-1);
	}
	return builder;
}

}