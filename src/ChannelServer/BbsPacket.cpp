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
#include "BbsPacket.h"
#include "ChannelServer.h"
#include "Database.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketReader.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"
#include "WorldServerConnection.h"

void BbsPacket::handleBbsPacket(Player *player, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case 0x00: newThread(player, packet); break;
		case 0x01: deleteThread(player, packet); break;
		case 0x02: sendThreadList((int16_t) packet.get<int32_t>(), player); break;
		case 0x03: showThread(packet.get<int32_t>(), player); break;
		case 0x04: newReply(player, packet); break;
		case 0x05: deleteReply(player, packet); break;
	}
}

void BbsPacket::newThread(Player *player, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::GuildBbs);
	packet.add<int8_t>(0x01);
	packet.add<int32_t>(player->getGuildId());
	packet.add<int32_t>(player->getId());
	packet.addBuffer(pack);
	ChannelServer::Instance()->sendToWorld(packet);
}

void BbsPacket::deleteThread(Player *player, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::GuildBbs);
	packet.add<int8_t>(0x02);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getGuildId());
	packet.add<int32_t>(pack.get<int32_t>());
	ChannelServer::Instance()->sendToWorld(packet);
}

void BbsPacket::newReply(Player *player, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::GuildBbs);
	packet.add<int8_t>(0x03);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getGuildId());
	packet.add<int32_t>(pack.get<int32_t>());
	packet.addString(pack.getString());
	ChannelServer::Instance()->sendToWorld(packet);
}

void BbsPacket::deleteReply(Player *player, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::GuildBbs);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getGuildId());
	packet.add<int32_t>(pack.get<int32_t>());
	packet.add<int32_t>(pack.get<int32_t>());
	ChannelServer::Instance()->sendToWorld(packet);
}

void BbsPacket::sendThreadList(int16_t beginId, Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::GuildBbs);
	packet.add<int8_t>(0x05);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getGuildId());
	packet.add<int32_t>(beginId);
	ChannelServer::Instance()->sendToWorld(packet);
}

void BbsPacket::showThread(int32_t id, Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::GuildBbs);
	packet.add<int8_t>(0x06);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getGuildId());
	packet.add<int32_t>(id);
	ChannelServer::Instance()->sendToWorld(packet);
}
