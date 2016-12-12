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
#include "MysticDoor.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerSkills.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "WidePoint.hpp"

namespace PartyPacket {

PACKET_IMPL(error, int8_t error) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(error);
	return builder;
}

PACKET_IMPL(inviteError, int8_t error, const string_t &player) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(error)
		.add<string_t>(player);
	return builder;
}

PACKET_IMPL(customError, const string_t &error) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x24);

	if (error.empty()) {
		builder
			.add<bool>(true)
			.add<string_t>(error);
	}
	else {
		builder.add<bool>(false);
	}
		
	return builder;
}

PACKET_IMPL(createParty, Party *party, Player *leader) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x08)
		.add<party_id_t>(party->getId());

	if (ref_ptr_t<MysticDoor> door = leader->getSkills()->getMysticDoor()) {
		if (door->getMapId() == leader->getMapId()) {
			builder
				.add<map_id_t>(door->getMapId())
				.add<map_id_t>(door->getTownId())
				.add<Point>(door->getMapPos());
		}
		else {
			builder
				.add<map_id_t>(door->getTownId())
				.add<map_id_t>(door->getMapId())
				.add<Point>(door->getTownPos());
		}
	}
	else {
		builder
			.add<map_id_t>(Maps::NoMap)
			.add<map_id_t>(Maps::NoMap)
			.add<Point>(Point{});
	}
	return builder;
}

PACKET_IMPL(joinParty, map_id_t targetMapId, Party *party, const string_t &player) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x0F)
		.add<party_id_t>(party->getId())
		.add<string_t>(player)
		.addBuffer(updateParty(targetMapId, party));
	return builder;
}

PACKET_IMPL(leaveParty, map_id_t targetMapId, Party *party, player_id_t playerId, const string_t &name, bool kicked) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x0C)
		.add<party_id_t>(party->getId())
		.add<player_id_t>(playerId)
		.add<bool>(true) // Is a regular leave (aka not disbanding)
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
		.add<party_id_t>(party->getId())
		.add<string_t>(inviter)
		.add<int8_t>(0);
	return builder;
}

PACKET_IMPL(disbandParty, Party *party) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x0C)
		.add<party_id_t>(party->getId())
		.add<player_id_t>(party->getLeaderId())
		.add<bool>(false) // Is not a regular leave (aka disbanding)
		.add<party_id_t>(party->getId());
	return builder;
}

PACKET_IMPL(setLeader, Party *party, player_id_t newLeader) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x1B)
		.add<player_id_t>(newLeader)
		.add<bool>(false);
	return builder;
}

PACKET_IMPL(silentUpdate, map_id_t targetMapId, Party *party) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x07)
		.add<party_id_t>(party->getId())
		.addBuffer(updateParty(targetMapId, party));
	return builder;
}

PACKET_IMPL(updateParty, map_id_t targetMapId, Party *party) {
	PacketBuilder builder;
	auto &members = party->getMembers();
	size_t offset = Parties::MaxMembers - members.size();
	size_t i = 0;
	channel_id_t channelId = ChannelServer::getInstance().getChannelId();
	auto &provider = ChannelServer::getInstance().getPlayerDataProvider();

	// Add party member IDs to packet
	for (const auto &kvp : members) {
		builder.add<player_id_t>(kvp.first);
	}
	for (i = 0; i < offset; i++) {
		builder.add<player_id_t>(0);
	}

	// Add party member names to packet
	for (const auto &kvp : members) {
		auto player = provider.getPlayerData(kvp.first);
		builder.add<string_t>(player->name, 13);
	}
	for (i = 0; i < offset; i++) {
		builder.add<string_t>("", 13);
	}

	// Add party member jobs to packet
	for (const auto &kvp : members) {
		auto player = provider.getPlayerData(kvp.first);
		builder.add<int32_t>(player->job.get(-1));
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(0);
	}

	// Add party member levels to packet
	for (const auto &kvp : members) {
		auto player = provider.getPlayerData(kvp.first);
		builder.add<int32_t>(player->level.is_initialized() ?
			player->level.get() :
			-1);
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(0);
	}

	// Add party member channels to packet
	for (const auto &kvp : members) {
		auto player = provider.getPlayerData(kvp.first);
		builder.add<int32_t>(player->cashShop ?
			-1 :
			player->channel.get(-2));
	}
	for (i = 0; i < offset; i++) {
		builder.add<int32_t>(-2);
	}

	builder.add<player_id_t>(party->getLeaderId());

	// Add party member maps to packet
	for (const auto &kvp : members) {
		auto player = provider.getPlayerData(kvp.first);
		if (!player->cashShop && !player->mts && player->channel == channelId && player->map == targetMapId) {
			builder.add<map_id_t>(targetMapId);
		}
		else {
			builder.add<map_id_t>(0);
		}
	}
	for (i = 0; i < offset; i++) {
		builder.add<map_id_t>(-2);
	}

	// Mystic Door information
	for (const auto &kvp : members) {
		if (kvp.second == nullptr) {
			builder
				.add<map_id_t>(Maps::NoMap)
				.add<map_id_t>(Maps::NoMap)
				.add<WidePoint>(WidePoint{-1, -1});
		}
		else if (ref_ptr_t<MysticDoor> door = kvp.second->getSkills()->getMysticDoor()) {
			builder
				.add<map_id_t>(door->getTownId())
				.add<map_id_t>(door->getMapId())
				.add<WidePoint>(WidePoint{door->getMapPos()});
		}
		else {
			builder
				.add<map_id_t>(Maps::NoMap)
				.add<map_id_t>(Maps::NoMap)
				.add<WidePoint>(WidePoint{-1, -1});
		}
	}
	for (i = 0; i < offset; i++) {
		builder
			.add<map_id_t>(Maps::NoMap)
			.add<map_id_t>(Maps::NoMap)
			.add<WidePoint>(WidePoint{-1, -1});
	}
	return builder;
}

PACKET_IMPL(updateDoor, uint8_t zeroBasedPlayerIndex, ref_ptr_t<MysticDoor> door) {
	PacketBuilder builder;
	builder
		.add<int16_t>(SMSG_PARTY)
		.add<int8_t>(0x25)
		.add<uint8_t>(zeroBasedPlayerIndex);

	if (door == nullptr) {
		builder
			.add<map_id_t>(Maps::NoMap)
			.add<map_id_t>(Maps::NoMap)
			.add<WidePoint>(WidePoint{-1, -1});
	}
	else {
		builder
			.add<map_id_t>(door->getTownId())
			.add<map_id_t>(door->getMapId())
			.add<WidePoint>(WidePoint{door->getMapPos()});
	}

	return builder;
}

}