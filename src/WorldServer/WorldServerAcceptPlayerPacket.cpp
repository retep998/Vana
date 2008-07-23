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
#include "WorldServer.h"
#include "PacketCreator.h"
#include "Channels.h"
#include "Rates.h"

void WorldServerAcceptPlayerPacket::groupChat(WorldServerAcceptPlayer *player, int playerid, char type, const string &message, const string &sender) {
	PacketCreator packet;
	packet.addHeader(INTER_FORWARD_TO);
	packet.addInt(playerid);
	packet.addShort(SEND_GROUP_CHAT);
	packet.addByte(type);
	packet.addString(sender);
	packet.addString(message);
	packet.send(player);
}

void WorldServerAcceptPlayerPacket::connect(WorldServerAcceptPlayer *player, int channel, short port) {
	PacketCreator packet;
	packet.addHeader(INTER_CHANNEL_CONNECT);
	packet.addInt(channel);
	packet.addShort(port);
	packet.send(player);
}

void WorldServerAcceptPlayerPacket::playerChangeChannel(WorldServerAcceptPlayer *player, int playerid, const string &ip, short port) {
	PacketCreator packet;
	packet.addHeader(INTER_PLAYER_CHANGE_CHANNEL);
	packet.addInt(playerid);
	packet.addString(ip);
	packet.addShort(port);
	packet.send(player);
}

void WorldServerAcceptPlayerPacket::sendToChannels(unsigned char *data, int len) {
	PacketCreator packet;
	packet.addBuffer(data, len);
	Channels::Instance()->sendToAll(packet);
}

void WorldServerAcceptPlayerPacket::findPlayer(WorldServerAcceptPlayer *player, int finder, int channel, const string &findee, unsigned char is) {
	PacketCreator packet;
	packet.addHeader(INTER_FIND);
	packet.addInt(finder);
	packet.addInt(channel);
	packet.addString(findee);
	packet.addByte(is);

	packet.send(player);
}

void WorldServerAcceptPlayerPacket::whisperPlayer(WorldServerAcceptPlayer *player, int whisperee, const string &whisperer, int channel, const string &message) {
	PacketCreator packet;
	packet.addHeader(INTER_WHISPER);
	packet.addInt(whisperee);
	packet.addString(whisperer);
	packet.addInt(channel);
	packet.addString(message);

	packet.send(player);
}

void WorldServerAcceptPlayerPacket::scrollingHeader(const string &message) {
	PacketCreator packet;
	packet.addHeader(INTER_SCROLLING_HEADER);
	packet.addString(message);
	
	Channels::Instance()->sendToAll(packet);
}

void WorldServerAcceptPlayerPacket::newConnectable(int channel, int playerid) {
	PacketCreator packet;
	packet.addHeader(INTER_NEW_CONNECTABLE);
	packet.addInt(playerid);

	packet.send(Channels::Instance()->getChannel(channel)->player);
}

void WorldServerAcceptPlayerPacket::sendRates(WorldServerAcceptPlayer *player, int setBit) {
	PacketCreator packet;
	packet.addHeader(INTER_SET_RATES);
	packet.addInt(setBit);

	if (setBit & Rates::SetBits::exp) {
		packet.addInt(WorldServer::Instance()->getExprate());
	}
	if (setBit & Rates::SetBits::questExp) {
		packet.addInt(WorldServer::Instance()->getQuestExprate());
	}
	if (setBit & Rates::SetBits::meso) {
		packet.addInt(WorldServer::Instance()->getMesorate());
	}
	if (setBit & Rates::SetBits::drop) {
		packet.addInt(WorldServer::Instance()->getDroprate());
	}

	packet.send(player);
}
