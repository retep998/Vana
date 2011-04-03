/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "ConfigurationPacket.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "PlayerDataProvider.h"
#include "Rates.h"
#include "Session.h"
#include "SmsgHeader.h"
#include "TimeUtilities.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"
#include <boost/tr1/unordered_map.hpp>
#include <map>

using std::map;
using std::tr1::unordered_map;

void WorldServerAcceptPacket::groupChat(uint16_t channel, int32_t playerid, int8_t type, const string &message, const string &sender) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_MESSAGE_GROUP);
	packet.add<int8_t>(type);
	packet.addString(sender);
	packet.addString(message);
	Channels::Instance()->sendToChannel(channel, packet);
}

void WorldServerAcceptPacket::connect(WorldServerAcceptConnection *connection, uint16_t channel, port_t port) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_CHANNEL_CONNECT);
	packet.add<int16_t>(channel);
	packet.add<port_t>(port);

	ConfigurationPacket::addConfig(WorldServer::Instance()->getConfig(), packet);

	connection->getSession()->send(packet);
}

void WorldServerAcceptPacket::findPlayer(WorldServerAcceptConnection *connection, int32_t finder, uint16_t channel, const string &findee, uint8_t is) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FIND);
	packet.add<int32_t>(finder);
	packet.add<int16_t>(channel);
	packet.addString(findee);
	packet.add<int8_t>(is);

	connection->getSession()->send(packet);
}

void WorldServerAcceptPacket::whisperPlayer(int16_t channel, int32_t whisperee, const string &whisperer, int16_t whispererChannel, const string &message) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_WHISPER);
	packet.add<int32_t>(whisperee);
	packet.addString(whisperer);
	packet.add<int16_t>(whispererChannel);
	packet.addString(message);

	Channels::Instance()->sendToChannel(channel, packet);
}

void WorldServerAcceptPacket::scrollingHeader(const string &message) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SCROLLING_HEADER);
	packet.addString(message);

	Channels::Instance()->sendToAll(packet);
}

void WorldServerAcceptPacket::sendRates(WorldServerAcceptConnection *connection, int32_t setBit) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SET_RATES);
	packet.add<int32_t>(setBit);

	Configuration &conf = WorldServer::Instance()->getConfig();

	if (setBit & Rates::SetBits::Exp) {
		packet.add<int32_t>(conf.expRate);
	}
	if (setBit & Rates::SetBits::QuestExp) {
		packet.add<int32_t>(conf.questExpRate);
	}
	if (setBit & Rates::SetBits::Meso) {
		packet.add<int32_t>(conf.mesoRate);
	}
	if (setBit & Rates::SetBits::Drop) {
		packet.add<int32_t>(conf.dropRate);
	}

	connection->getSession()->send(packet);
}