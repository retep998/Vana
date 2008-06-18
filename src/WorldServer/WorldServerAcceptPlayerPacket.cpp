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
	Packet packet;
	packet.addHeader(INTER_CHANNEL_CONNECT);
	packet.addInt(channel);
	packet.addShort(port);
	packet.send(player);
}

void WorldServerAcceptPlayerPacket::playerChangeChannel(WorldServerAcceptPlayer *player, int playerid, const string &ip, short port) {
	Packet packet;
	packet.addHeader(INTER_PLAYER_CHANGE_CHANNEL);
	packet.addInt(playerid);
	packet.addString(ip);
	packet.addShort(port);
	packet.send(player);
}

void WorldServerAcceptPlayerPacket::sendToChannels(unsigned char *data, int len) {
	Packet packet;
	packet.addBuffer(data, len);
	Channels::Instance()->sendToAll(packet);
}

void WorldServerAcceptPlayerPacket::findPlayer(WorldServerAcceptPlayer *player, int finder, int channel, const string &findee, unsigned char is) {
	Packet packet;
	packet.addHeader(INTER_FIND);
	packet.addInt(finder);
	packet.addInt(channel);
	packet.addString(findee);
	packet.addByte(is);

	packet.send(player);
}

void WorldServerAcceptPlayerPacket::whisperPlayer(WorldServerAcceptPlayer *player, int whisperee, const string &whisperer, int channel, const string &message) {
	Packet packet;
	packet.addHeader(INTER_WHISPER);
	packet.addInt(whisperee);
	packet.addString(whisperer);
	packet.addInt(channel);
	packet.addString(message);

	packet.send(player);
}

void WorldServerAcceptPlayerPacket::scrollingHeader(const string &message) {
	Packet packet;
	packet.addHeader(INTER_SCROLLING_HEADER);
	packet.addString(message);
	
	Channels::Instance()->sendToAll(packet);
}

void WorldServerAcceptPlayerPacket::newConnectable(int channel, int playerid) {
	Packet packet;
	packet.addHeader(INTER_NEW_CONNECTABLE);
	packet.addInt(playerid);

	packet.send(Channels::Instance()->getChannel(channel)->player);
}
