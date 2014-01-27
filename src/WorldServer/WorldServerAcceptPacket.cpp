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
#include "WorldServerAcceptPacket.hpp"
#include "Channels.hpp"
#include "Configuration.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "PacketCreator.hpp"
#include "PacketReader.hpp"
#include "Party.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "TimeUtilities.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptConnection.hpp"
#include <unordered_map>
#include <map>

auto WorldServerAcceptPacket::groupChat(uint16_t channel, int32_t playerId, int8_t type, const string_t &message, const string_t &sender) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_TO_PLAYER);
	packet.add<int32_t>(playerId);
	packet.add<header_t>(SMSG_MESSAGE_GROUP);
	packet.add<int8_t>(type);
	packet.addString(sender);
	packet.addString(message);

	Channels::getInstance().sendToChannel(channel, packet);
}

auto WorldServerAcceptPacket::connect(WorldServerAcceptConnection *connection, uint16_t channel, port_t port) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_CHANNEL_CONNECT);
	packet.add<int16_t>(channel);
	packet.add<port_t>(port);

	packet.addClass<WorldConfig>(WorldServer::getInstance().getConfig());

	connection->getSession()->send(packet);
}

auto WorldServerAcceptPacket::findPlayer(WorldServerAcceptConnection *connection, int32_t finder, uint16_t channel, const string_t &findee, uint8_t is) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_FIND);
	packet.add<int32_t>(finder);
	packet.add<int16_t>(channel);
	packet.addString(findee);
	packet.add<int8_t>(is);

	connection->getSession()->send(packet);
}

auto WorldServerAcceptPacket::whisperPlayer(int16_t channel, int32_t whisperee, const string_t &whisperer, int16_t whispererChannel, const string_t &message) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_WHISPER);
	packet.add<int32_t>(whisperee);
	packet.addString(whisperer);
	packet.add<int16_t>(whispererChannel);
	packet.addString(message);

	Channels::getInstance().sendToChannel(channel, packet);
}

auto WorldServerAcceptPacket::rehashConfig(const WorldConfig &config) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_REHASH_CONFIG);
	packet.addClass<WorldConfig>(config);

	Channels::getInstance().sendToAll(packet);
}