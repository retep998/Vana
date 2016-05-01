/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Common/ExitCodes.hpp"
#include "Common/PacketReader.hpp"
#include "Common/WorldConfig.hpp"
#include "WorldServer/Channels.hpp"
#include "WorldServer/LoginServerSession.hpp"
#include "WorldServer/PlayerDataProvider.hpp"
#include "WorldServer/SyncPacket.hpp"
#include "WorldServer/WorldServer.hpp"
#include <iostream>

namespace Vana {
namespace WorldServer {

auto LoginServerConnectHandler::connect(ref_ptr_t<LoginServerSession> session, PacketReader &reader) -> void {
	world_id_t worldId = reader.get<world_id_t>();
	if (worldId != -1) {
		port_t port = reader.get<port_t>();
		WorldConfig conf = reader.get<WorldConfig>();
		WorldServer::getInstance().establishedLoginConnection(worldId, port, conf);
	}
	else {
		WorldServer::getInstance().log(LogType::CriticalError, "No world to handle");
		ExitCodes::exit(ExitCodes::ServerConnectionError);
	}
}

}
}