/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "InterHeader.h"
#include "InterHelper.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "WorldServerConnection.h"

void SyncPacket::updateLevel(WorldServerConnection *player, int32_t playerid, uint8_t level) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdateLevel);
	packet.add<int32_t>(playerid);
	packet.add<uint8_t>(level);
	player->getSession()->send(packet);
}

void SyncPacket::updateJob(WorldServerConnection *player, int32_t playerid, int16_t job) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdateJob);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(job);
	player->getSession()->send(packet);
}

void SyncPacket::updateMap(WorldServerConnection *player, int32_t playerid, int32_t map) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdateMap);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(map);
	player->getSession()->send(packet);
}

void SyncPacket::partyOperation(WorldServerConnection *player, int8_t type, int32_t playerid, int32_t target) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(type);
	packet.add<int32_t>(playerid);
	if (target != 0) {
		packet.add<int32_t>(target);
	}
	player->getSession()->send(packet);
}

void SyncPacket::partyInvite(WorldServerConnection *player, int32_t playerid, const string &invitee) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(playerid);
	packet.addString(invitee);
	player->getSession()->send(packet);
}

void SyncPacket::playerChangeChannel(WorldServerConnection *player, Player *info, uint16_t channel) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeChannelRequest);
	packet.add<int32_t>(info->getId());
	packet.add<int16_t>(channel);

	packet.add<int64_t>(info->getConnectionTime());

	info->getActiveBuffs()->getBuffTransferPacket(packet);
	info->getSummons()->getSummonTransferPacket(packet);

	player->getSession()->send(packet);
}

void SyncPacket::playerChangeServer(WorldServerConnection *player, Player *info, bool cashShop) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeServerRequest);
	packet.add<int32_t>(info->getId());
	packet.addBool(true);
	packet.addBool(cashShop);

	packet.add<int64_t>(info->getConnectionTime());

	info->getActiveBuffs()->getBuffTransferPacket(packet);
	info->getSummons()->getSummonTransferPacket(packet);

	player->getSession()->send(packet);
}

void SyncPacket::registerPlayer(WorldServerConnection *player, uint32_t ip, int32_t playerid, const string &name, int32_t map, int16_t job, uint8_t level, int32_t guildid, uint8_t guildrank, int32_t allianceid, uint8_t alliancerank) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::Connect);
	packet.add<uint32_t>(ip);
	packet.add<int32_t>(playerid);
	packet.addString(name);
	packet.add<int32_t>(map);
	packet.add<int16_t>(job);
	packet.add<uint8_t>(level);
	packet.add<int32_t>(guildid);
	packet.add<uint8_t>(guildrank);
	packet.add<int32_t>(allianceid);
	packet.add<uint8_t>(alliancerank);
	player->getSession()->send(packet);
}

void SyncPacket::removePlayer(WorldServerConnection *player, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::Disconnect);
	packet.add<int32_t>(playerid);
	player->getSession()->send(packet);
}

void SyncPacket::playerBuffsTransferred(WorldServerConnection *player, int32_t playerid, bool fromCashOrMts) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(fromCashOrMts ? Sync::Player::ChangeServerGo : Sync::Player::ChangeChannelGo);
	packet.add<int32_t>(playerid);
	if (fromCashOrMts) {
		packet.addBool(false);
		packet.addBool(false);
	}
	player->getSession()->send(packet);
}

void SyncPacket::buddyInvite(WorldServerConnection *player, int32_t playerid, int32_t inviteeid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddies::Invite);
	packet.add<int32_t>(inviteeid);
	packet.add<int32_t>(playerid);
	player->getSession()->send(packet);
}

void SyncPacket::buddyOnline(WorldServerConnection *player, int32_t playerid, vector<int32_t> players, bool online) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddies::OnlineOffline);
	packet.add<int32_t>(playerid);
	packet.addBool(online);
	packet.addVector(players);
	player->getSession()->send(packet);
}
