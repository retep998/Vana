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
#include "LoginServerConnectHandler.hpp"
#include "Channels.hpp"
#include "Configuration.hpp"
#include "InitializeWorld.hpp"
#include "LoginServerConnection.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "SyncPacket.hpp"
#include "WorldServer.hpp"
#include <iostream>

auto LoginServerConnectHandler::connect(LoginServerConnection *connection, PacketReader &packet) -> void {
	int8_t worldId = packet.get<int8_t>();
	if (worldId != -1) {
		WorldServer::getInstance().setWorldId(worldId);
		WorldServer::getInstance().setInterPort(packet.get<port_t>());

		const WorldConfig &conf = packet.getClass<WorldConfig>();
		WorldServer::getInstance().setConfig(conf);

		WorldServer::getInstance().listen();
		std::cout << "Handling world " << static_cast<int32_t>(worldId) << std::endl;

		Initializing::worldEstablished();

		WorldServer::getInstance().displayLaunchTime();
	}
	else {
		std::cerr << "ERROR: No world to handle" << std::endl;
		WorldServer::getInstance().shutdown();
	}
}

auto LoginServerConnectHandler::newPlayer(PacketReader &packet) -> void {
	uint16_t channel = packet.get<int16_t>();
	int32_t playerId = packet.get<int32_t>();
	const Ip &ip = packet.getClass<Ip>();

	if (Channels::getInstance().getChannel(channel)) {
		Player *player = PlayerDataProvider::getInstance().getPlayer(playerId);
		if (player == nullptr || !player->isOnline()) {
			// Do not create the connectable if the player is already online
			// (extra security if the client ignores CC packet)
			PlayerDataProvider::getInstance().initialPlayerConnect(playerId, channel, ip);
			SyncPacket::PlayerPacket::newConnectable(channel, playerId, ip, packet);
		}
	}
}

auto LoginServerConnectHandler::rehashConfig(PacketReader &packet) -> void {
	const WorldConfig &config = packet.getClass<WorldConfig>();
	WorldServer::getInstance().rehashConfig(config);
}