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
#include "GuildPacket.h"
#include "ChannelServer.h"
#include "Database.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "Maps.h"
#include "Party.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "SendHeader.h"
#include "WorldServerConnection.h"

void GuildPacket::handlePacket(Player *player, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case 0x02: { // Create Guild
			if (player->getGuildId() != 0) {
				PlayerPacket::showMessage(player, "You can't create a guild while you are in one.", PlayerPacket::NoticeTypes::Box);
				return;
			}
			if (player->getInventory()->getMesos() < 5000000) {
				PlayerPacket::showMessage(player, "You have not enough mesos to create a guild. You need 5,000,000 mesos (5 million) to create a guild.", PlayerPacket::NoticeTypes::Box);
				return;
			}
			PacketCreator pack;
			pack.add<int16_t>(IMSG_SYNC);
			pack.add<int8_t>(Sync::SyncTypes::Guild);
			pack.add<int8_t>(Sync::Guild::Create);
			pack.add<int32_t>(0);
			pack.add<int32_t>(player->getId());
			pack.add<int8_t>(1);
			pack.addString(packet.getString());
			ChannelServer::Instance()->sendToWorld(pack);
			break;
		}
		case 0x05: { // Invite
			if (player->getGuildId() <= 0 || player->getGuildRank() > 2) {
				// Hacking
				return;
			}
			guildInvite(player->getGuildId(), player->getId(), packet.getString());
			break;
		}
		case 0x06: guildInviteAccepted(player->getId()); break;
		case 0x07: removeGuildPlayer(player->getGuildId(), player->getId(), player->getName(), false); break; // Leave
		case 0x08: { // Expel
			int32_t memid = packet.get<int32_t>();
			string name = packet.getString();
			if (player->getGuildId() <= 0 || player->getGuildRank() > 2) {
				// Hacking
				return;
			}
			removeGuildPlayer(player->getGuildId(), memid, name, true);
			break;
		}
		case 0x0d: { // Change Rank Titles
			if (player->getGuildId() <= 0 || player->getGuildRank() != 1) {
				// Hacking
				return;
			}
			sendNewGuildRankTitles(player->getGuildId(), packet);
			break;
		}
		case 0x0e: { // Change Rank
			int32_t charid = packet.get<int32_t>();
			int8_t newrank = packet.get<int8_t>();
			if (newrank <= 1 || newrank > 5)
				return;
			if (player->getGuildId() <= 0 || player->getGuildRank() > 2) {
				// Hacking
				return;
			}
			changeRankOfPlayer(player->getGuildId(), player->getId(), charid, newrank);
			break;
		}
		case 0x0f: { // Make Emblem
			if (player->getGuildId() <= 0 || player->getGuildRank() != 1 || player->getMap() != 200000301) {
				// Hacking
				return;
			}
			sendEmblemChangeInfo(player->getGuildId(), player->getId(), packet);
			break;
		}
		case 0x10: { // Change Notice
			if (player->getGuildId() <= 0 || player->getGuildRank() > 2) {
				// Hacking
				return;
			}
			changeGuildNotice(player->getGuildId(), packet.getString());
			break;
		}
		case 0x1e: { // Guild Contract Accepted/Denied
			PacketCreator pack;
			pack.add<int16_t>(IMSG_SYNC);
			pack.add<int8_t>(Sync::SyncTypes::Guild);
			pack.add<int8_t>(Sync::Guild::Create);
			pack.add<int32_t>(0);
			pack.add<int32_t>(packet.get<int32_t>());
			pack.add<int8_t>(2);
			pack.add<int32_t>(player->getParty()->getId());
			pack.add<int8_t>(packet.get<int8_t>());
			ChannelServer::Instance()->sendToWorld(pack);
			break;
		}
	}
}

void GuildPacket::handleDenyPacket(Player *player, PacketReader &packet) {
	if (player->getGuildId() != 0)
		return;
	displayGuildDeny(packet);
}

int8_t GuildPacket::checkGuildExist(string name) {
	if (name.length() < 3 || name.length() > 12)
		return 2;
	return ((PlayerDataProvider::Instance()->getGuild(name) != nullptr) ? 1 : 0);
}

void GuildPacket::sendCreateGuildWindow(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x01);
	player->getSession()->send(packet);
}

void GuildPacket::sendChangeGuildEmblem(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x11);
	player->getSession()->send(packet);
}

void GuildPacket::guildInvite(int32_t guildid, int32_t playerid, const string &name) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::Invite);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	packet.addString(name);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::removeGuildPlayer(int32_t guildid, int32_t playerid, string name, bool expelled) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ExpelOrLeave);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	packet.addString(name);
	packet.add<int8_t>(expelled ? 1 : 0);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::guildInviteAccepted(int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::AcceptInvite);
	packet.add<int32_t>(0);
	packet.add<int32_t>(playerid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::sendNewGuildRankTitles(int32_t guildid, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangeTitles);
	packet.add<int32_t>(guildid);
	packet.addBuffer(pack);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::changeGuildNotice(int32_t guildid, const string &notice) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangeNotice);
	packet.add<int32_t>(guildid);
	packet.addString(notice);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::changeRankOfPlayer(int32_t guildid, int32_t playerid, int32_t victimid, uint8_t newrank) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangeRanks);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(victimid);
	packet.add<uint8_t>(newrank);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::sendIncreaseCapacity(int32_t guildid, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangeCapacity);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::guildDisband(int32_t guildid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::Disband);
	packet.add<int32_t>(guildid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::displayGuildDeny(PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::DenyInvite);
	packet.add<int32_t>(0);
	packet.addBuffer(pack);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::addGuildPoint(int32_t guildid, int32_t amount) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangePoints);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(amount);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::sendEmblemChangeInfo(int32_t guildid, int32_t playerid, PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangeEmblem);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	packet.addBuffer(pack);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::displayGuildRankBoard(int32_t playerid, int32_t npcid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::GetRankBoard);
	packet.add<int32_t>(0);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(npcid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::sendRemoveEmblem(int32_t guildid, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::RemoveEmblem);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::handleEmblemChange(PacketReader &packet) {
	Guild *gi = PlayerDataProvider::Instance()->getGuild(packet.get<int32_t>());
	if (gi == nullptr)
		return;

	GuildLogo logo;
	logo.logo = packet.get<int16_t>();
	logo.color = packet.get<uint8_t>();
	logo.background = packet.get<int16_t>();
	logo.backgroundColor = packet.get<uint8_t>();

	gi->logo = logo;

	int32_t playersToChange = packet.get<int32_t>();

	for (int32_t i = 0; i < playersToChange; i++) {
		Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
		if (player == nullptr)
			continue;
		PacketCreator pack;
		pack.addHeader(SMSG_GUILD_EMBLEM);
		pack.add<int32_t>(player->getId());
		pack.add<int16_t>(logo.background);
		pack.add<uint8_t>(logo.backgroundColor);
		pack.add<int16_t>(logo.logo);
		pack.add<uint8_t>(logo.color);
		Maps::getMap(player->getMap())->sendPacket(pack, player);
	}
}

void GuildPacket::handleNameChange(PacketReader &packet) {
	int32_t guildid = packet.get<int32_t>();
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	int32_t playersToChange = packet.get<int32_t>();
	GuildLogo logo;
	string name = guildid != -1 ? guild->name : "";

	for (int32_t i = 0; i < playersToChange; i++) {
		Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
		if (player == nullptr)
			continue;
		player->setGuildId(guildid);
		player->setGuildRank(packet.get<uint8_t>());

		PacketCreator pack;
		pack.addHeader(SMSG_GUILD_NAME);
		pack.add<int32_t>(player->getId());
		pack.addString(name);
		Maps::getMap(player->getMap())->sendPacket(pack, player);
		if (guildid != -1 && PlayerDataProvider::Instance()->hasEmblem(guildid)) {
			logo = guild->logo;
			PacketCreator pack = PacketCreator();
			pack.addHeader(SMSG_GUILD_EMBLEM);
			pack.add<int32_t>(player->getId());
			pack.add<int16_t>(logo.background);
			pack.add<uint8_t>(logo.backgroundColor);
			pack.add<int16_t>(logo.logo);
			pack.add<uint8_t>(logo.color);
			Maps::getMap(player->getMap())->sendPacket(pack, player);
		}
	}
}
