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
#include "Common/Session.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/Reactor.hpp"
#include "ChannelServer/SmsgHeader.hpp"

namespace Vana {
namespace ChannelServer {
namespace Packets {

PACKET_IMPL(spawnReactor, Reactor *reactor) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_REACTOR_SPAWN)
		.add<map_object_t>(reactor->getId())
		.add<reactor_id_t>(reactor->getReactorId())
		.add<int8_t>(reactor->getState())
		.add<Point>(reactor->getPos())
		.add<bool>(reactor->facesLeft());
	return builder;
}

PACKET_IMPL(triggerReactor, Reactor *reactor) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_REACTOR_TRIGGER)
		.add<map_object_t>(reactor->getId())
		.add<int8_t>(reactor->getState())
		.add<Point>(reactor->getPos())
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(destroyReactor, Reactor *reactor) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_REACTOR_DESPAWN)
		.add<map_object_t>(reactor->getId())
		.add<int8_t>(reactor->getState())
		.add<Point>(reactor->getPos());
	return builder;
}

}
}
}