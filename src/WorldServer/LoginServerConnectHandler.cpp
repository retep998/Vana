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
#include "LoginServerConnectHandler.h"
#include "Channels.h"
#include "Configuration.h"
#include "InitializeWorld.h"
#include "LoginServerConnection.h"
#include "PacketReader.h"
#include "PlayerDataProvider.h"
#include "Rates.h"
#include "SyncPacket.h"
#include "WorldServer.h"
#include <iostream>

void LoginServerConnectHandler::connect(LoginServerConnection *connection, PacketReader &packet) {
	int8_t worldId = packet.get<int8_t>();
	if (worldId != -1) {
		WorldServer::Instance()->setWorldId(worldId);
		WorldServer::Instance()->setInterPort(packet.get<port_t>());

		const WorldConfig &conf = packet.getClass<WorldConfig>();
		WorldServer::Instance()->setConfig(conf);

		WorldServer::Instance()->listen();
		std::cout << "Handling world " << static_cast<int32_t>(worldId) << std::endl;

		Initializing::worldEstablished();

		WorldServer::Instance()->displayLaunchTime();
	}
	else {
		std::cerr << "Error: No world to handle" << std::endl;
		WorldServer::Instance()->shutdown();
	}
}

void LoginServerConnectHandler::newPlayer(PacketReader &packet) {
	uint16_t channel = packet.get<int16_t>();
	int32_t playerId = packet.get<int32_t>();
	ip_t ip = packet.get<ip_t>();
	int64_t loginKey = packet.get<int64_t>(); // TODO: IMPLEMENT

	if (Channels::Instance()->getChannel(channel)) {
		if (PlayerDataProvider::Instance()->getPlayer(playerId) == nullptr) {
			// Do not create the connectable if the player is already online
			// (extra security if the client ignores CC packet)
			SyncPacket::PlayerPacket::newConnectable(channel, playerId, ip, loginKey, packet);
		}
	}
}