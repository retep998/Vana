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
#include "WorldServerAcceptPacket.h"
#include "Channels.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Rates.h"
#include "SendHeader.h"
#include "WorldServer.h"
#include "WorldServerAcceptPlayer.h"

void WorldServerAcceptPacket::groupChat(WorldServerAcceptPlayer *player, int32_t playerid, int8_t type, const string &message, const string &sender) {
	PacketCreator packet;
	packet.addShort(INTER_FORWARD_TO);
	packet.addInt(playerid);
	packet.addShort(SEND_GROUP_CHAT);
	packet.addByte(type);
	packet.addString(sender);
	packet.addString(message);
	player->getSession()->send(packet);
}

void WorldServerAcceptPacket::connect(WorldServerAcceptPlayer *player, uint16_t channel, uint16_t port, uint8_t maxMultiLevel, int16_t maxStats) {
	PacketCreator packet;
	packet.addShort(INTER_CHANNEL_CONNECT);
	packet.addShort(channel);
	packet.addShort(port);
	packet.addByte(maxMultiLevel);
	packet.addShort(maxStats);
	packet.addClock(clock());
	player->getSession()->send(packet);
}

void WorldServerAcceptPacket::playerChangeChannel(WorldServerAcceptPlayer *player, int32_t playerid, const string &ip, int16_t port) {
	PacketCreator packet;
	packet.addShort(INTER_PLAYER_CHANGE_CHANNEL);
	packet.addInt(playerid);
	packet.addString(ip);
	packet.addShort(port);
	player->getSession()->send(packet);
}

void WorldServerAcceptPacket::sendToChannels(unsigned char *data, int32_t len) {
	PacketCreator packet;
	packet.addBuffer(data, len);
	Channels::Instance()->sendToAll(packet);
}

void WorldServerAcceptPacket::findPlayer(WorldServerAcceptPlayer *player, int32_t finder, uint16_t channel, const string &findee, uint8_t is) {
	PacketCreator packet;
	packet.addShort(INTER_FIND);
	packet.addInt(finder);
	packet.addShort(channel);
	packet.addString(findee);
	packet.addByte(is);

	player->getSession()->send(packet);
}

void WorldServerAcceptPacket::whisperPlayer(WorldServerAcceptPlayer *player, int32_t whisperee, const string &whisperer, uint16_t channel, const string &message) {
	PacketCreator packet;
	packet.addShort(INTER_WHISPER);
	packet.addInt(whisperee);
	packet.addString(whisperer);
	packet.addShort(channel);
	packet.addString(message);

	player->getSession()->send(packet);
}

void WorldServerAcceptPacket::scrollingHeader(const string &message) {
	PacketCreator packet;
	packet.addShort(INTER_SCROLLING_HEADER);
	packet.addString(message);
	
	Channels::Instance()->sendToAll(packet);
}

void WorldServerAcceptPacket::newConnectable(uint16_t channel, int32_t playerid) {
	PacketCreator packet;
	packet.addShort(INTER_NEW_CONNECTABLE);
	packet.addInt(playerid);

	Channels::Instance()->getChannel(channel)->player->getSession()->send(packet);
}

void WorldServerAcceptPacket::sendRates(WorldServerAcceptPlayer *player, int32_t setBit) {
	PacketCreator packet;
	packet.addShort(INTER_SET_RATES);
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

	player->getSession()->send(packet);
}
