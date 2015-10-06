/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "LoginServerAcceptPacket.hpp"
#include "Common/InterHeader.hpp"
#include "Common/InterHelper.hpp"
#include "Common/WorldConfig.hpp"
#include "LoginServer/World.hpp"

namespace Vana {
namespace LoginServer {
namespace Packets {
namespace Interserver {

PACKET_IMPL(connect, World *world) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_WORLD_CONNECT)
		.add<world_id_t>(world->getId().get(-1))
		.add<port_t>(world->getPort())
		.add<WorldConfig>(world->getConfig());
	return builder;
}

PACKET_IMPL(noMoreWorld) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_WORLD_CONNECT)
		.add<int8_t>(-1);
	return builder;
}

PACKET_IMPL(connectChannel, optional_t<world_id_t> worldId, optional_t<Ip> ip, optional_t<port_t> port) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_LOGIN_CHANNEL_CONNECT)
		.add<world_id_t>(worldId.get(-1))
		.add<Ip>(ip.get(Ip{0}))
		.add<port_t>(port.get(0));
	return builder;
}

PACKET_IMPL(playerConnectingToChannel, channel_id_t channel, player_id_t charId, const Ip &ip) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_TO_CHANNEL)
		.add<channel_id_t>(channel)
		.add<header_t>(IMSG_SYNC)
		.add<sync_t>(Sync::SyncTypes::Player)
		.add<sync_t>(Sync::Player::NewConnectable)
		.add<player_id_t>(charId)
		.add<Ip>(ip)
		// The size of the held packet that should be there - there isn't one
		.add<uint16_t>(0);
	return builder;
}

PACKET_IMPL(rehashConfig, World *world) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_REHASH_CONFIG)
		.add<WorldConfig>(world->getConfig());
	return builder;
}

}
}
}
}