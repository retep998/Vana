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
#include "SyncPacket.h"
#include "ChannelServer.h"
#include "Configuration.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "Session.h"

auto SyncPacket::ConfigPacket::scrollingHeader(const string_t &message) -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_SYNC);
	sendPacket.add<int8_t>(Sync::SyncTypes::Config);
	sendPacket.add<int8_t>(Sync::Config::ScrollingHeader);
	sendPacket.addString(message);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto SyncPacket::ConfigPacket::resetRates() -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_SYNC);
	sendPacket.add<int8_t>(Sync::SyncTypes::Config);
	sendPacket.add<int8_t>(Sync::Config::RateReset);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto SyncPacket::ConfigPacket::modifyRates(const Rates &rates) -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_SYNC);
	sendPacket.add<int8_t>(Sync::SyncTypes::Config);
	sendPacket.add<int8_t>(Sync::Config::RateSet);
	sendPacket.addClass<Rates>(rates);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto SyncPacket::PlayerPacket::updateLevel(int32_t playerId, int32_t level) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdatePlayer);
	packet.add<int8_t>(Sync::Player::UpdateBits::Level);
	packet.add<int32_t>(playerId);
	packet.add<int32_t>(level);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::updateJob(int32_t playerId, int32_t job) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdatePlayer);
	packet.add<int8_t>(Sync::Player::UpdateBits::Job);
	packet.add<int32_t>(playerId);
	packet.add<int32_t>(job);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::updateMap(int32_t playerId, int32_t map) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdatePlayer);
	packet.add<int8_t>(Sync::Player::UpdateBits::Map);
	packet.add<int32_t>(playerId);
	packet.add<int32_t>(map);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::changeChannel(Player *info, uint16_t channel) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeChannelRequest);
	packet.add<int32_t>(info->getId());
	packet.add<uint16_t>(channel);
	packet.addClass<Ip>(info->getIp());
	packet.add<int64_t>(info->getConnectionTime());

	packet.addClass<PlayerActiveBuffs>(*info->getActiveBuffs());
	packet.addClass<PlayerSummons>(*info->getSummons());

	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::connect(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::Connect);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getMapId());
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::disconnect(int32_t playerId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::Disconnect);
	packet.add<int32_t>(playerId);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PlayerPacket::connectableEstablished(int32_t playerId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeChannelGo);
	packet.add<int32_t>(playerId);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::PartyPacket::sync(int8_t type, int32_t playerId, int32_t target) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
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
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddy::Invite);
	packet.add<int32_t>(inviterId);
	packet.add<int32_t>(inviteeId);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto SyncPacket::BuddyPacket::buddyOnline(int32_t playerId, const vector_t<int32_t> &players, bool online) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddy::OnlineOffline);
	packet.add<int32_t>(playerId);
	packet.add<bool>(online);
	packet.addVector(players);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}