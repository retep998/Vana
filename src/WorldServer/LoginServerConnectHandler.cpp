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
#include "ExitCodes.hpp"
#include "LoginServerConnection.hpp"
#include "PacketReader.hpp"
#include "PlayerDataProvider.hpp"
#include "SyncPacket.hpp"
#include "WorldServer.hpp"
#include <iostream>

auto LoginServerConnectHandler::connect(LoginServerConnection *connection, PacketReader &reader) -> void {
	world_id_t worldId = reader.get<world_id_t>();
	if (worldId != -1) {
		port_t port = reader.get<port_t>();
		WorldConfig conf = reader.get<WorldConfig>();
		std::cout << "Handling world " << static_cast<int32_t>(worldId) << std::endl;
		WorldServer::getInstance().establishedLoginConnection(worldId, port, conf);
	}
	else {
		std::cerr << "ERROR: No world to handle" << std::endl;
		ExitCodes::exit(ExitCodes::ServerConnectionError);
	}
}