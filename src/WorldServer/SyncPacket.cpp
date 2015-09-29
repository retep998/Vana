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
#include "SyncPacket.hpp"
#include "Common/AbstractConnection.hpp"
#include "Common/InterHeader.hpp"
#include "Common/InterHelper.hpp"
#include "Common/MapConstants.hpp"
#include "Common/PacketReader.hpp"
#include "Common/RatesConfig.hpp"
#include "Common/Session.hpp"
#include "Common/TimeUtilities.hpp"
#include "WorldServer/Channel.hpp"
#include "WorldServer/Channels.hpp"
#include "WorldServer/PlayerDataProvider.hpp"
#include "WorldServer/WorldServerAcceptConnection.hpp"

namespace Vana {
namespace WorldServer {
namespace Packets {
namespace Interserver {

PACKET_IMPL(sendSyncData, function_t<void(PacketBuilder &)> buildSyncData) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::ChannelStart);
	buildSyncData(builder);
	return builder;
}

PACKET_IMPL(Config::scrollingHeader, const string_t &message) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::ScrollingHeader)
		.add<string_t>(message);
	return builder;
}

PACKET_IMPL(Config::setRates, const RatesConfig &rates) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::RateSet)
		.add<RatesConfig>(rates);
	return builder;
}

PACKET_IMPL(Player::newConnectable, player_id_t playerId, const Ip &ip, PacketReader &buffer) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::NewConnectable)
		.add<player_id_t>(playerId)
		.add<Ip>(ip)
		.add<uint16_t>(static_cast<uint16_t>(buffer.getBufferLength()))
		.addBuffer(buffer);
	return builder;
}

PACKET_IMPL(Player::deleteConnectable, player_id_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::DeleteConnectable)
		.add<player_id_t>(playerId);
	return builder;
}

PACKET_IMPL(Player::playerChangeChannel, player_id_t playerId, channel_id_t channelId, const Ip &ip, port_t port) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::ChangeChannelGo)
		.add<player_id_t>(playerId)
		.add<channel_id_t>(channelId)
		.add<Ip>(ip)
		.add<port_t>(port);
	return builder;
}

PACKET_IMPL(Player::updatePlayer, const PlayerData &data, update_bits_t flags) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::UpdatePlayer)
		.add<player_id_t>(data.id)
		.add<update_bits_t>(flags);

	if (flags & Sync::Player::UpdateBits::Full) {
		builder.add<PlayerData>(data);
	}
	else {
		if (flags & Sync::Player::UpdateBits::Level) {
			builder.add<player_level_t>(data.level.get());
		}
		if (flags & Sync::Player::UpdateBits::Job) {
			builder.add<job_id_t>(data.job.get());
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			builder.add<map_id_t>(data.map.get());
		}
		if (flags & Sync::Player::UpdateBits::Transfer) {
			builder.add<bool>(data.transferring);
		}
		if (flags & Sync::Player::UpdateBits::Channel) {
			builder.add<optional_t<channel_id_t>>(data.channel);
		}
		if (flags & Sync::Player::UpdateBits::Ip) {
			builder.add<Ip>(data.ip);
		}
		if (flags & Sync::Player::UpdateBits::Cash) {
			builder.add<bool>(data.cashShop);
		}
		if (flags & Sync::Player::UpdateBits::Mts) {
			builder.add<bool>(data.mts);
		}
	}
	return builder;
}

PACKET_IMPL(Player::characterCreated, const PlayerData &data) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::CharacterCreated)
		.add<PlayerData>(data);
	return builder;
}

PACKET_IMPL(Player::characterDeleted, player_id_t id) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::CharacterDeleted)
		.add<player_id_t>(id);
	return builder;
}

PACKET_IMPL(Party::removePartyMember, party_id_t partyId, player_id_t playerId, bool kicked) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::RemoveMember)
		.add<party_id_t>(partyId)
		.add<player_id_t>(playerId)
		.add<bool>(kicked);
	return builder;
}

PACKET_IMPL(Party::addPartyMember, party_id_t partyId, player_id_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::AddMember)
		.add<party_id_t>(partyId)
		.add<player_id_t>(playerId);
	return builder;
}

PACKET_IMPL(Party::newPartyLeader, party_id_t partyId, player_id_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::SwitchLeader)
		.add<party_id_t>(partyId)
		.add<player_id_t>(playerId);
	return builder;
}

PACKET_IMPL(Party::createParty, party_id_t partyId, player_id_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::Create)
		.add<party_id_t>(partyId)
		.add<player_id_t>(playerId);
	return builder;
}

PACKET_IMPL(Party::disbandParty, party_id_t partyId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<sync_t>(Sync::Party::Disband)
		.add<party_id_t>(partyId);
	return builder;
}

PACKET_IMPL(Buddy::sendBuddyInvite, player_id_t inviteeId, player_id_t inviterId, const string_t &name) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::Invite)
		.add<player_id_t>(inviterId)
		.add<player_id_t>(inviteeId)
		.add<string_t>(name);
	return builder;
}

PACKET_IMPL(Buddy::sendAcceptBuddyInvite, player_id_t inviteeId, player_id_t inviterId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::AcceptInvite)
		.add<player_id_t>(inviterId)
		.add<player_id_t>(inviteeId);
	return builder;
}

PACKET_IMPL(Buddy::sendBuddyRemoval, player_id_t listOwnerId, player_id_t removalId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::RemoveBuddy)
		.add<player_id_t>(listOwnerId)
		.add<player_id_t>(removalId);
	return builder;
}

PACKET_IMPL(Buddy::sendReaddBuddy, player_id_t listOwnerId, player_id_t buddyId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::ReaddBuddy)
		.add<player_id_t>(listOwnerId)
		.add<player_id_t>(buddyId);
	return builder;
}

}
}
}
}