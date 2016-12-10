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
#include "sync_handler.hpp"
#include "common/config/rates.hpp"
#include "common/inter_header.hpp"
#include "common/inter_helper.hpp"
#include "common/io/database.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include "world_server/channel.hpp"
#include "world_server/channels.hpp"
#include "world_server/player_data_provider.hpp"
#include "world_server/sync_packet.hpp"
#include "world_server/world_server.hpp"
#include "world_server/world_server_accepted_session.hpp"
#include "world_server/world_server_accept_packet.hpp"

namespace vana {
namespace world_server {

auto sync_handler::handle(ref_ptr<world_server_accepted_session> session, packet_reader &reader) -> void {
	protocol_sync type = reader.get<protocol_sync>();
	switch (type) {
		case sync::sync_types::config: handle_config_sync(reader); break;
		default: world_server::get_instance().get_player_data_provider().handle_sync(session, type, reader); break;
	}
}

auto sync_handler::handle(ref_ptr<login_server_session> session, packet_reader &reader) -> void {
	protocol_sync type = reader.get<protocol_sync>();
	world_server::get_instance().get_player_data_provider().handle_sync(session, type, reader);
}

auto sync_handler::handle_config_sync(packet_reader &reader) -> void {
	switch (reader.get<protocol_sync>()) {
		case sync::config::rate_set: world_server::get_instance().set_rates(reader.get<config::rates>()); break;
		case sync::config::rate_reset: world_server::get_instance().reset_rates(reader.get<int32_t>()); break;
		case sync::config::scrolling_header: world_server::get_instance().set_scrolling_header(reader.get<string>()); break;
		default: THROW_CODE_EXCEPTION(not_implemented_exception, "config_sync type");
	}
}

}
}