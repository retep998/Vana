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
#include "common/packet_reader.hpp"
#include "channel_server/buddy_list_packet.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/party.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_buddy_list.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/smsg_header.hpp"
#include "channel_server/sync_packet.hpp"

namespace vana {
namespace channel_server {

auto sync_handler::handle(packet_reader &reader) -> void {
	protocol_sync type = reader.get<protocol_sync>();
	switch (type) {
		case sync::sync_types::config: handle_config_sync(reader); break;
		default: channel_server::get_instance().get_player_data_provider().handle_sync(type, reader); break;
	}
}

auto sync_handler::handle_config_sync(packet_reader &reader) -> void {
	switch (reader.get<protocol_sync>()) {
		case sync::config::rate_set: channel_server::get_instance().set_rates(reader.get<config::rates>()); break;
		case sync::config::scrolling_header: channel_server::get_instance().set_scrolling_header(reader.get<string>()); break;
		default: throw not_implemented_exception{"config_sync type"};
	}
}

}
}