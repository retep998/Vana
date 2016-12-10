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
#include "login_server_session_handler.hpp"
#include "common/exit_code.hpp"
#include "common/session.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/login_server_session.hpp"
#include <iostream>
#include <limits>

namespace vana {
namespace channel_server {

auto login_server_session_handler::connect(ref_ptr<login_server_session> session, packet_reader &reader) -> void {
	game_world_id world_id = reader.get<game_world_id>();
	bool show_log_and_exit = true;
	if (world_id != -1) {
		ip value = reader.get<ip>();
		connection_port port = reader.get<connection_port>();
		if (result::success == channel_server::get_instance().connect_to_world(world_id, port, value)) {
			show_log_and_exit = false;
		}
	}

	if (show_log_and_exit) {
		channel_server::get_instance().log(vana::log::type::critical_error, "No world server to connect");
		exit(exit_code::server_connection_error);
	}
}

}
}