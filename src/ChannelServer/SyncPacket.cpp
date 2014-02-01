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
#include "ChannelServer.hpp"
#include "Configuration.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "Session.hpp"

namespace SyncPacket {

PACKET_IMPL(ConfigPacket::scrollingHeader, const string_t &message) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::ScrollingHeader)
		.addString(message);
	return builder;
}

PACKET_IMPL(ConfigPacket::resetRates) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::RateReset);
	return builder;
}

PACKET_IMPL(ConfigPacket::modifyRates, const Rates &rates) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Config)
		.add<sync_t>(Sync::Config::RateSet)
		.addClass<Rates>(rates);
	return builder;
}

PACKET_IMPL(PlayerPacket::updatePlayer, const PlayerData &player, update_bits_t flags) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::UpdatePlayer)
		.add<update_bits_t>(flags)
		.add<int32_t>(player.id);

	if (flags & Sync::Player::UpdateBits::Full) {
		builder.addClass<PlayerData>(player);
	}
	else {
		if (flags & Sync::Player::UpdateBits::Level) {
			builder.add<int16_t>(player.level);
		}
		if (flags & Sync::Player::UpdateBits::Job) {
			builder.add<int16_t>(player.job);
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			builder.add<int32_t>(player.map);
		}
		if (flags & Sync::Player::UpdateBits::Channel) {
			builder.add<channel_id_t>(player.channel);
		}
		if (flags & Sync::Player::UpdateBits::Ip) {
			builder.add<Ip>(player.ip);
		}
		if (flags & Sync::Player::UpdateBits::Cash) {
			builder.add<bool>(player.cashShop);
		}
	}
	return builder;
}

PACKET_IMPL(PlayerPacket::changeChannel, Player *info, channel_id_t channel) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::ChangeChannelRequest)
		.add<int32_t>(info->getId())
		.add<channel_id_t>(channel)
		.addClass<Ip>(info->getIp())
		.add<int64_t>(info->getConnectionTime());

	Player *followed = info->getFollow();
	builder
		.add<int32_t>(followed != nullptr ? followed->getId() : 0)
		.add<bool>(info->isGmChat())
		.addClass<PlayerActiveBuffs>(*info->getActiveBuffs())
		.addClass<PlayerSummons>(*info->getSummons());
	return builder;
}

PACKET_IMPL(PlayerPacket::connect, const PlayerData &player, bool firstConnect) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::Connect)
		.add<bool>(firstConnect)
		.add<int32_t>(player.id);

	if (firstConnect) {
		builder.addClass<PlayerData>(player);
	}
	else {
		builder
			.add<int32_t>(player.map)
			.add<channel_id_t>(player.channel)
			.add<Ip>(player.ip);
	}

	return builder;
}

PACKET_IMPL(PlayerPacket::disconnect, int32_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::Disconnect)
		.add<int32_t>(playerId);
	return builder;
}

PACKET_IMPL(PlayerPacket::connectableEstablished, int32_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::ChangeChannelGo)
		.add<int32_t>(playerId);
	return builder;
}

PACKET_IMPL(PartyPacket::sync, int8_t type, int32_t playerId, int32_t target) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Party)
		.add<int8_t>(type)
		.add<int32_t>(playerId);

	if (target != 0) {
		builder.add<int32_t>(target);
	}
	return builder;
}

PACKET_IMPL(BuddyPacket::buddyInvite, int32_t inviterId, int32_t inviteeId) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::Invite)
		.add<int32_t>(inviterId)
		.add<int32_t>(inviteeId);
	return builder;
}

PACKET_IMPL(BuddyPacket::buddyOnline, int32_t playerId, const vector_t<int32_t> &players, bool online) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Buddy)
		.add<sync_t>(Sync::Buddy::OnlineOffline)
		.add<int32_t>(playerId)
		.add<bool>(online)
		.addVector(players);
	return builder;
}

}