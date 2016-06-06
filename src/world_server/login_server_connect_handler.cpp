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
#include "login_server_connect_handler.hpp"
#include "common/exit_codes.hpp"
#include "common/packet_reader.hpp"
#include "common/world_config.hpp"
#include "world_server/channels_temp.hpp"
#include "world_server/login_server_session.hpp"
#include "world_server/player_data_provider.hpp"
#include "world_server/sync_packet.hpp"
#include "world_server/world_server.hpp"
#include <iostream>

namespace vana {
namespace world_server {

auto login_server_connect_handler::connect(ref_ptr<login_server_session> session, packet_reader &reader) -> void {
	game_world_id world_id = reader.get<game_world_id>();
	if (world_id != -1) {
		connection_port port = reader.get<connection_port>();
		world_config conf = reader.get<world_config>();
		world_server::get_instance().established_login_connection(world_id, port, conf);
	}
	else {
		world_server::get_instance().log(log_type::critical_error, "No world to handle");
		exit(exit_code::server_connection_error);
	}
}

}
}