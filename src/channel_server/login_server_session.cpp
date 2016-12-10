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
#include "login_server_session.hpp"
#include "common/common_header.hpp"
#include "common/config/world.hpp"
#include "common/exit_code.hpp"
#include "common/inter_header.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/server_type.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/party_handler.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/sync_handler.hpp"
#include "channel_server/login_server_session_handler.hpp"
#include <iostream>

namespace vana {
namespace channel_server {

auto login_server_session::handle(packet_reader &reader) -> result {
	switch (reader.get<packet_header>()) {
		case IMSG_LOGIN_CHANNEL_CONNECT: login_server_session_handler::connect(shared_from_this(), reader); break;

		case CMSG_PONG:
		case SMSG_PING:
			/* Intentionally blank */
			break;

		default: return result::failure;
	}
	return result::success;
}

auto login_server_session::on_connect() -> void {
	channel_server::get_instance().on_connect_to_login(shared_from_this());
}

auto login_server_session::on_disconnect() -> void {
	channel_server::get_instance().on_disconnect_from_login();
}

}
}