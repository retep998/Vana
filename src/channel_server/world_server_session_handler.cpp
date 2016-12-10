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
#include "world_server_session_handler.hpp"
#include "common/config/world.hpp"
#include "common/data/provider/beauty.hpp"
#include "common/data/provider/drop.hpp"
#include "common/data/provider/item.hpp"
#include "common/data/provider/mob.hpp"
#include "common/data/provider/quest.hpp"
#include "common/data/provider/reactor.hpp"
#include "common/data/provider/script.hpp"
#include "common/data/provider/shop.hpp"
#include "common/data/provider/skill.hpp"
#include "common/exit_code.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/session.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/players_packet.hpp"
#include "channel_server/world_server_session.hpp"
#include <iostream>
#include <limits>

namespace vana {
namespace channel_server {

auto world_server_session_handler::connect(ref_ptr<world_server_session> session, packet_reader &reader) -> void {
	game_channel_id channel = reader.get<game_channel_id>();
	if (channel != -1) {
		connection_port port = reader.get<connection_port>();
		config::world conf = reader.get<config::world>();
		channel_server::get_instance().established_world_connection(channel, port, conf);
	}
	else {
		channel_server::get_instance().log(vana::log::type::critical_error, "no channel to handle");
		channel_server::get_instance().shutdown();
	}
}

auto world_server_session_handler::reload_mcdb(packet_reader &reader) -> void {
	string args = reader.get<string>();
	channel_server::get_instance().reload_data(args);
}

}
}