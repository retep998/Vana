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
#include "WorldServerConnectPlayerPacket.h"
#include "WorldServerConnectPlayer.h"
#include "PacketCreator.h"
#include "InterHeader.h"

void WorldServerConnectPlayerPacket::playerChangeChannel(WorldServerConnectPlayer *player, int playerid, int channel) {
	Packet packet;
	packet.addHeader(INTER_PLAYER_CHANGE_CHANNEL);
	packet.addInt(playerid);
	packet.addInt(channel);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::registerPlayer(WorldServerConnectPlayer *player, int playerid, char *name) {
	Packet packet;
	packet.addHeader(INTER_REGISTER_PLAYER);
	packet.addInt(playerid);
	packet.addShort(strlen(name));
	packet.addString(name, strlen(name));
	packet.send(player);
}

void WorldServerConnectPlayerPacket::removePlayer(WorldServerConnectPlayer *player, int playerid) {
	Packet packet;
	packet.addHeader(INTER_REMOVE_PLAYER);
	packet.addInt(playerid);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::findPlayer(WorldServerConnectPlayer *player, int playerid, char *findee_name) {
	Packet packet;
	packet.addHeader(INTER_FIND);
	packet.addInt(playerid);
	packet.addShort(strlen(findee_name));
	packet.addString(findee_name, strlen(findee_name));
	packet.send(player);
}

void WorldServerConnectPlayerPacket::whisperPlayer(WorldServerConnectPlayer *player, int playerid, char *whisperee, char *message) {
	Packet packet;
	packet.addHeader(INTER_WHISPER);
	packet.addInt(playerid);
	packet.addShort(strlen(whisperee));
	packet.addString(whisperee, strlen(whisperee));
	packet.addShort(strlen(message));
	packet.addString(message, strlen(message));
	packet.send(player);
}

void WorldServerConnectPlayerPacket::scrollingHeader(WorldServerConnectPlayer *player, char *message) {
	Packet packet;
	packet.addHeader(INTER_SCROLLING_HEADER);
	packet.addShort(strlen(message));
	packet.addString(message, strlen(message));
	packet.send(player);
}
