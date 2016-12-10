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
#include "login_server_accept_handler.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/util/string.hpp"
#include "login_server/channel.hpp"
#include "login_server/login_server.hpp"
#include "login_server/login_server_accepted_session.hpp"
#include "login_server/world.hpp"
#include "login_server/worlds.hpp"
#include <iostream>

namespace vana {
namespace login_server {

auto login_server_accept_handler::register_channel(ref_ptr<login_server_accepted_session> session, packet_reader &reader) -> void {
	game_channel_id chan_id = reader.get<game_channel_id>();
	auto chan = new vana::login_server::channel{};
	const ip &ip_value = reader.get<ip>();
	optional<game_world_id> world_id = session->get_world_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}

	chan->set_external_ip_information(ip_value, reader.get<vector<external_ip>>());
	chan->set_port(reader.get<connection_port>());
	login_server::get_instance().get_worlds().get_world(world_id.get())->add_channel(chan_id, chan);
	login_server::get_instance().log(vana::log::type::server_connect, [&](out_stream &log) {
		log << "World " << static_cast<int32_t>(world_id.get()) << "; Channel " << static_cast<int32_t>(chan_id);
	});
}

auto login_server_accept_handler::update_channel_pop(ref_ptr<login_server_accepted_session> session, packet_reader &reader) -> void {
	game_channel_id chan_id = reader.get<game_channel_id>();
	int32_t population = reader.get<int32_t>();
	optional<game_world_id> world_id = session->get_world_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}

	auto &worlds = login_server::get_instance().get_worlds();
	world *world_value = worlds.get_world(world_id.get());
	world_value->get_channel(chan_id)->set_population(population);
	worlds.calculate_player_load(world_value);
}

auto login_server_accept_handler::remove_channel(ref_ptr<login_server_accepted_session> session, packet_reader &reader) -> void {
	game_channel_id chan_id = reader.get<game_channel_id>();

	optional<game_world_id> world_id = session->get_world_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}

	login_server::get_instance().get_worlds().get_world(world_id.get())->remove_channel(chan_id);
	login_server::get_instance().log(vana::log::type::server_disconnect, [&](out_stream &log) {
		log << "World " << static_cast<int32_t>(world_id.get()) << "; Channel " << static_cast<int32_t>(chan_id);
	});
}

}
}