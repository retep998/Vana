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

void SyncPacket::playerChangeChannel(WorldServerConnection *player, Player *info, uint16_t channel) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeServerRequest);
	packet.add<int32_t>(info->getId());
	packet.addBool(false);

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

void SyncPacket::playerBuffsTransferred(WorldServerConnection *player, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeServerGo);
	packet.add<int32_t>(playerid);
	packet.addBool(true);
	packet.addBool(true);
	player->getSession()->send(packet);
}
