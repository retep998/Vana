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
#include "PacketCreator.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "Session.hpp"

auto SyncPacket::ConfigPacket::scrollingHeader(const string_t &message) -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_SYNC);
	sendPacket.add<sync_t>(Sync::SyncTypes::Config);
	sendPacket.add<sync_t>(Sync::Config::ScrollingHeader);
	sendPacket.addString(message);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto SyncPacket::ConfigPacket::resetRates() -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_SYNC);
	sendPacket.add<sync_t>(Sync::SyncTypes::Config);
	sendPacket.add<sync_t>(Sync::Config::RateReset);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto SyncPacket::ConfigPacket::modifyRates(const Rates &rates) -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_SYNC);
	sendPacket.add<sync_t>(Sync::SyncTypes::Config);
	sendPacket.add<sync_t>(Sync::Config::RateSet);
	sendPacket.addClass<Rates>(rates);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto SyncPacket::PlayerPacket::updatePlayer(const PlayerData &player, update_bits_t flags) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::UpdatePlayer);
	packet.add<update_bits_t>(flags);
	packet.add<int32_t>(player.id);
	if (flags & Sync::Player::UpdateBits::Full) {
		packet.addClass<PlayerData>(player);
	}
	else {
		if (flags & Sync::Player::UpdateBits::Level) {
			packet.add<int16_t>(player.level);
		}
		if (flags & Sync::Player::UpdateBits::Job) {
			packet.add<int16_t>(player.job);
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			packet.add<int32_t>(player.map);
		}
		if (flags & Sync::Player::UpdateBits::Channel) {
			packet.add<channel_id_t>(player.channel);
		}
	}
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::changeChannel(Player *info, channel_id_t channel) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::ChangeChannelRequest);
	packet.add<int32_t>(info->getId());
	packet.add<channel_id_t>(channel);
	packet.addClass<Ip>(info->getIp());
	packet.add<int64_t>(info->getConnectionTime());
	Player *followed = info->getFollow();
	packet.add<int32_t>(followed != nullptr ? followed->getId() : 0);
	packet.add<bool>(info->isGmChat());

	packet.addClass<PlayerActiveBuffs>(*info->getActiveBuffs());
	packet.addClass<PlayerSummons>(*info->getSummons());

	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::connect(const PlayerData &player) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::Connect);
	packet.addClass<PlayerData>(player);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::disconnect(int32_t playerId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::Disconnect);
	packet.add<int32_t>(playerId);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::connectableEstablished(int32_t playerId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::ChangeChannelGo);
	packet.add<int32_t>(playerId);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PartyPacket::sync(int8_t type, int32_t playerId, int32_t target) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(type);
	packet.add<int32_t>(playerId);
	if (target != 0) {
		packet.add<int32_t>(target);
	}
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::BuddyPacket::buddyInvite(int32_t inviterId, int32_t inviteeId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Buddy);
	packet.add<sync_t>(Sync::Buddy::Invite);
	packet.add<int32_t>(inviterId);
	packet.add<int32_t>(inviteeId);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::BuddyPacket::buddyOnline(int32_t playerId, const vector_t<int32_t> &players, bool online) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Buddy);
	packet.add<sync_t>(Sync::Buddy::OnlineOffline);
	packet.add<int32_t>(playerId);
	packet.add<bool>(online);
	packet.addVector(players);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}