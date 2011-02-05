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
#include "AlliancePacket.h"
#include "ChannelServer.h"
#include "Database.h"
#include "GuildPacket.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "SendHeader.h"
#include "WorldServerConnection.h"

void AlliancePacket::handlePacket(Player *player, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case 0x01: sendShowInfo(player->getAllianceId(), player->getId()); break;
		case 0x02: { // Leave Alliance
				if (player->getAllianceId() == 0 || player->getAllianceRank() != 2 || player->getGuildId() == 0 || player->getGuildRank() != 1)
					return;
				sendChangeGuild(player->getAllianceId(), player->getId(), player->getGuildId(), 2);
				break;
		}
		case 0x03: sendInvitation(player->getAllianceId(), player->getId(), packet.getString()); break;
		case 0x04: { // Invite accepted
				int32_t guildid = packet.get<int32_t>();
				string guildname = packet.getString();
				if (player->getAllianceId() != 0 || player->getGuildRank() != 1 || player->getGuildId() == 0)
					return;
				sendChangeGuild(guildid, player->getId(), player->getGuildId(), 0);
				break;
		}
		case 0x06: { // Expel Guild
				int32_t guildid = packet.get<int32_t>();
				int32_t allianceid = packet.get<int32_t>();
				if (player->getAllianceId() == 0 || player->getAllianceRank() != 1 || player->getAllianceId() != allianceid)
					return;
				sendChangeGuild(allianceid, player->getId(), guildid, 1);
				break;
		}
		case 0x07: { // Change Alliance Leader
				if (player->getAllianceId() == 0 || player->getAllianceRank() != 1 || player->getGuildId() == 0 || player->getGuildRank() > 1)
					return;
				sendChangeLeader(player->getAllianceId(), player->getId(), packet.get<int32_t>());
				break;
		}
		case 0x08: sendChangeTitles(player->getAllianceId(), player->getId(), packet); break;
		case 0x09: sendChangeRank(player->getAllianceId(), player->getId(), packet); break;
		case 0x0a: sendChangeNotice(player->getAllianceId(), player->getId(), packet); break;
	}
}

void AlliancePacket::handleDenyPacket(Player *player, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::InviteDenied);
	packet.add<int32_t>(0);
	packet.add<int32_t>(player->getId());
	packet.addBuffer(pack);
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::sendShowInfo(int32_t allianceid, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::GetInfo);
	packet.add<int32_t>(allianceid);
	packet.add<int32_t>(playerid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::sendChangeTitles(int32_t allianceid, int32_t playerid, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeTitles);
	packet.add<int32_t>(allianceid);
	packet.add<int32_t>(playerid);
	packet.addBuffer(pack);
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::sendChangeNotice(int32_t allianceid, int32_t playerid, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeNotice);
	packet.add<int32_t>(allianceid);
	packet.add<int32_t>(playerid);
	packet.addString(pack.getString());
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::sendInvitation(int32_t allianceid, int32_t playerid, const string &guildname) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::Invite);
	packet.add<int32_t>(allianceid);
	packet.add<int32_t>(playerid);
	packet.addString(guildname);
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::sendChangeGuild(int32_t allianceid, int32_t playerid, int32_t guildid, uint8_t option) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeGuild);
	packet.add<int32_t>(allianceid);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	packet.add<uint8_t>(option);
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::sendChangeLeader(int32_t allianceid, int32_t playerid, int32_t victim) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeLeader);
	packet.add<int32_t>(allianceid);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(victim);
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::sendChangeRank(int32_t allianceid, int32_t playerid, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeRank);
	packet.add<int32_t>(allianceid);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(pack.get<int32_t>());
	packet.add<uint8_t>(pack.get<uint8_t>());
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::sendCreateAlliance(int32_t playerid, string alliancename) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::Create);
	packet.add<int32_t>(0);
	packet.add<int32_t>(playerid);
	packet.addString(alliancename);
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::increaseAllianceCapacity(int32_t allianceid, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeCapacity);
	packet.add<int32_t>(allianceid);
	packet.add<int32_t>(playerid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void AlliancePacket::sendDisbandAlliance(int32_t allianceid, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::Disband);
	packet.add<int32_t>(allianceid);
	packet.add<int32_t>(playerid);
	ChannelServer::Instance()->sendToWorld(packet);
}
