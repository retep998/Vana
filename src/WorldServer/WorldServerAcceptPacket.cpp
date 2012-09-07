/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "Configuration.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "PlayerDataProvider.h"
#include "Session.h"
#include "SmsgHeader.h"
#include "TimeUtilities.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"
#include <unordered_map>
#include <map>

using std::map;
using std::unordered_map;

void WorldServerAcceptPacket::groupChat(uint16_t channel, int32_t playerId, int8_t type, const string &message, const string &sender) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_TO_PLAYER);
	packet.add<int32_t>(playerId);
	packet.add<header_t>(SMSG_MESSAGE_GROUP);
	packet.add<int8_t>(type);
	packet.addString(sender);
	packet.addString(message);

	Channels::Instance()->sendToChannel(channel, packet);
}

void WorldServerAcceptPacket::connect(WorldServerAcceptConnection *connection, uint16_t channel, port_t port) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_CHANNEL_CONNECT);
	packet.add<int16_t>(channel);
	packet.add<port_t>(port);

	packet.addClass<WorldConfig>(WorldServer::Instance()->getConfig());

	connection->getSession()->send(packet);
}

void WorldServerAcceptPacket::findPlayer(WorldServerAcceptConnection *connection, int32_t finder, uint16_t channel, const string &findee, uint8_t is) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_FIND);
	packet.add<int32_t>(finder);
	packet.add<int16_t>(channel);
	packet.addString(findee);
	packet.add<int8_t>(is);

	connection->getSession()->send(packet);
}

void WorldServerAcceptPacket::whisperPlayer(int16_t channel, int32_t whisperee, const string &whisperer, int16_t whispererChannel, const string &message) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_WHISPER);
	packet.add<int32_t>(whisperee);
	packet.addString(whisperer);
	packet.add<int16_t>(whispererChannel);
	packet.addString(message);

	Channels::Instance()->sendToChannel(channel, packet);
}

void WorldServerAcceptPacket::rehashConfig(const WorldConfig &config) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_REHASH_CONFIG);
	packet.addClass<WorldConfig>(config);

	Channels::Instance()->sendToAll(packet);
}