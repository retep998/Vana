/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "WorldServerConnectPacket.hpp"
#include "ChannelServer.hpp"
#include "Configuration.hpp"
#include "InterHeader.hpp"
#include "PacketCreator.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "WorldServerConnection.hpp"

auto WorldServerConnectPacket::groupChat(int8_t type, int32_t playerId, const vector_t<int32_t> &receivers, const string_t &chat) -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_GROUP_CHAT);
	sendPacket.add<int32_t>(playerId);
	sendPacket.add<int8_t>(type);
	sendPacket.addString(chat);
	sendPacket.addVector(receivers);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto WorldServerConnectPacket::findPlayer(int32_t playerId, const string_t &findeeName) -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_FIND);
	sendPacket.add<int32_t>(playerId);
	sendPacket.addString(findeeName);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto WorldServerConnectPacket::whisperPlayer(int32_t playerId, const string_t &whisperee, const string_t &message) -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_WHISPER);
	sendPacket.add<int32_t>(playerId);
	sendPacket.addString(whisperee);
	sendPacket.addString(message);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto WorldServerConnectPacket::rankingCalculation() -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_TO_LOGIN);
	sendPacket.add<header_t>(IMSG_CALCULATE_RANKING);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto WorldServerConnectPacket::sendToChannels(PacketCreator &packet) -> void {
	PacketCreator sendPacket;
	sendPacket.add<int16_t>(IMSG_TO_CHANNELS);
	sendPacket.addBuffer(packet);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto WorldServerConnectPacket::sendToWorlds(PacketCreator &packet) -> void {
	PacketCreator sendPacket;
	sendPacket.add<int16_t>(IMSG_TO_WORLDS);
	sendPacket.addBuffer(packet);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}

auto WorldServerConnectPacket::reloadMcdb(const string_t &type) -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_REFRESH_DATA);
	sendPacket.addString(type);
	sendToChannels(sendPacket);
}

auto WorldServerConnectPacket::rehashConfig() -> void {
	PacketCreator sendPacket;
	sendPacket.add<header_t>(IMSG_TO_LOGIN);
	sendPacket.add<header_t>(IMSG_REHASH_CONFIG);
	ChannelServer::getInstance().sendPacketToWorld(sendPacket);
}