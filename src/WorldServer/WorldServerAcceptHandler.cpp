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
#include "WorldServerAcceptHandler.h"
#include "Channel.h"
#include "Channels.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "InterHelper.h"
#include "IpUtilities.h"
#include "PacketReader.h"
#include "PartyHandler.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"
#include "WorldServerAcceptPacket.h"

void WorldServerAcceptHandler::groupChat(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int8_t type = packet.get<int8_t>(); // Buddy = 0, party = 1, guild = 2, alliance = 3
	string message = packet.getString();
	uint8_t receivers = packet.get<int8_t>();
	string sender = PlayerDataProvider::Instance()->getPlayer(playerid)->getName();
	for (size_t i = 0; i < receivers; i++) {
		int32_t receiver = packet.get<int32_t>();
		uint16_t channel = PlayerDataProvider::Instance()->getPlayer(receiver)->getChannel();
		WorldServerAcceptPacket::groupChat(channel, receiver, type, message, sender);
	}	
}

void WorldServerAcceptHandler::partyOperation(WorldServerAcceptConnection *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t playerid = packet.get<int32_t>();
	switch (type) {
		case 0x01: PartyHandler::createParty(player, playerid); break;
		case 0x02: PartyHandler::leaveParty(player, playerid); break;
		case 0x03: PartyHandler::joinParty(player, playerid, packet.get<int32_t>()); break;
		case 0x04: PartyHandler::invitePlayer(player, playerid, packet.getString()); break;
		case 0x05: PartyHandler::expelPlayer(player, playerid, packet.get<int32_t>()); break;
		case 0x06: PartyHandler::giveLeader(player, playerid, packet.get<int32_t>(), 0); break;
	}
}

void WorldServerAcceptHandler::playerChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Channel *chan = Channels::Instance()->getChannel(packet.get<int16_t>());
	if (chan) {
		WorldServerAcceptPacket::sendPacketToChannelForHolding(chan->getId(), playerid, packet);
		PlayerDataProvider::Instance()->addPendingPlayer(playerid, chan->getId());
	}
	else { // Channel doesn't exist (offline)
		WorldServerAcceptPacket::playerChangeChannel(player, playerid, 0, -1);
	}
}

void WorldServerAcceptHandler::handleChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Player *gamePlayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (gamePlayer) {
		uint16_t chanid = PlayerDataProvider::Instance()->getPendingPlayerChannel(playerid);
		Channel *chan = Channels::Instance()->getChannel(chanid);
		Channel *curchan = Channels::Instance()->getChannel(gamePlayer->getChannel());
		if (chan) {
			WorldServerAcceptPacket::newConnectable(chan->getId(), playerid);
			uint32_t chanIp = IpUtilities::matchIpSubnet(gamePlayer->getIp(), chan->getExternalIps(), chan->getIp());
			WorldServerAcceptPacket::playerChangeChannel(curchan->getConnection(), playerid, chanIp, chan->getPort());
		}
		else {
			WorldServerAcceptPacket::playerChangeChannel(curchan->getConnection(), playerid, 0, -1);
		}
		PlayerDataProvider::Instance()->removePendingPlayer(playerid);
	}
}

void WorldServerAcceptHandler::findPlayer(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t finder = packet.get<int32_t>();
	string findee_name = packet.getString();

	Player *findee = PlayerDataProvider::Instance()->getPlayer(findee_name);

	WorldServerAcceptPacket::findPlayer(player, finder, findee->getChannel(), (findee->isOnline() ? findee->getName() : findee_name));
}

void WorldServerAcceptHandler::whisperPlayer(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t whisperer = packet.get<int32_t>();
	string whisperee_name = packet.getString();
	string message = packet.getString();

	Player *whisperee = PlayerDataProvider::Instance()->getPlayer(whisperee_name);
	if (whisperee->isOnline()) {
		WorldServerAcceptPacket::findPlayer(player, whisperer, -1, whisperee->getName(), 1);
		WorldServerAcceptPacket::whisperPlayer(whisperee->getChannel(), whisperee->getId(), PlayerDataProvider::Instance()->getPlayer(whisperer)->getName(), player->getChannel(),  message);
	}
	else {
		WorldServerAcceptPacket::findPlayer(player, whisperer, whisperee->getChannel(), whisperee_name);
	}
}

void WorldServerAcceptHandler::playerConnect(WorldServerAcceptConnection *player, PacketReader &packet) {
	uint32_t ip = packet.get<uint32_t>();
	int32_t id = packet.get<int32_t>();
	string name = packet.getString();
	int32_t map = packet.get<int32_t>();
	int16_t job = static_cast<int16_t>(packet.get<int32_t>());
	uint8_t level = static_cast<uint8_t>(packet.get<int32_t>());
	int32_t guildid = packet.get<int32_t>();
	uint8_t guildrank = packet.get<uint8_t>();
	int32_t allianceid = packet.get<int32_t>();
	uint8_t alliancerank = packet.get<uint8_t>();
	
	Player *p = PlayerDataProvider::Instance()->getPlayer(id, true);
	if (p == 0) {
		p = new Player(id);
	}
	p->setIp(ip);
	p->setName(name);
	p->setMap(map);
	p->setJob(job);
	p->setLevel(level);
	p->setGuild(PlayerDataProvider::Instance()->getGuild(guildid));
	p->setGuildRank(guildrank);
	p->setAlliance(PlayerDataProvider::Instance()->getAlliance(allianceid));
	p->setAllianceRank(alliancerank);
	p->setChannel(player->getChannel());
	p->setOnline(true);
	PlayerDataProvider::Instance()->registerPlayer(p);
}

void WorldServerAcceptHandler::playerDisconnect(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	PlayerDataProvider::Instance()->remove(id, player->getChannel());
	int16_t channel = PlayerDataProvider::Instance()->removePendingPlayerEarly(id);
	if (channel != -1) {
		WorldServerAcceptPacket::sendHeldPacketRemoval(channel, id);
	}
}

void WorldServerAcceptHandler::scrollingHeader(WorldServerAcceptConnection *player, PacketReader &packet) {
	string message = packet.getString();
	WorldServer::Instance()->setScrollingHeader(message);
}

void WorldServerAcceptHandler::updateJob(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int16_t job = packet.get<int16_t>();
	Player *plyr = PlayerDataProvider::Instance()->getPlayer(id);
	plyr->setJob(job);
	if (plyr->getParty() != 0) {
		PartyHandler::silentUpdate(id);
	}
	if (plyr->getGuild() != 0) {
		Guild *guild = plyr->getGuild();
		GuildPacket::sendPlayerStatUpdate(guild, plyr, false);
	}
}

void WorldServerAcceptHandler::updateLevel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	uint8_t level = packet.get<uint8_t>();
	Player *plyr = PlayerDataProvider::Instance()->getPlayer(id);
	plyr->setLevel(level);
	if (plyr->getParty() != 0) {
		PartyHandler::silentUpdate(id);
	}
	if (plyr->getGuild() != 0) {
		Guild *guild = plyr->getGuild();
		GuildPacket::sendPlayerStatUpdate(guild, plyr, true);
	}
}

void WorldServerAcceptHandler::updateMap(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int32_t map = packet.get<int32_t>();
	if (Player *p = PlayerDataProvider::Instance()->getPlayer(id)) {
		p->setMap(map);
		if (p->getParty() != 0) {
			PartyHandler::silentUpdate(id);
		}
	}
}

void WorldServerAcceptHandler::sendToChannels(PacketReader &packet) {
	PacketCreator pack;
	pack.addBuffer(packet);
	Channels::Instance()->sendToAll(pack);
}

void WorldServerAcceptHandler::sendToLogin(PacketReader &packet) {
	PacketCreator pack;
	pack.addBuffer(packet);
	WorldServer::Instance()->getLoginConnection()->getSession()->send(pack);
}