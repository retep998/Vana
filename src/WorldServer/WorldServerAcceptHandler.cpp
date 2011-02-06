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
#include "ChannelChangeRequests.h"
#include "Channels.h"
#include "IpUtilities.h"
#include "PacketReader.h"
#include "PartyHandler.h"
#include "Players.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"
#include "WorldServerAcceptPacket.h"

void WorldServerAcceptHandler::groupChat(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int8_t type = packet.get<int8_t>(); // Buddy = 0 party = 1 guild = 2
	string message = packet.getString();
	uint8_t receivers = packet.get<int8_t>();
	string sender = Players::Instance()->getPlayer(playerid)->name;
	for (size_t i = 0; i < receivers; i++) {
		int32_t receiver = packet.get<int32_t>();
		WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(Players::Instance()->getPlayer(receiver)->channel)->player;
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
		WorldServerAcceptPacket::sendPacketToChannelForHolding(chan->id, playerid, packet);
		ChannelChangeRequests::Instance()->addPendingPlayer(playerid, chan->id);
	}
	else { // Channel doesn't exist (offline)
		WorldServerAcceptPacket::playerChangeChannel(player, playerid, 0, -1);
	}
}

void WorldServerAcceptHandler::handleChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Player *gamePlayer = Players::Instance()->getPlayer(playerid);
	if (gamePlayer) {
		uint16_t chanid = ChannelChangeRequests::Instance()->getPendingPlayerChannel(playerid);
		Channel *chan = Channels::Instance()->getChannel(chanid);
		Channel *curchan = Channels::Instance()->getChannel(gamePlayer->channel);
		if (chan) {
			WorldServerAcceptPacket::newConnectable(chan->id, playerid);
			uint32_t chanIp = IpUtilities::matchIpSubnet(gamePlayer->ip, chan->external_ip, chan->ip);
			WorldServerAcceptPacket::playerChangeChannel(curchan->player, playerid, chanIp, chan->port);
		}
		else {
			WorldServerAcceptPacket::playerChangeChannel(curchan->player, playerid, 0, -1);
		}
		ChannelChangeRequests::Instance()->removePendingPlayer(playerid);
	}
}

void WorldServerAcceptHandler::findPlayer(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t finder = packet.get<int32_t>();
	string findee_name = packet.getString();

	Player *findee = Players::Instance()->getPlayerFromName(findee_name);
	if (findee->channel != -1)
		WorldServerAcceptPacket::findPlayer(player, finder, findee->channel, findee->name);
	else
		WorldServerAcceptPacket::findPlayer(player, finder, findee->channel, findee_name);
}

void WorldServerAcceptHandler::whisperPlayer(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t whisperer = packet.get<int32_t>();
	string whisperee_name = packet.getString();
	string message = packet.getString();

	Player *whisperee = Players::Instance()->getPlayerFromName(whisperee_name);
	if (whisperee->channel != -1) {
		WorldServerAcceptPacket::findPlayer(player, whisperer, -1, whisperee->name, 1);
		WorldServerAcceptPacket::whisperPlayer(Channels::Instance()->getChannel(whisperee->channel)->player, whisperee->id, Players::Instance()->getPlayer(whisperer)->name, player->getChannel(),  message);
	}
	else
		WorldServerAcceptPacket::findPlayer(player, whisperer, whisperee->channel, whisperee_name);
}

void WorldServerAcceptHandler::registerPlayer(WorldServerAcceptConnection *player, PacketReader &packet) {
	uint32_t ip = packet.get<uint32_t>();
	int32_t id = packet.get<int32_t>();
	string name = packet.getString();
	int32_t map = packet.get<int32_t>();
	int32_t job = packet.get<int32_t>();
	int32_t level = packet.get<int32_t>();
	Players::Instance()->registerPlayer(ip, id, name, player->getChannel(), map, job, level);
}

void WorldServerAcceptHandler::removePlayer(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	Players::Instance()->remove(id, player->getChannel());
	ChannelChangeRequests::Instance()->removePendingPlayerEarly(id);
}

void WorldServerAcceptHandler::scrollingHeader(WorldServerAcceptConnection *player, PacketReader &packet) {
	string message = packet.getString();
	WorldServer::Instance()->setScrollingHeader(message);
}

void WorldServerAcceptHandler::updateJob(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int32_t job = packet.get<int32_t>();
	Players::Instance()->getPlayer(id)->job = job;
	if (Players::Instance()->getPlayer(id)->party != 0) {
		PartyHandler::silentUpdate(id);
	}
}

void WorldServerAcceptHandler::updateLevel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int32_t level = packet.get<int32_t>();
	Players::Instance()->getPlayer(id)->level = level;
	if (Players::Instance()->getPlayer(id)->party != 0) {
		PartyHandler::silentUpdate(id);
	}
}

void WorldServerAcceptHandler::updateMap(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int32_t map = packet.get<int32_t>();
	Players::Instance()->getPlayer(id)->map = map;
	if (Players::Instance()->getPlayer(id)->party != 0) {
		PartyHandler::silentUpdate(id);
	}
}

void WorldServerAcceptHandler::toChannels(PacketReader &packet) {
	PacketCreator pack;
	pack.addBuffer(packet);
	Channels::Instance()->sendToAll(pack);
}
