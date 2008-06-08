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

void WorldServerConnectPlayerPacket::registerPlayer(WorldServerConnectPlayer *player, int playerid, const string &name) {
	Packet packet;
	packet.addHeader(INTER_REGISTER_PLAYER);
	packet.addInt(playerid);
	packet.addString(name);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::removePlayer(WorldServerConnectPlayer *player, int playerid) {
	Packet packet;
	packet.addHeader(INTER_REMOVE_PLAYER);
	packet.addInt(playerid);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::findPlayer(WorldServerConnectPlayer *player, int playerid, const string &findee_name) {
	Packet packet;
	packet.addHeader(INTER_FIND);
	packet.addInt(playerid);
	packet.addString(findee_name);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::whisperPlayer(WorldServerConnectPlayer *player, int playerid, const string &whisperee, const string &message) {
	Packet packet;
	packet.addHeader(INTER_WHISPER);
	packet.addInt(playerid);
	packet.addString(whisperee);
	packet.addString(message);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::scrollingHeader(WorldServerConnectPlayer *player, const string &message) {
	Packet packet;
	packet.addHeader(INTER_SCROLLING_HEADER);
	packet.addString(message);
	packet.send(player);
}
