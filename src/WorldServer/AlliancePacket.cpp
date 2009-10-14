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
#include "AlliancePacket.h"
#include "Channels.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Players.h"
#include "SendHeader.h"
#include "TimeUtilities.h"
#include "WorldServerAcceptConnection.h"

// AlliancePacket InterServerPacket namespace
void AlliancePacket::InterServerPacket::changeAlliance(Alliance *alliance, int8_t type) {
	unordered_map<int32_t, Guild *> guilds = alliance->getGuilds();

	PacketCreator packet;
	packet.add<int16_t>(INTER_ALLIANCE);
	packet.add<int8_t>(0x01);
	packet.add<uint8_t>(type);
	packet.add<int32_t>(type == 1 ? alliance->getId() : 0);
	packet.add<uint8_t>(alliance->getSize());
	for (unordered_map<int32_t, Guild *>::iterator iter = guilds.begin(); iter != guilds.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
		packet.add<int32_t>(iter->second->m_players.size());
		for (unordered_map<int32_t, Player *>::iterator iter2 = iter->second->m_players.begin(); iter2 != iter->second->m_players.end(); iter2++) {
			packet.add<int32_t>(iter2->second->id);
			packet.add<uint8_t>(type == 1 ? iter2->second->alliancerank : 0);
		}
	}

	Channels::Instance()->sendToAll(packet);
}

void AlliancePacket::InterServerPacket::changeGuild(Alliance *alliance, Guild *guild) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_ALLIANCE);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(alliance == 0 ? 0 : alliance->getId());
	packet.add<int32_t>(guild->getId());
	packet.add<int32_t>(guild->m_players.size());

	unordered_map<int32_t, Player *>::iterator iter;
	for (iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
		packet.add<int32_t>(iter->second->id);
		packet.add<uint8_t>(alliance == 0 ? 5 : iter->second->alliancerank);
	}

	Channels::Instance()->sendToAll(packet);
}

void AlliancePacket::InterServerPacket::changeLeader(Alliance *alliance, Player *oldLeader) {
	PacketCreator pack;
	pack.add<int16_t>(INTER_ALLIANCE);
	pack.add<int8_t>(0x02);

	pack.add<int32_t>(alliance->getId());
	pack.add<int32_t>(oldLeader->id);
	pack.add<int32_t>(alliance->getLeaderId());
	Channels::Instance()->sendToAll(pack);
}

void AlliancePacket::InterServerPacket::changePlayerRank(Alliance *alliance, Player *player) {
	PacketCreator pack;
	pack.add<int16_t>(INTER_ALLIANCE);
	pack.add<int8_t>(0x05);

	pack.add<int32_t>(alliance->getId());
	pack.add<int32_t>(player->id);
	pack.add<int32_t>(player->alliancerank);
	Channels::Instance()->sendToAll(pack);
}

void AlliancePacket::InterServerPacket::changeCapacity(Alliance *alliance) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_ALLIANCE);
	packet.add<int8_t>(0x03);

	packet.add<int32_t>(alliance->getId());
	packet.add<int32_t>(alliance->getCapacity());
	Channels::Instance()->sendToAll(packet);
}

// AlliancePacket namespace
void AlliancePacket::sendAllianceInfo(Alliance *alliance, Player *requestee) {
	WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(requestee->channel)->player;
	if (channel == 0)
		return;
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(requestee->id);

	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x0c);

	if (alliance == 0) {
		std::cout << "Alliance not found: Player " << requestee->name << ", allianceid " << alliance->getId() << std::endl;
		packet.addBool(false);
		channel->getSession()->send(packet);
		return; // Dont send any packets
	}

	packet.addBool(true);

	addAllianceInfo(packet, alliance);
	
	channel->getSession()->send(packet);
	
	packet = PacketCreator();
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(requestee->id);

	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x0D);

	addGuildsInfo(packet, alliance);

	channel->getSession()->send(packet);
}

void AlliancePacket::sendInvite(Alliance *alliance, Player *inviter, Player *invitee) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(invitee->id);

	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x03);

	packet.add<int32_t>(inviter->guildid);
	packet.addString(inviter->name);
	packet.addString(alliance->getName());

	WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(invitee->channel)->player;
	if (channel != 0)
		channel->getSession()->send(packet);
}

void AlliancePacket::sendInviteAccepted(Alliance *alliance, Guild *guild) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x12);

	addAllianceInfo(packet, alliance);

	packet.add<int32_t>(guild->getId());
	GuildPacket::addGuildInformation(packet, guild);

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendInviteDenied(Alliance *alliance, Guild *guild) {
	Player *leader = Players::Instance()->getPlayer(alliance->getLeaderId());

	// GMS doesnt have an actual deny packet. They just use the note packet...
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(leader->id);

	packet.add<int16_t>(SMSG_NOTE);
	packet.add<int8_t>(0x09);

	packet.addString(guild->getName() + " Guild has rejected the Guild Union invitation.");

	WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(leader->channel)->player;
	if (channel != 0)
		channel->getSession()->send(packet);
}

void AlliancePacket::sendGuildLeft(Alliance *alliance, Guild *guild, bool expelled) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x10);

	addAllianceInfo(packet, alliance);

	packet.add<int32_t>(guild->getId()); // Expelled/left guild ID
	GuildPacket::addGuildInformation(packet, guild); // Its data 0.o
	packet.add<int8_t>(expelled);

	sendToAlliance(packet, alliance);
	GuildPacket::sendToGuild(packet, guild);
}

void AlliancePacket::sendNewAlliance(Alliance *alliance) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x0f);

	addAllianceInfo(packet, alliance);
	addGuildsInfo(packet, alliance, false);

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendDeleteAlliance(Alliance *alliance) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x1d);

	packet.add<int32_t>(alliance->getId());

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendUpdateLeader(Alliance *alliance, Player *oldLeader) { 
	PacketCreator packet;
	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x19);

	packet.add<int32_t>(alliance->getId());
	packet.add<int32_t>(oldLeader->id); // From
	packet.add<int32_t>(alliance->getLeaderId()); // To

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendUpdatePlayer(Alliance *alliance, Player *player, uint8_t option) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_ALLIANCE);

	if (option == 0) {
		packet.add<int8_t>(0x1b);

		packet.add<int32_t>(player->id);
		packet.add<uint8_t>(player->alliancerank);
	}
	else if (option == 1) {
		packet.add<int8_t>(0x18);

		packet.add<int32_t>(alliance->getId());
		packet.add<int32_t>(player->guildid);
		packet.add<int32_t>(player->id);
		packet.add<int32_t>(player->level);
		packet.add<int32_t>(player->job);
	}
	else {
		packet.add<int8_t>(0x0e);

		packet.add<int32_t>(alliance->getId());
		packet.add<int32_t>(player->guildid);
		packet.add<int32_t>(player->id);
		packet.add<uint8_t>(player->online);
	}

	sendToAlliance(packet, alliance, (option == 2 ? player : 0));
}

void AlliancePacket::sendUpdateNotice(Alliance *alliance) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x1c);

	packet.add<int32_t>(alliance->getId());
	packet.addString(alliance->getNotice());
	
	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendUpdateCapacity(Alliance *alliance) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x17);

	addAllianceInfo(packet, alliance);

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendUpdateTitles(Alliance *alliance) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_ALLIANCE);
	packet.add<int8_t>(0x1a);

	packet.add<int32_t>(alliance->getId());
	packet.addString(alliance->getTitle(0));
	packet.addString(alliance->getTitle(1));
	packet.addString(alliance->getTitle(2));
	packet.addString(alliance->getTitle(3));
	packet.addString(alliance->getTitle(4));

	sendToAlliance(packet, alliance);
}

void AlliancePacket::addAllianceInfo(PacketCreator &packet, Alliance *alliance) {
	unordered_map<int32_t, Guild *> guilds = alliance->getGuilds();

	packet.add<int32_t>(alliance->getId());
	packet.addString(alliance->getName());
	
	for (int8_t i = 0; i < 5; i++)
		packet.addString(alliance->getTitle(i));

	packet.add<int8_t>(alliance->getSize());

	for (unordered_map<int32_t, Guild *>::iterator iter = guilds.begin(); iter != guilds.end(); iter++)
		packet.add<int32_t>(iter->second->getId());

	packet.add<int32_t>(alliance->getCapacity());
	packet.addString(alliance->getNotice());
}

void AlliancePacket::addGuildsInfo(PacketCreator &packet, Alliance *alliance, bool addSize) {
	unordered_map<int32_t, Guild *> guilds = alliance->getGuilds();
	unordered_map<int32_t, Guild *>::iterator iter;

	if (addSize)
		packet.add<int32_t>(alliance->getSize());
	
	for (iter = guilds.begin(); iter != guilds.end(); iter++) {
		GuildPacket::addGuildInformation(packet, iter->second);
	}
}

void AlliancePacket::sendToAlliance(PacketCreator &packet, Alliance *alliance, Player *skipped) {
	unordered_map<int32_t, Guild *> guilds = alliance->getGuilds();

	PacketCreator pack;
	pack.add<int16_t>(INTER_FORWARD_TO);
	pack.add<int32_t>(-1); // Set the playerid to -1, this will be changed later in the packet
	pack.addBuffer(packet);

	unordered_map<int32_t, Guild *>::iterator iter;
	unordered_map<int32_t, Player *>::iterator iter2;

	for (iter = guilds.begin(); iter != guilds.end(); iter++) {
		for (iter2 = iter->second->m_players.begin(); iter2 != iter->second->m_players.end(); iter2++) {
			if (!iter2->second->online || iter2->second == skipped) 
				continue;

			pack.set<int32_t>(iter2->second->id, 2);
			
			if (Channel *channel = Channels::Instance()->getChannel(iter2->second->channel)) 
				channel->player->getSession()->send(pack);
		}
	}
}
