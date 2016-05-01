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
#include "LoginServerSessionHandler.hpp"
#include "Common/ExitCodes.hpp"
#include "Common/Session.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/LoginServerSession.hpp"
#include <iostream>
#include <limits>

namespace Vana {
namespace ChannelServer {

auto LoginServerSessionHandler::connect(ref_ptr_t<LoginServerSession> session, PacketReader &reader) -> void {
	world_id_t worldId = reader.get<world_id_t>();
	bool showLogAndExit = true;
	if (worldId != -1) {
		Ip ip = reader.get<Ip>();
		port_t port = reader.get<port_t>();
		if (Result::Successful == ChannelServer::getInstance().connectToWorld(worldId, port, ip)) {
			showLogAndExit = false;
		}
	}

	if (showLogAndExit) {
		ChannelServer::getInstance().log(LogType::CriticalError, "No world server to connect");
		ExitCodes::exit(ExitCodes::ServerConnectionError);
	}
}

}
}