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
#pragma once

#include "Ip.hpp"
#include "Types.hpp"

class LoginServerAcceptConnection;
class World;

namespace LoginServerAcceptPacket {
	auto connect(World *world) -> void;
	auto noMoreWorld(LoginServerAcceptConnection *connection) -> void;
	auto connectChannel(LoginServerAcceptConnection *connection, int8_t worldId, const Ip &ip, port_t port) -> void;
	auto newPlayer(World *world, uint16_t channel, int32_t charId, const Ip &ip) -> void;
	auto rehashConfig(World *world) -> void;
}