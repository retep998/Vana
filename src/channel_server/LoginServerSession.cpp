/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "LoginServerSession.hpp"
#include "common_temp/ExitCodes.hpp"
#include "common_temp/InterHeader.hpp"
#include "common_temp/PacketReader.hpp"
#include "common_temp/PacketWrapper.hpp"
#include "common_temp/ServerType.hpp"
#include "common_temp/WorldConfig.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/PartyHandler.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/SyncHandler.hpp"
#include "channel_server/LoginServerSessionHandler.hpp"
#include <iostream>

namespace vana {
namespace channel_server {

auto login_server_session::handle(packet_reader &reader) -> result {
	switch (reader.get<packet_header>()) {
		case IMSG_LOGIN_CHANNEL_CONNECT: login_server_session_handler::connect(shared_from_this(), reader); break;
		default: return result::failure;
	}
	return result::successful;
}

auto login_server_session::on_connect() -> void {
	channel_server::get_instance().on_connect_to_login(shared_from_this());
}

auto login_server_session::on_disconnect() -> void {
	channel_server::get_instance().on_disconnect_from_login();
}

}
}