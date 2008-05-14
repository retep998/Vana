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
#include "BufferUtilities.h"
#include "Channels.h"
#include "Players.h"

void WorldServerAcceptHandler::playerChangeChannel(WorldServerAcceptPlayer *player, unsigned char *packet) {
	Channel *chan = Channels::Instance()->getChannel(BufferUtilities::getInt(packet+4));
	int playerid = BufferUtilities::getInt(packet);
	WorldServerAcceptPlayerPacket::newConnectable(chan->id, playerid);
	WorldServerAcceptPlayerPacket::playerChangeChannel(player, playerid, chan->ip, chan->port);
}

void WorldServerAcceptHandler::findPlayer(WorldServerAcceptPlayer *player, unsigned char *packet) {
	int finder = BufferUtilities::getInt(packet);
	short findee_namelen = BufferUtilities::getShort(packet+4);
	char findee_name[15];
	BufferUtilities::getString(packet+6, findee_namelen, findee_name);

	Player *findee = Players::Instance()->getPlayerFromName(findee_name);
	if (findee->channel != -1)
		WorldServerAcceptPlayerPacket::findPlayer(player, finder, findee->channel, findee->name);
	else
		WorldServerAcceptPlayerPacket::findPlayer(player, finder, findee->channel, findee_name);
}

void WorldServerAcceptHandler::whisperPlayer(WorldServerAcceptPlayer *player, unsigned char *packet) {
	int whisperer = BufferUtilities::getInt(packet);
	short whisperee_namelen = BufferUtilities::getShort(packet+4);
	char whisperee_name[15];
	BufferUtilities::getString(packet+6, whisperee_namelen, whisperee_name);
	short messagelen = BufferUtilities::getShort(packet+6+whisperee_namelen);
	char message[91];
	BufferUtilities::getString(packet+8+whisperee_namelen, messagelen, message);

	Player *whisperee = Players::Instance()->getPlayerFromName(whisperee_name);
	if (whisperee->channel != -1) {
		WorldServerAcceptPlayerPacket::findPlayer(player, whisperer, -1, whisperee->name, 1);
		WorldServerAcceptPlayerPacket::whisperPlayer(Channels::Instance()->getChannel(whisperee->channel)->player, whisperee->id, Players::Instance()->getPlayer(whisperer)->name, player->getChannel(),  message);
	}
	else
		WorldServerAcceptPlayerPacket::findPlayer(player, whisperer, whisperee->channel, whisperee_name);
}

void WorldServerAcceptHandler::registerPlayer(WorldServerAcceptPlayer *player, unsigned char *packet) {
	int id = BufferUtilities::getInt(packet);
	short namelen = BufferUtilities::getShort(packet+4);
	char name[15];
	BufferUtilities::getString(packet+6, namelen, name);
	Players::Instance()->registerPlayer(id, name, player->getChannel());
}

void WorldServerAcceptHandler::removePlayer(WorldServerAcceptPlayer *player, unsigned char *packet) {
	int id = BufferUtilities::getShort(packet);
	Players::Instance()->remove(id, player->getChannel());
}

void WorldServerAcceptHandler::scrollingHeader(WorldServerAcceptPlayer *player, unsigned char *packet) {
	short messagelen = BufferUtilities::getShort(packet);
	char message[100];
	BufferUtilities::getString(packet+2, messagelen, message);

	WorldServer::Instance()->setScrollingHeader(message);
}
