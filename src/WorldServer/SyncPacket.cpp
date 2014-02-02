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
#include "SyncPacket.hpp"
#include "AbstractConnection.hpp"
#include "Channel.hpp"
#include "Channels.hpp"
#include "Configuration.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "MapConstants.hpp"
#include "PacketReader.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "TimeUtilities.hpp"
#include "WorldServerAcceptConnection.hpp"

namespace SyncPacket {

PACKET_IMPL(sendSyncData, function_t<void(PacketBuilder &)> buildSyncData) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::ChannelStart);
	buildSyncData(builder);
	return builder;
}

PACKET_IMPL(ConfigPacket::scrollingHeader, const string_t &message) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::ScrollingHeader)
		.add<string_t>(message);
	return builder;
}

PACKET_IMPL(ConfigPacket::setRates, const Rates &rates) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::RateSet)
		.add<Rates>(rates);
	return builder;
}

PACKET_IMPL(PlayerPacket::newConnectable, int32_t playerId, const Ip &ip, PacketReader &buffer) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::NewConnectable)
		.add<int32_t>(playerId)
		.add<Ip>(ip)
		.add<uint16_t>(buffer.getBufferLength())
		.addBuffer(buffer);
	return builder;
}

PACKET_IMPL(PlayerPacket::deleteConnectable, int32_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::DeleteConnectable)
		.add<int32_t>(playerId);
	return builder;
}

PACKET_IMPL(PlayerPacket::playerChangeChannel, int32_t playerId, channel_id_t channelId, const Ip &ip, port_t port) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::ChangeChannelGo)
		.add<int32_t>(playerId)
		.add<channel_id_t>(channelId)
		.add<Ip>(ip)
		.add<port_t>(port);
	return builder;
}

PACKET_IMPL(PlayerPacket::updatePlayer, const PlayerData &data, update_bits_t flags) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::UpdatePlayer)
		.add<int32_t>(data.id)
		.add<update_bits_t>(flags);

	if (flags & Sync::Player::UpdateBits::Full) {
		builder.add<PlayerData>(data);
	}
	else {
		if (flags & Sync::Player::UpdateBits::Level) {
			builder.add<int16_t>(data.level);
		}
		if (flags & Sync::Player::UpdateBits::Job) {
			builder.add<int16_t>(data.job);
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			builder.add<int32_t>(data.map);
		}
		if (flags & Sync::Player::UpdateBits::Channel) {
			builder.add<channel_id_t>(data.channel);
		}
		if (flags & Sync::Player::UpdateBits::Ip) {
			builder.add<Ip>(data.ip);
		}
		if (flags & Sync::Player::UpdateBits::Cash) {
			builder.add<bool>(data.cashShop);
		}
	}
	return builder;
}

PACKET_IMPL(PlayerPacket::characterCreated, const PlayerData &data) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::CharacterCreated)
		.add<PlayerData>(data);
	return builder;
}

PACKET_IMPL(PlayerPacket::characterDeleted, int32_t id) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::CharacterDeleted)
		.add<int32_t>(id);
	return builder;
}

PACKET_IMPL(PartyPacket::removePartyMember, int32_t partyId, int32_t playerId, bool kicked) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::RemoveMember)
		.add<int32_t>(partyId)
		.add<int32_t>(playerId)
		.add<bool>(kicked);
	return builder;
}

PACKET_IMPL(PartyPacket::addPartyMember, int32_t partyId, int32_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::AddMember)
		.add<int32_t>(partyId)
		.add<int32_t>(playerId);
	return builder;
}

PACKET_IMPL(PartyPacket::newPartyLeader, int32_t partyId, int32_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::SwitchLeader)
		.add<int32_t>(partyId)
		.add<int32_t>(playerId);
	return builder;
}

PACKET_IMPL(PartyPacket::createParty, int32_t partyId, int32_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::Create)
		.add<int32_t>(partyId)
		.add<int32_t>(playerId);
	return builder;
}

PACKET_IMPL(PartyPacket::disbandParty, int32_t partyId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::Disband)
		.add<int32_t>(partyId);
	return builder;
}

PACKET_IMPL(BuddyPacket::sendBuddyInvite, int32_t inviteeId, int32_t inviterId, const string_t &name) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::Invite)
		.add<int32_t>(inviterId)
		.add<int32_t>(inviteeId)
		.add<string_t>(name);
	return builder;
}

PACKET_IMPL(BuddyPacket::sendBuddyOnlineOffline, const vector_t<int32_t> &players, int32_t playerId, channel_id_t channelId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::OnlineOffline)
		.add<int32_t>(playerId)
		.add<channel_id_t>(channelId)
		.add<vector_t<int32_t>>(players);
	return builder;
}

}