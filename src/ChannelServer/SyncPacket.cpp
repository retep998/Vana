/*
Copyright (C) 2008-2012 Vana Development Team

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

void SyncPacket::ConfigPacket::scrollingHeader(const string &message) {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_SYNC);
	sendPacket.add<int8_t>(Sync::SyncTypes::Config);
	sendPacket.add<int8_t>(Sync::Config::ScrollingHeader);
	sendPacket.addString(message);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}

void SyncPacket::ConfigPacket::resetRates() {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_SYNC);
	sendPacket.add<int8_t>(Sync::SyncTypes::Config);
	sendPacket.add<int8_t>(Sync::Config::RateReset);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}

void SyncPacket::ConfigPacket::modifyRates(const Rates &rates) {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_SYNC);
	sendPacket.add<int8_t>(Sync::SyncTypes::Config);
	sendPacket.add<int8_t>(Sync::Config::RateSet);
	sendPacket.addClass<Rates>(rates);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}

void SyncPacket::PlayerPacket::updateLevel(int32_t playerId, int32_t level) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdatePlayer);
	packet.add<int8_t>(Sync::Player::UpdateBits::Level);
	packet.add<int32_t>(playerId);
	packet.add<int32_t>(level);
	ChannelServer::Instance()->sendPacketToWorld(packet);
}

void SyncPacket::PlayerPacket::updateJob(int32_t playerId, int32_t job) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdatePlayer);
	packet.add<int8_t>(Sync::Player::UpdateBits::Job);
	packet.add<int32_t>(playerId);
	packet.add<int32_t>(job);
	ChannelServer::Instance()->sendPacketToWorld(packet);
}

void SyncPacket::PlayerPacket::updateMap(int32_t playerId, int32_t map) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdatePlayer);
	packet.add<int8_t>(Sync::Player::UpdateBits::Map);
	packet.add<int32_t>(playerId);
	packet.add<int32_t>(map);
	ChannelServer::Instance()->sendPacketToWorld(packet);
}

void SyncPacket::PlayerPacket::changeChannel(Player *info, uint16_t channel) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeChannelRequest);
	packet.add<int32_t>(info->getId());
	packet.add<uint16_t>(channel);
	packet.add<ip_t>(info->getIp());
	packet.add<int64_t>(info->getConnectionTime());

	packet.addClass<PlayerActiveBuffs>(*info->getActiveBuffs());
	packet.addClass<PlayerSummons>(*info->getSummons());

	ChannelServer::Instance()->sendPacketToWorld(packet);
}

void SyncPacket::PlayerPacket::connect(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::Connect);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getMap());
	ChannelServer::Instance()->sendPacketToWorld(packet);
}

void SyncPacket::PlayerPacket::disconnect(int32_t playerId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::Disconnect);
	packet.add<int32_t>(playerId);
	ChannelServer::Instance()->sendPacketToWorld(packet);
}

void SyncPacket::PlayerPacket::connectableEstablished(int32_t playerId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeChannelGo);
	packet.add<int32_t>(playerId);
	ChannelServer::Instance()->sendPacketToWorld(packet);
}

void SyncPacket::PartyPacket::sync(int8_t type, int32_t playerId, int32_t target) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(type);
	packet.add<int32_t>(playerId);
	if (target != 0) {
		packet.add<int32_t>(target);
	}
	ChannelServer::Instance()->sendPacketToWorld(packet);
}

void SyncPacket::BuddyPacket::buddyInvite(int32_t inviterId, int32_t inviteeId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddy::Invite);
	packet.add<int32_t>(inviterId);
	packet.add<int32_t>(inviteeId);
	ChannelServer::Instance()->sendPacketToWorld(packet);
}

void SyncPacket::BuddyPacket::buddyOnline(int32_t playerId, const vector<int32_t> &players, bool online) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddy::OnlineOffline);
	packet.add<int32_t>(playerId);
	packet.addBool(online);
	packet.addVector(players);
	ChannelServer::Instance()->sendPacketToWorld(packet);
}