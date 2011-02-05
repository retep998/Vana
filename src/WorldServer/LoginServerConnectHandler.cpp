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
#include "LoginServerConnectHandler.h"
#include "Channels.h"
#include "Configuration.h"
#include "ConfigurationPacket.h"
#include "InitializeWorld.h"
#include "LoginServerConnection.h"
#include "PacketReader.h"
#include "PlayerDataProvider.h"
#include "Rates.h"
#include "SyncPacket.h"
#include "WorldServer.h"
#include <iostream>

void LoginServerConnectHandler::connect(LoginServerConnection *player, PacketReader &packet) {
	int8_t worldid = packet.get<int8_t>();
	if (worldid != -1) {
		WorldServer::Instance()->setWorldId(worldid);
		WorldServer::Instance()->setInterPort(packet.get<uint16_t>());

		Configuration conf = ConfigurationPacket::getConfig(packet);
		WorldServer::Instance()->setConfig(conf);

		WorldServer::Instance()->setCashServerPort(packet.get<uint16_t>());

		WorldServer::Instance()->listen();
		Initializing::worldEstablished();
		std::cout << "Handling world " << (int32_t) worldid << std::endl;
		WorldServer::Instance()->displayLaunchTime();
	}
	else {
		std::cout << "Error: No world to handle" << std::endl;
		WorldServer::Instance()->shutdown();
	}
}

void LoginServerConnectHandler::newPlayer(PacketReader &packet) {
	uint16_t channel = packet.get<int16_t>();
	int32_t playerid = packet.get<int32_t>();
	uint32_t playerip = packet.get<uint32_t>();

	if (Channels::Instance()->getChannel(channel)) {
		if (PlayerDataProvider::Instance()->getPlayer(playerid) == nullptr) {
			// Do not create the connectable if the player is already online
			// (extra security if the client ignores CC packet)
			SyncPacket::PlayerPacket::newConnectable(channel, playerid, playerip);
		}
	}
}
