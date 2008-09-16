/*
Copyright (C) 2008 Vana Development Team

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
#include "WorldServerAcceptPlayerPacket.h"
#include "WorldServerAcceptPlayer.h"
#include "WorldServer.h"
#include "Channels.h"
#include "Players.h"
#include "ReadPacket.h"
#include "PartyHandler.h"

void WorldServerAcceptHandler::groupChat(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t playerid = packet->getInt();
	int8_t type = packet->getByte(); // Buddy = 0 party = 1 guild = 2
	string message = packet->getString();
	uint8_t receivers = packet->getByte();
	string sender = Players::Instance()->getPlayer(playerid)->name;
	for (size_t i = 0; i < receivers; i++) {
		int32_t receiver = packet->getInt();
		WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(Players::Instance()->getPlayer(receiver)->channel)->player;
		WorldServerAcceptPlayerPacket::groupChat(channel, receiver, type, message, sender);
	}	
}

void WorldServerAcceptHandler::partyOperation(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int8_t type = packet->getByte();
	int32_t playerid = packet->getInt();
	switch(type) {
		case 0x01: PartyHandler::createParty(player, playerid); break;
		case 0x02: PartyHandler::leaveParty(player, playerid); break;
		case 0x03: PartyHandler::joinParty(player, playerid, packet->getInt()); break;
		case 0x04: PartyHandler::invitePlayer(player, playerid, packet->getString()); break;
		case 0x05: PartyHandler::expelPlayer(player, playerid, packet->getInt()); break;
		case 0x06: PartyHandler::giveLeader(player, playerid, packet->getInt(), 0); break;
	}
}

void WorldServerAcceptHandler::playerChangeChannel(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t playerid = packet->getInt();
	Channel *chan = Channels::Instance()->getChannel(packet->getShort());

	if (chan) {
		WorldServerAcceptPlayerPacket::newConnectable(chan->id, playerid);
		WorldServerAcceptPlayerPacket::playerChangeChannel(player, playerid, chan->ip, chan->port);
	}
	else { // Channel doesn't exist (offline)
		WorldServerAcceptPlayerPacket::playerChangeChannel(player, playerid, "255.255.255.255", -1);
	}
}

void WorldServerAcceptHandler::findPlayer(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t finder = packet->getInt();
	string findee_name = packet->getString();

	Player *findee = Players::Instance()->getPlayerFromName(findee_name);
	if (findee->channel == 65535)
		WorldServerAcceptPlayerPacket::findPlayer(player, finder, findee->channel, findee->name);
	else
		WorldServerAcceptPlayerPacket::findPlayer(player, finder, findee->channel, findee_name);
}

void WorldServerAcceptHandler::whisperPlayer(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t whisperer = packet->getInt();
	string whisperee_name = packet->getString();
	string message = packet->getString();

	Player *whisperee = Players::Instance()->getPlayerFromName(whisperee_name);
	if (whisperee->channel != -1) {
		WorldServerAcceptPlayerPacket::findPlayer(player, whisperer, -1, whisperee->name, 1);
		WorldServerAcceptPlayerPacket::whisperPlayer(Channels::Instance()->getChannel(whisperee->channel)->player, whisperee->id, Players::Instance()->getPlayer(whisperer)->name, player->getChannel(),  message);
	}
	else
		WorldServerAcceptPlayerPacket::findPlayer(player, whisperer, whisperee->channel, whisperee_name);
}

void WorldServerAcceptHandler::registerPlayer(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t id = packet->getInt();
	string name = packet->getString();
	int32_t map = packet->getInt();
	int32_t job = packet->getInt();
	int32_t level = packet->getInt();
	Players::Instance()->registerPlayer(id, name, player->getChannel(), map, job, level);
}

void WorldServerAcceptHandler::removePlayer(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t id = packet->getInt();
	Players::Instance()->remove(id, player->getChannel());
}

void WorldServerAcceptHandler::scrollingHeader(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	string message = packet->getString();
	WorldServer::Instance()->setScrollingHeader(message);
}

void WorldServerAcceptHandler::updateJob(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t id = packet->getInt();
	int32_t job = packet->getInt();
	Players::Instance()->getPlayer(id)->job = job;
	if (Players::Instance()->getPlayer(id)->party != 0) {
		PartyHandler::silentUpdate(id);
	}
}

void WorldServerAcceptHandler::updateLevel(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t id = packet->getInt();
	int32_t level = packet->getInt();
	Players::Instance()->getPlayer(id)->level = level;
	if (Players::Instance()->getPlayer(id)->party != 0) {
		PartyHandler::silentUpdate(id);
	}
}

void WorldServerAcceptHandler::updateMap(WorldServerAcceptPlayer *player, ReadPacket *packet) {
	int32_t id = packet->getInt();
	int32_t map = packet->getInt();
	Players::Instance()->getPlayer(id)->map = map;
	if (Players::Instance()->getPlayer(id)->party != 0) {
		PartyHandler::silentUpdate(id);
	}
}
