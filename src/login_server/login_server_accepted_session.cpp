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
#include "login_server_accepted_session.hpp"
#include "common/common_header.hpp"
#include "common/inter_header.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/server_type.hpp"
#include "common/session.hpp"
#include "common/util/string.hpp"
#include "login_server/login_server.hpp"
#include "login_server/login_server_accept_handler.hpp"
#include "login_server/ranking_calculator.hpp"
#include "login_server/world.hpp"
#include "login_server/worlds.hpp"

namespace vana {
namespace login_server {

login_server_accepted_session::login_server_accepted_session(abstract_server &server) :
	server_accepted_session{server}
{
}

auto login_server_accepted_session::handle(packet_reader &reader) -> result {
	if (server_accepted_session::handle(reader) == result::failure) {
		return result::failure;
	}
	auto &server = login_server::get_instance();
	switch (reader.get<packet_header>()) {
		case IMSG_REGISTER_CHANNEL: login_server_accept_handler::register_channel(shared_from_this(), reader); break;
		case IMSG_UPDATE_CHANNEL_POP: login_server_accept_handler::update_channel_pop(shared_from_this(), reader); break;
		case IMSG_REMOVE_CHANNEL: login_server_accept_handler::remove_channel(shared_from_this(), reader); break;
		case IMSG_CALCULATE_RANKING: ranking_calculator::run_thread(); break;
		case IMSG_TO_WORLD: {
			game_world_id world_id = reader.get<game_world_id>();
			server.get_worlds().send(world_id, packets::identity(reader));
			break;
		}
		case IMSG_TO_WORLD_LIST: {
			vector<game_world_id> worlds = reader.get<vector<game_world_id>>();
			server.get_worlds().send(worlds, packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_WORLDS: server.get_worlds().send(packets::identity(reader)); break;

		case IMSG_REHASH_CONFIG: server.rehash_config(); break;

		case CMSG_PONG:
		case SMSG_PING:
		case IMSG_PASSWORD:
			/* Intentionally blank */
			break;

		default: return result::failure;
	}
	return result::success;
}

auto login_server_accepted_session::authenticated(server_type type) -> void {
	switch (type) {
		case server_type::world: login_server::get_instance().get_worlds().add_world_server(shared_from_this()); break;
		case server_type::channel: login_server::get_instance().get_worlds().add_channel_server(shared_from_this()); break;
		default: disconnect();
	}
}

auto login_server_accepted_session::set_world_id(game_world_id id) -> void {
	m_world_id = id;
}

auto login_server_accepted_session::get_world_id() const -> optional<game_world_id> {
	return m_world_id;
}

auto login_server_accepted_session::on_disconnect() -> void {
	auto &server = login_server::get_instance();
	if (m_world_id.is_initialized()) {
		world *world_value = server.get_worlds().get_world(m_world_id.get());
		world_value->set_connected(false);
		world_value->clear_channels();

		server.log(vana::log::type::server_disconnect, [&](out_stream &log) {
			log << "World " << static_cast<int32_t>(m_world_id.get());
		});
	}
	server.finalize_server_session(shared_from_this());
}

}
}