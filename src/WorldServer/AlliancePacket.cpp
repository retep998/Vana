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
#include "Alliance.h"
#include "Channel.h"
#include "Channels.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"
#include "TimeUtilities.h"
#include "WorldServerAcceptConnection.h"

void AlliancePacket::sendAllianceInfo(Alliance *alliance, Player *requestee) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(requestee->getId());

	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x0c);

	if (alliance == nullptr) {
		packet.addBool(false);

		Channels::Instance()->sendToChannel(requestee->getChannel(), packet);
		return; // Don't send any more packets
	}

	packet.addBool(true);

	addAllianceInfo(packet, alliance);

	Channels::Instance()->sendToChannel(requestee->getChannel(), packet);

	packet = PacketCreator();
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(requestee->getId());

	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x0D);

	addGuildsInfo(packet, alliance);

	Channels::Instance()->sendToChannel(requestee->getChannel(), packet);
}

void AlliancePacket::sendInvite(Alliance *alliance, Player *inviter, Player *invitee) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(invitee->getId());

	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x03);

	packet.add<int32_t>(inviter->getGuild()->getId());
	packet.addString(inviter->getName());
	packet.addString(alliance->getName());

	Channels::Instance()->sendToChannel(invitee->getChannel(), packet);
}

void AlliancePacket::sendInviteAccepted(Alliance *alliance, Guild *guild) {
	PacketCreator packet;
	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x12);

	addAllianceInfo(packet, alliance);

	packet.add<int32_t>(guild->getId());
	GuildPacket::addGuildInformation(packet, guild);

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendInviteDenied(Alliance *alliance, Guild *guild) {
	Player *leader = PlayerDataProvider::Instance()->getPlayer(alliance->getLeaderId());

	// GMS doesnt have an actual deny packet. They just use the note packet...
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(leader->getId());

	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(0x09);

	packet.addString(guild->getName() + " Guild has rejected the Guild Union invitation.");

	Channels::Instance()->sendToChannel(leader->getChannel(), packet);
}

void AlliancePacket::sendGuildLeft(Alliance *alliance, Guild *guild, bool expelled) {
	PacketCreator packet;
	packet.addHeader(SMSG_ALLIANCE);
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
	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x0f);

	addAllianceInfo(packet, alliance);
	addGuildsInfo(packet, alliance, false);

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendDeleteAlliance(Alliance *alliance) {
	PacketCreator packet;
	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x1d);

	packet.add<int32_t>(alliance->getId());

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendUpdateLeader(Alliance *alliance, Player *oldLeader) {
	PacketCreator packet;
	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x19);

	packet.add<int32_t>(alliance->getId());
	packet.add<int32_t>(oldLeader->getId()); // From
	packet.add<int32_t>(alliance->getLeaderId()); // To

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendUpdatePlayer(Alliance *alliance, Player *player, uint8_t option) {
	PacketCreator packet;
	packet.addHeader(SMSG_ALLIANCE);

	if (option == 0) {
		packet.add<int8_t>(0x1b);

		packet.add<int32_t>(player->getId());
		packet.add<uint8_t>(player->getAllianceRank());
	}
	else if (option == 1) {
		packet.add<int8_t>(0x18);

		packet.add<int32_t>(alliance->getId());
		packet.add<int32_t>(player->getGuild()->getId());
		packet.add<int32_t>(player->getId());
		packet.add<int32_t>(player->getLevel());
		packet.add<int32_t>(player->getJob());
	}
	else {
		packet.add<int8_t>(0x0e);

		packet.add<int32_t>(alliance->getId());
		packet.add<int32_t>(player->getGuild()->getId());
		packet.add<int32_t>(player->getId());
		packet.add<uint8_t>(player->isOnline());
	}

	sendToAlliance(packet, alliance, (option == 2 ? player : 0));
}

void AlliancePacket::sendUpdateNotice(Alliance *alliance) {
	PacketCreator packet;
	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x1c);

	packet.add<int32_t>(alliance->getId());
	packet.addString(alliance->getNotice());

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendUpdateCapacity(Alliance *alliance) {
	PacketCreator packet;
	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x17);

	addAllianceInfo(packet, alliance);

	sendToAlliance(packet, alliance);
}

void AlliancePacket::sendUpdateTitles(Alliance *alliance) {
	PacketCreator packet;
	packet.addHeader(SMSG_ALLIANCE);
	packet.add<int8_t>(0x1a);

	packet.add<int32_t>(alliance->getId());
	for (uint8_t i = 1; i <= GuildsAndAlliances::RankQuantity; i++)
		packet.addString(alliance->getTitle(i));

	sendToAlliance(packet, alliance);
}

void AlliancePacket::addAllianceInfo(PacketCreator &packet, Alliance *alliance) {
	unordered_map<int32_t, Guild *> guilds = alliance->getGuilds();

	packet.add<int32_t>(alliance->getId());
	packet.addString(alliance->getName());

	for (uint8_t i = 1; i <= GuildsAndAlliances::RankQuantity; i++)
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
	pack.add<int16_t>(IMSG_FORWARD_TO);
	pack.add<int32_t>(-1); // Set the playerid to -1, this will be changed later in the packet
	pack.addBuffer(packet);

	unordered_map<int32_t, Guild *>::iterator iter;
	unordered_map<int32_t, Player *>::iterator iter2;

	for (iter = guilds.begin(); iter != guilds.end(); iter++) {
		for (iter2 = iter->second->m_players.begin(); iter2 != iter->second->m_players.end(); iter2++) {
			if (!iter2->second->isOnline() || iter2->second == skipped)
				continue;

			pack.set<int32_t>(iter2->second->getId(), 2);

			if (Channel *channel = Channels::Instance()->getChannel(iter2->second->getChannel())) {
				channel->getConnection()->getSession()->send(pack);
			}
		}
	}
}
