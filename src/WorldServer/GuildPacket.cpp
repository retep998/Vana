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
#include "Alliance.h"
#include "Channel.h"
#include "Channels.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameObjects.h"
#include "Guild.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"
#include "SyncHandler.h"
#include "TimeUtilities.h"
#include "WorldServerAcceptConnection.h"

void GuildPacket::sendGuildInfo(Guild *guild, Player *requestee, bool isNew) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(requestee->getId());

	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(isNew ? 0x20 : 0x1a);

	if (!isNew) {
		if (guild == nullptr) {
			packet.addBool(false);

			Channels::Instance()->sendToChannel(requestee->getChannel(), packet);
			return;
		}
		packet.addBool(true);
	}

	addGuildInformation(packet, guild);

	Channels::Instance()->sendToChannel(requestee->getChannel(), packet);
}

void GuildPacket::sendInvite(Player *inviter, Player *invitee) {
	PacketCreator pack;
	pack.add<int16_t>(IMSG_FORWARD_TO);
	pack.add<int32_t>(invitee->getId());

	pack.addHeader(SMSG_GUILD);
	pack.add<int8_t>(0x05);
	pack.add<int32_t>(inviter->getGuild()->getId());
	pack.addString(inviter->getName());

	Channels::Instance()->sendToChannel(invitee->getChannel(), pack);
}

void GuildPacket::sendCapacityUpdate(Guild *guild) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x3a);

	packet.add<int32_t>(guild->getId());
	packet.add<int32_t>(guild->getCapacity());

	sendToGuild(packet, guild);
}

void GuildPacket::sendEmblemUpdate(Guild *guild) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x42);

	GuildLogo logo = guild->getLogo();
	packet.add<int32_t>(guild->getId());
	packet.add<int16_t>(logo.background);
	packet.add<uint8_t>(logo.backgroundColor);
	packet.add<int16_t>(logo.logo);
	packet.add<uint8_t>(logo.color);

	sendToGuild(packet, guild);
}

void GuildPacket::sendGuildPointsUpdate(Guild *guild) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x48);

	packet.add<int32_t>(guild->getId());
	packet.add<int32_t>(guild->getGuildPoints());

	sendToGuild(packet, guild);

	packet = PacketCreator();
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(0x06);

	packet.add<int32_t>(guild->getGuildPoints());

	sendToGuild(packet, guild);
}

void GuildPacket::sendTitlesUpdate(Guild *guild) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x3e);

	packet.add<int32_t>(guild->getId());
	for (uint8_t i = 1; i <= GuildsAndAlliances::RankQuantity; i++)
		packet.addString(guild->getTitle(i));

	sendToGuild(packet, guild);
}

void GuildPacket::sendNoticeUpdate(Guild *guild) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x44);

	packet.add<int32_t>(guild->getId());
	packet.addString(guild->getNotice());

	sendToGuild(packet, guild);
}

void GuildPacket::sendRankUpdate(Guild *guild, Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x40);

	packet.add<int32_t>(guild->getId());
	packet.add<int32_t>(player->getId());
	packet.add<uint8_t>(player->getGuildRank());

	sendToGuild(packet, guild);
}

void GuildPacket::sendPlayerStatUpdate(Guild *guild, Player *player, bool levelMessage, bool isFromLogon) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x3c);

	packet.add<int32_t>(guild->getId());
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getLevel());
	packet.add<int32_t>(player->getJob());

	sendToGuild(packet, guild, (isFromLogon ? player : 0));

	if (levelMessage) {
		packet = PacketCreator();
		packet.addHeader(SMSG_PLAYER_LEVEL_UPDATE);
		packet.add<int8_t>(0x02); // 1 = family, 2 = guild
		packet.add<int32_t>(player->getLevel());
		packet.addString(player->getName());

		sendToGuild(packet, guild, player);
	}
}

void GuildPacket::sendPlayerUpdate(Guild *guild, Player *player, uint8_t type, bool toSelf) {
	/* Types are:
		0 = New player
		1 = Leave player
		2 = Expel player
		3 = Log in/log out
	*/
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);

	if (type == 0) {
		packet.add<int8_t>(0x27);

		packet.add<int32_t>(guild->getId());
		packet.add<int32_t>(player->getId());
		packet.addString(player->getName(), 13);
		packet.add<int32_t>(player->getJob());
		packet.add<int32_t>(player->getLevel());
		packet.add<int32_t>(player->getGuildRank());
		packet.add<int32_t>(player->isOnline() ? 1 : 0);
		packet.addBytes("91940491");
		packet.add<int32_t>(player->getAllianceRank());
	}
	else if (type == 1 || type == 2) {
		packet.add<int8_t>(type == 1 ? 0x2c : 0x2f);

		packet.add<int32_t>(guild->getId());
		packet.add<int32_t>(player->getId());
		packet.addString(player->getName());
	}
	else {
		packet.add<int8_t>(0x3d);

		packet.add<int32_t>(guild->getId());
		packet.add<int32_t>(player->getId());
		packet.add<int8_t>(player->isOnline() ? 1 : 0);
	}

	sendToGuild(packet, guild, (toSelf ? 0 : player));
}

void GuildPacket::sendGuildDisband(Guild *guild) {
	PacketCreator packet;
	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x32);

	packet.add<int32_t>(guild->getId());
	packet.add<int8_t>(1); // Maybe reason?

	sendToGuild(packet, guild);
}

void GuildPacket::sendGuildContract(Player *player, bool isLeader, int32_t partyId, const string &creator, const string &guildName) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(player->getId());

	packet.addHeader(SMSG_GUILD);

	packet.add<int8_t>(0x03);
	if (!isLeader) {
		packet.add<int32_t>(partyId);
		packet.addString(creator);
		packet.addString(guildName);
	}

	Channels::Instance()->sendToChannel(player->getChannel(), packet);
}

void GuildPacket::sendGuildDenyResult(Player *inviter, Player *invitee, uint8_t result) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(inviter->getId());
	packet.addHeader(SMSG_GUILD);

	packet.add<uint8_t>(result);
	packet.addString(invitee->getName());

	Channels::Instance()->sendToChannel(inviter->getChannel(), packet);
}

void GuildPacket::sendPlayerGuildMessage(Player *player, uint8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(player->getId());

	packet.addHeader(SMSG_GUILD);
	packet.add<uint8_t>(type);

	Channels::Instance()->sendToChannel(player->getChannel(), packet);
}

void GuildPacket::sendPlayerMessage(Player *player, uint8_t sort, const string &message) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(player->getId());

	packet.addHeader(SMSG_NOTICE);
	packet.add<uint8_t>(sort);
	packet.addString(message);

	Channels::Instance()->sendToChannel(player->getChannel(), packet);
}

void GuildPacket::sendGuildRankBoard(Player *player, int32_t npcid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(player->getId());

	packet.addHeader(SMSG_GUILD);
	packet.add<int8_t>(0x49);
	packet.add<int32_t>(npcid);

	mysqlpp::Query query = Database::getCharDB().query("SELECT name, points, logo, logobg, logocolor, logobgcolor FROM guilds ORDER BY points DESC LIMIT 50");
	mysqlpp::StoreQueryResult res = query.store();

	packet.add<int32_t>((int32_t) res.num_rows());

	for (int32_t i = 0; i < (int32_t) res.num_rows(); i++) {
		packet.addString((string) res[i]["name"]);
		packet.add<int32_t>((int32_t) res[i]["points"]);
		packet.add<int32_t>((int32_t) res[i]["logo"]);
		packet.add<int32_t>((int32_t) res[i]["logocolor"]);
		packet.add<int32_t>((int32_t) res[i]["logobg"]);
		packet.add<int32_t>((int32_t) res[i]["logobgcolor"]);
	}

	Channels::Instance()->sendToChannel(player->getChannel(), packet);
}

void GuildPacket::addGuildInformation(PacketCreator &packet, Guild *guild) {
	// Initializing the leader
	Player *leader = PlayerDataProvider::Instance()->getPlayer(guild->getLeader(), true);

	if (leader == nullptr) {
		std::cout << "Leader not found in the server. Leader ID: " << guild->getLeader() << std::endl;
	}
	packet.add<int32_t>(guild->getId());
	packet.addString(guild->getName());

	for (uint8_t i = 1; i <= GuildsAndAlliances::RankQuantity; i++) {
		packet.addString(guild->getTitle(i));
	}

	packet.add<uint8_t>(guild->m_players.size());

	if (leader != nullptr) {
		packet.add<int32_t>(leader->getId());
	}

	for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
		if (iter->second->getId() != guild->getLeader()) {
			packet.add<int32_t>(iter->second->getId());
		}
	}

	// Adding the information of the players
	if (leader != nullptr) {
		packet.addString(leader->getName(), 13);
		packet.add<int32_t>(leader->getJob());
		packet.add<int32_t>(leader->isOnline() ? leader->getLevel() : -1); // FIXME
		packet.add<int32_t>(leader->getGuildRank());
		packet.add<int32_t>(leader->isOnline() ? 1 : 0);
		packet.addBytes("91940491");
		packet.add<int32_t>(leader->getAllianceRank());
	}

	for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
		if (iter->second->getId() != guild->getLeader()) {
			packet.addString(iter->second->getName(), 13);
			packet.add<int32_t>(iter->second->getJob());
			packet.add<int32_t>(iter->second->isOnline() ? iter->second->getLevel() : -1); // FIXME
			packet.add<int32_t>(iter->second->getGuildRank());
			packet.add<int32_t>(iter->second->isOnline() ? 1 : 0);
			packet.addBytes("91940491");
			packet.add<int32_t>(iter->second->getAllianceRank());
		}
	}

	// Adding Guild info
	GuildLogo logo = guild->getLogo();
	packet.add<int32_t>(guild->getCapacity());
	packet.add<int16_t>(logo.background);
	packet.add<uint8_t>(logo.backgroundColor);
	packet.add<int16_t>(logo.logo);
	packet.add<uint8_t>(logo.color);
	packet.addString(guild->getNotice());
	packet.add<int32_t>(guild->getGuildPoints());
	packet.add<int32_t>(guild->getAlliance() == nullptr ? 0 : guild->getAlliance()->getId());
}

void GuildPacket::sendToGuild(PacketCreator &packet, Guild *guild, Player *player) {
	unordered_map<int32_t, Player *> players = guild->m_players;
	unordered_map<int32_t, Player *>::iterator iter;
	for (iter = players.begin(); iter != players.end(); iter++) {
		if ((player != nullptr && iter->second == player) || !iter->second->isOnline()) {
			continue;
		}
		PacketCreator pack;
		pack.add<int16_t>(IMSG_FORWARD_TO);
		pack.add<int32_t>(iter->second->getId());

		pack.addBuffer(packet);

		Channels::Instance()->sendToChannel(iter->second->getChannel(), pack);
	}
}
