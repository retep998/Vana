/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "Guilds.h"
#include "InterHeader.h"
#include "Maps.h"
#include "Party.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "PlayerPacket.h"
#include "Player.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "SendHeader.h"
#include "WorldServerConnection.h"

void GuildPacket::handlePacket(Player *player, PacketReader &packet) {
	switch(packet.get<int8_t>()) {
		case 0x02:{ // Create guild
			if (player->getGuildId() != 0) {
				PlayerPacket::showMessage(player, "You can't create a guild while you are in one.", 1);
				return;
			}

			if (player->getInventory()->getMesos() < 5000000) {
				PlayerPacket::showMessage(player, "You have not enough mesos to create a guild. You need 5,000,000 mesos (5 million) to create a guild.", 1);
				return;
			}

			PacketCreator pack;
			pack.add<int16_t>(INTER_GUILD_OPERATION);
			pack.add<int8_t>(0x0d);
			pack.add<int32_t>(0);
			pack.add<int32_t>(player->getId());
			pack.add<int8_t>(1);
			pack.addString(packet.getString());
			ChannelServer::Instance()->sendToWorld(pack);
		}
		break;
		case 0x05: // Invite someone
		{
			if (player->getGuildRank() > 2 || player->getGuildId() <= 0) {
				std::cout << player->getName() << " wants to send a guild invitation without having permission... (Guildid = " << player->getGuildId() << ", GuildRank = " << player->getGuildRank() << ")" << std::endl;
				return;
			}
			
			guildInvite(player->getGuildId(), player->getId(), packet.getString());
				
		}
		break;
		case 0x06: guildInviteAccepted(player->getId()); break;
		case 0x07: removeGuildPlayer(player->getGuildId(), player->getId(), player->getName(), false); break; // Leave
		case 0x08: // Expelling
		{
			int32_t memid = packet.get<int32_t>();
			string name = packet.getString();

			if (player->getGuildRank() > 2) {
				std::cout << player->getName() << " tries to expel a player without rank 1 or 2" << std::endl;
				return;
			}

			removeGuildPlayer(player->getGuildId(), memid, name, true);
			
		}
		break;
		case 0x0d: // Change titles
		{
			if (player->getGuildId() <= 0 || player->getGuildRank() != 1) {
				std::cout << player->getName() << " wants to change the rank titles..." << std::endl;
				return;
			}

			sendNewGuildRankTitles(player->getGuildId(), packet);

		}
		break;
		case 0x0e: // Change rank
		{
			int32_t charid = packet.get<int32_t>();
			int8_t newrank = packet.get<int8_t>();

			if (newrank <= 1 || newrank > 5)
				return;

			if (player->getGuildId() <= 0 || player->getGuildRank() > 2) {
				std::cout << player->getName() << " wants to change the rank of player with id " << charid << std::endl;
				return;
			}
			changeRankOfPlayer(player->getGuildId(), player->getId(), charid, newrank);
		}
		break;
		case 0x0f: // Make emblem
		{

			if (player->getGuildId() <= 0 || player->getGuildRank() != 1 || (!player->isGm() && player->getMap() != 200000301)) {
				std::cout << player->getName() << " wants to change the guild emblem without being a guild leader!" << std::endl;
				return;
			}

			sendEmblemChangeInfo(player->getGuildId(), player->getId(), packet);
		}
		break;
		case 0x10: // Change notice
		{
			if (player->getGuildId() <= 0 || player->getGuildRank() > 2) {
				std::cout << player->getName() << " (rank " << (int16_t) player->getGuildRank() << ") Wants to change the notice..." << std::endl;
				return;
			}

			changeGuildNotice(player->getGuildId(), packet.getString());
		}
		break;
		case 0x1e: // Guild Contract accepted/denied
		{
			PacketCreator pack;
			pack.add<int16_t>(INTER_GUILD_OPERATION);
			pack.add<int8_t>(0x0d);
			pack.add<int32_t>(0);
			pack.add<int32_t>(packet.get<int32_t>());
			pack.add<int8_t>(2);
			pack.add<int32_t>(player->getParty()->getId());
			pack.add<int8_t>(packet.get<int8_t>());
			ChannelServer::Instance()->sendToWorld(pack);
		}
		break;
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
	else if (Guilds::Instance()->getGuild(name) != 0)
		return 1;
	else
		return 0;
}

void GuildPacket::sendCreateGuildWindow(Player * player) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_GUILD);
	packet.add<int8_t>(0x01);
	
	player->getSession()->send(packet);
}

void GuildPacket::sendChangeGuildEmblem(Player * player) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_GUILD);
	packet.add<int8_t>(0x11);
	player->getSession()->send(packet);
}

void GuildPacket::guildInvite(int32_t guildid, int32_t playerid, const string &name) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x01);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	packet.addString(name);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::removeGuildPlayer(int32_t guildid, int32_t playerid, string name, bool expelled) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x02);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	packet.addString(name);
	packet.add<int8_t>(expelled ? 1 : 0);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::guildInviteAccepted(int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x03);
	packet.add<int32_t>(0);
	packet.add<int32_t>(playerid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::sendNewGuildRankTitles(int32_t guildid, PacketReader &packet) {
	PacketCreator pack;
	pack.add<int16_t>(INTER_GUILD_OPERATION);
	pack.add<int8_t>(0x04);
	pack.add<int32_t>(guildid);
	pack.addBuffer(packet);
	ChannelServer::Instance()->sendToWorld(pack);
}

void GuildPacket::changeGuildNotice(int32_t guildid, const string &notice) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x05);
	packet.add<int32_t>(guildid);
	packet.addString(notice);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::changeRankOfPlayer(int32_t guildid, int32_t playerid, int32_t victimid, uint8_t newrank) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x06);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(victimid);
	packet.add<uint8_t>(newrank);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::sendIncreaseCapacity(int32_t guildid, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x07);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::guildDisband(int32_t guildid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x08);
	packet.add<int32_t>(guildid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::displayGuildDeny(PacketReader &pack) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x09);
	packet.add<int32_t>(0);
	packet.addBuffer(pack);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::addGuildPoint(int32_t guildid, int32_t amount) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x0a);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(amount);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::sendEmblemChangeInfo(int32_t guildid, int32_t playerid, PacketReader &packet) {
	PacketCreator pack;
	pack.add<int16_t>(INTER_GUILD_OPERATION);
	pack.add<int8_t>(0x0b);
	pack.add<int32_t>(guildid);
	pack.add<int32_t>(playerid);
	pack.addBuffer(packet);
	ChannelServer::Instance()->sendToWorld(pack);
}

void GuildPacket::displayGuildRankBoard(int32_t playerid, int32_t npcid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x0c);
	packet.add<int32_t>(0);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(npcid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::sendRemoveEmblem(int32_t guildid, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GUILD_OPERATION);
	packet.add<int8_t>(0x0f);
	packet.add<int32_t>(guildid);
	packet.add<int32_t>(playerid);
	ChannelServer::Instance()->sendToWorld(packet);
}

void GuildPacket::handleEmblemChange(PacketReader &packet) {
	Guild * gi = Guilds::Instance()->getGuild(packet.get<int32_t>());
	if (gi == 0) 
		return;
	int16_t logo = packet.get<int16_t>();
	uint8_t logocolor = packet.get<uint8_t>();
	int16_t logobg = packet.get<int16_t>();
	uint8_t logobgcolor = packet.get<uint8_t>();

	gi->logo = logo;
	gi->logocolor = logocolor;
	gi->logobg = logobg;
	gi->logobgcolor = logobgcolor;

	int32_t playersToChange = packet.get<int32_t>();

	for (int32_t i = 0; i < playersToChange; i++){
		Player * player = Players::Instance()->getPlayer(packet.get<int32_t>());
		if (player == 0) continue;
		PacketCreator pack;
		pack.add<int16_t>(SMSG_GUILD_EMBLEM);
		pack.add<int32_t>(player->getId());
		pack.add<int16_t>(logobg);
		pack.add<uint8_t>(logobgcolor);
		pack.add<int16_t>(logo);
		pack.add<uint8_t>(logocolor);
		Maps::getMap(player->getMap())->sendPacket(pack, player);	
	}
}

void GuildPacket::handleNameChange(PacketReader &packet) {
	string name = packet.getString();
	int32_t guildid = packet.get<int32_t>();
	int32_t playersToChange = packet.get<int32_t>();

	for (int32_t i = 0; i < playersToChange; i++) {
		Player * player = Players::Instance()->getPlayer(packet.get<int32_t>());
		if (player == 0) continue;
		player->setGuildId(guildid);
		player->setGuildRank(packet.get<uint8_t>());

		PacketCreator pack;
		pack.add<int16_t>(SMSG_GUILD_NAME);
		pack.add<int32_t>(player->getId());
		pack.addString(name);
		Maps::getMap(player->getMap())->sendPacket(pack, player);
	}
}
