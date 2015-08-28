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
#include "ChannelServer.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "RatesConfig.hpp"
#include "Session.hpp"

namespace Vana {
namespace Packets {
namespace Interserver {

PACKET_IMPL(Config::scrollingHeader, const string_t &message) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::ScrollingHeader)
		.add<string_t>(message);
	return builder;
}

PACKET_IMPL(Config::resetRates, int32_t flags) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::RateReset)
		.add<int32_t>(flags);
	return builder;
}

PACKET_IMPL(Config::modifyRates, const RatesConfig &rates) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::RateSet)
		.add<RatesConfig>(rates);
	return builder;
}

PACKET_IMPL(Player::updatePlayer, const PlayerData &player, update_bits_t flags) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::UpdatePlayer)
		.add<update_bits_t>(flags)
		.add<player_id_t>(player.id);

	if (flags & Sync::Player::UpdateBits::Full) {
		builder.add<PlayerData>(player);
	}
	else {
		if (flags & Sync::Player::UpdateBits::Level) {
			builder.add<int16_t>(player.level.get());
		}
		if (flags & Sync::Player::UpdateBits::Job) {
			builder.add<int16_t>(player.job.get());
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			builder.add<int32_t>(player.map.get());
		}
		if (flags & Sync::Player::UpdateBits::Channel) {
			builder.add<channel_id_t>(player.channel.get());
		}
		if (flags & Sync::Player::UpdateBits::Ip) {
			builder.add<Ip>(player.ip);
		}
		if (flags & Sync::Player::UpdateBits::Cash) {
			builder.add<bool>(player.cashShop);
		}
		if (flags & Sync::Player::UpdateBits::Mts) {
			builder.add<bool>(player.mts);
		}
	}
	return builder;
}

PACKET_IMPL(Player::changeChannel, Vana::Player *info, channel_id_t channel) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::ChangeChannelRequest)
		.add<player_id_t>(info->getId())
		.add<channel_id_t>(channel)
		.add<Ip>(info->getIp())
		.addBuffer(info->getTransferPacket());

	return builder;
}

PACKET_IMPL(Player::connect, const PlayerData &player, bool firstConnect) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::Connect)
		.add<bool>(firstConnect)
		.add<player_id_t>(player.id);

	if (firstConnect) {
		builder.add<PlayerData>(player);
	}
	else {
		builder
			.add<map_id_t>(player.map.get())
			.add<channel_id_t>(player.channel.get())
			.add<Ip>(player.ip);
	}

	return builder;
}

PACKET_IMPL(Player::disconnect, player_id_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::Disconnect)
		.add<player_id_t>(playerId);
	return builder;
}

PACKET_IMPL(Player::connectableEstablished, player_id_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::ChangeChannelGo)
		.add<player_id_t>(playerId);
	return builder;
}

PACKET_IMPL(Party::sync, int8_t type, player_id_t playerId, int32_t target) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<int8_t>(type)
		.add<player_id_t>(playerId);

	if (target != 0) {
		builder.add<int32_t>(target);
	}
	return builder;
}

PACKET_IMPL(Buddy::buddyInvite, player_id_t inviterId, player_id_t inviteeId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::Invite)
		.add<player_id_t>(inviterId)
		.add<player_id_t>(inviteeId);
	return builder;
}

PACKET_IMPL(Buddy::acceptBuddyInvite, player_id_t inviteeId, player_id_t inviterId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::AcceptInvite)
		.add<player_id_t>(inviteeId)
		.add<player_id_t>(inviterId);
	return builder;
}

PACKET_IMPL(Buddy::removeBuddy, player_id_t listOwnerId, player_id_t removalId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::RemoveBuddy)
		.add<player_id_t>(listOwnerId)
		.add<player_id_t>(removalId);
	return builder;
}

PACKET_IMPL(Buddy::readdBuddy, player_id_t listOwnerId, player_id_t buddyId) {
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