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
#include "WorldServerAcceptPlayerPacket.h"
#include "WorldServerAcceptPlayer.h"
#include "PacketCreator.h"
#include "Channels.h"

void WorldServerAcceptPlayerPacket::connect(WorldServerAcceptPlayer *player, int channel, short port) {
	Packet packet = Packet();
	packet.addHeader(INTER_CHANNEL_CONNECT);
	packet.addInt(channel);
	packet.addShort(port);
	packet.packetSend(player);
}

void WorldServerAcceptPlayerPacket::playerChangeChannel(WorldServerAcceptPlayer *player, int playerid, char *ip, short port) {
	Packet packet = Packet();
	packet.addHeader(INTER_PLAYER_CHANGE_CHANNEL);
	packet.addInt(playerid);
	packet.addShort(strlen(ip));
	packet.addString(ip, strlen(ip));
	packet.addShort(port);
	packet.packetSend(player);
}

void WorldServerAcceptPlayerPacket::sendToChannels(unsigned char *data, int len) {
	Packet packet = Packet();
	packet.addBytesHex(data, len);
	Channels::Instance()->sendToAll(packet);
}

void WorldServerAcceptPlayerPacket::findPlayer(WorldServerAcceptPlayer *player, int finder, int channel, char *findee, unsigned char is) {
	Packet packet = Packet();
	packet.addHeader(INTER_FIND);
	packet.addInt(finder);
	packet.addInt(channel);
	packet.addShort(strlen(findee));
	packet.addString(findee, strlen(findee));
	packet.addByte(is);

	packet.packetSend(player);
}

void WorldServerAcceptPlayerPacket::whisperPlayer(WorldServerAcceptPlayer *player, int whisperee, char *whisperer, int channel, char *message) {
	Packet packet = Packet();
	packet.addHeader(INTER_WHISPER);
	packet.addInt(whisperee);
	packet.addShort(strlen(whisperer));
	packet.addString(whisperer, strlen(whisperer));
	packet.addInt(channel);
	packet.addShort(strlen(message));
	packet.addString(message, strlen(message));

	packet.packetSend(player);
}

void WorldServerAcceptPlayerPacket::scrollingHeader(char *message) {
	Packet packet = Packet();
	packet.addHeader(INTER_SCROLLING_HEADER);
	packet.addShort(strlen(message));
	packet.addString(message, strlen(message));
	
	Channels::Instance()->sendToAll(packet);
}

void WorldServerAcceptPlayerPacket::newConnectable(int channel, int playerid) {
	Packet packet = Packet();
	packet.addHeader(INTER_NEW_CONNECTABLE);
	packet.addInt(playerid);

	packet.packetSend(Channels::Instance()->getChannel(channel)->player);
}
