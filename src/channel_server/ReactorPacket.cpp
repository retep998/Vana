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
#include "ReactorPacket.hpp"
#include "common/Session.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/Reactor.hpp"
#include "channel_server/SmsgHeader.hpp"

namespace vana {
namespace channel_server {
namespace packets {

PACKET_IMPL(spawn_reactor, reactor *reactor) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_REACTOR_SPAWN)
		.add<game_map_object>(reactor->get_id())
		.add<game_reactor_id>(reactor->get_reactor_id())
		.add<int8_t>(reactor->get_state())
		.add<point>(reactor->get_pos())
		.add<bool>(reactor->faces_left());
	return builder;
}

PACKET_IMPL(trigger_reactor, reactor *reactor) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_REACTOR_TRIGGER)
		.add<game_map_object>(reactor->get_id())
		.add<int8_t>(reactor->get_state())
		.add<point>(reactor->get_pos())
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(destroy_reactor, reactor *reactor) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_REACTOR_DESPAWN)
		.add<game_map_object>(reactor->get_id())
		.add<int8_t>(reactor->get_state())
		.add<point>(reactor->get_pos());
	return builder;
}

}
}
}