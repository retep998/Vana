/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "LoginServerConnectHandler.h"
#include "InitializeWorld.h"
#include "LoginServerConnection.h"
#include "WorldServerAcceptPacket.h"
#include "WorldServer.h"
#include "Channels.h"
#include "PacketReader.h"
#include "Players.h"
#include "Rates.h"
#include <iostream>

void LoginServerConnectHandler::connect(LoginServerConnection *player, PacketReader &packet) {
	int8_t worldid = packet.get<int8_t>();
	if (worldid != -1) {
		WorldServer::Instance()->setWorldId(worldid);
		WorldServer::Instance()->setInterPort(packet.get<int16_t>());
		WorldServer::Instance()->setMaxChannels(packet.get<int32_t>());
		WorldServer::Instance()->setMaxChars(packet.get<int32_t>());
		WorldServer::Instance()->setMaxMultiLevel(packet.get<int8_t>());
		WorldServer::Instance()->setMaxStats(packet.get<int16_t>());
		WorldServer::Instance()->setScrollingHeader(packet.getString());

		// Boss junk
		WorldServer::Instance()->setPianusAttempts(packet.get<int16_t>());
		WorldServer::Instance()->setPapAttempts(packet.get<int16_t>());
		WorldServer::Instance()->setZakumAttempts(packet.get<int16_t>());
		WorldServer::Instance()->setHorntailAttempts(packet.get<int16_t>());
		WorldServer::Instance()->setPinkBeanAttempts(packet.get<int16_t>());
		WorldServer::Instance()->setPianusChannels(packet.getVector<int8_t>());
		WorldServer::Instance()->setPapChannels(packet.getVector<int8_t>());
		WorldServer::Instance()->setZakumChannels(packet.getVector<int8_t>());
		WorldServer::Instance()->setHorntailChannels(packet.getVector<int8_t>());
		WorldServer::Instance()->setPinkBeanChannels(packet.getVector<int8_t>());

		// Rates
		int32_t ratesSetBit = packet.get<int32_t>();
		if (ratesSetBit & Rates::SetBits::exp) {
			WorldServer::Instance()->setExprate(packet.get<int32_t>());
		}
		if (ratesSetBit & Rates::SetBits::questExp) {
			WorldServer::Instance()->setQuestExprate(packet.get<int32_t>());
		}
		if (ratesSetBit & Rates::SetBits::meso) {
			WorldServer::Instance()->setMesorate(packet.get<int32_t>());
		}
		if (ratesSetBit & Rates::SetBits::drop) {
			WorldServer::Instance()->setDroprate(packet.get<int32_t>());
		}

		WorldServer::Instance()->listen();
		std::cout << "Handling world " << (int32_t) worldid << std::endl;

		Initializing::loadPostAssignment(worldid);
	}
	else {
		std::cout << "Error: No world to handle" << std::endl;
		WorldServer::Instance()->shutdown();
	}
}

void LoginServerConnectHandler::newPlayer(PacketReader &packet) {
	uint16_t channel = packet.get<int16_t>();
	int32_t playerid = packet.get<int32_t>();

	if (Channels::Instance()->getChannel(channel)) {
		if (Players::Instance()->getPlayer(playerid) == 0) {
			// Do not create the connectable if the player is already online
			// (extra security if the client ignores CC packet)
			WorldServerAcceptPacket::newConnectable(channel, playerid);
		}
	}
}
