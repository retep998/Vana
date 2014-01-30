/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Configuration.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "LoginServerAcceptConnection.hpp"
#include "PacketCreator.hpp"
#include "Session.hpp"
#include "World.hpp"
#include "Worlds.hpp"

auto LoginServerAcceptPacket::connect(World *world) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_WORLD_CONNECT);
	packet.add<world_id_t>(world->getId());
	packet.add<port_t>(world->getPort());

	packet.addClass<WorldConfig>(world->getConfig());

	world->send(packet);
}

auto LoginServerAcceptPacket::noMoreWorld(LoginServerAcceptConnection *connection) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_WORLD_CONNECT);
	packet.add<int8_t>(-1);
	connection->getSession()->send(packet);
}

auto LoginServerAcceptPacket::connectChannel(LoginServerAcceptConnection *connection, world_id_t worldId, const Ip &ip, port_t port) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_LOGIN_CHANNEL_CONNECT);
	packet.add<world_id_t>(worldId);
	packet.addClass<Ip>(ip);
	packet.add<port_t>(port);
	connection->getSession()->send(packet);
}

auto LoginServerAcceptPacket::playerConnectingToChannel(World *world, channel_id_t channel, int32_t charId, const Ip &ip) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_TO_CHANNEL);
	packet.add<channel_id_t>(channel);
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::NewConnectable);
	packet.add<int32_t>(charId);
	packet.addClass<Ip>(ip);
	// The size of the held packet that should be there - there isn't one
	packet.add<uint16_t>(0);
	world->send(packet);
}

auto LoginServerAcceptPacket::rehashConfig(World *world) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_REHASH_CONFIG);
	packet.addClass<WorldConfig>(world->getConfig());
	world->send(packet);
}