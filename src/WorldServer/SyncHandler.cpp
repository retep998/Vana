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
#include "SyncHandler.hpp"
#include "Common/Database.hpp"
#include "Common/InterHeader.hpp"
#include "Common/InterHelper.hpp"
#include "Common/PacketReader.hpp"
#include "Common/RatesConfig.hpp"
#include "Common/Session.hpp"
#include "WorldServer/Channel.hpp"
#include "WorldServer/Channels.hpp"
#include "WorldServer/PlayerDataProvider.hpp"
#include "WorldServer/SyncPacket.hpp"
#include "WorldServer/WorldServer.hpp"
#include "WorldServer/WorldServerAcceptedSession.hpp"
#include "WorldServer/WorldServerAcceptPacket.hpp"

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
		case sync::config::rate_set: world_server::get_instance().set_rates(reader.get<rates_config>()); break;
		case sync::config::rate_reset: world_server::get_instance().reset_rates(reader.get<int32_t>()); break;
		case sync::config::scrolling_header: world_server::get_instance().set_scrolling_header(reader.get<string>()); break;
		default: throw not_implemented_exception{"config_sync type"};
	}
}

}
}