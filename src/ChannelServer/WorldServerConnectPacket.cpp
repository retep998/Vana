/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "WorldServerConnectPacket.h"
#include "ChannelServer.h"
#include "Configuration.h"
#include "InterHeader.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Session.h"
#include "SmsgHeader.h"
#include "WorldServerConnection.h"

void WorldServerConnectPacket::groupChat(int8_t type, int32_t playerId, const vector<int32_t> &receivers, const string &chat) {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_GROUP_CHAT);
	sendPacket.add<int32_t>(playerId);
	sendPacket.add<int8_t>(type);
	sendPacket.addString(chat);
	sendPacket.addVector(receivers);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}

void WorldServerConnectPacket::findPlayer(int32_t playerId, const string &findeeName) {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_FIND);
	sendPacket.add<int32_t>(playerId);
	sendPacket.addString(findeeName);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}

void WorldServerConnectPacket::whisperPlayer(int32_t playerId, const string &whisperee, const string &message) {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_WHISPER);
	sendPacket.add<int32_t>(playerId);
	sendPacket.addString(whisperee);
	sendPacket.addString(message);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}

void WorldServerConnectPacket::rankingCalculation() {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_TO_LOGIN);
	sendPacket.add<header_t>(IMSG_CALCULATE_RANKING);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}

void WorldServerConnectPacket::sendToChannels(PacketCreator &packet) {
	PacketCreator sendPacket;
	sendPacket.add<int16_t>(IMSG_TO_CHANNELS);
	sendPacket.addBuffer(packet);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}

void WorldServerConnectPacket::sendToWorlds(PacketCreator &packet) {
	PacketCreator sendPacket;
	sendPacket.add<int16_t>(IMSG_TO_WORLDS);
	sendPacket.addBuffer(packet);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}

void WorldServerConnectPacket::reloadMcdb(const string &type) {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_REFRESH_DATA);
	sendPacket.addString(type);
	sendToChannels(sendPacket);
}

void WorldServerConnectPacket::rehashConfig() {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_TO_LOGIN);
	sendPacket.add<header_t>(IMSG_REHASH_CONFIG);
	ChannelServer::Instance()->sendPacketToWorld(sendPacket);
}