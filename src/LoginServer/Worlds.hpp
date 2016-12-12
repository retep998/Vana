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

#include "Types.hpp"
#include <functional>
#include <map>
#include <string>

class Channel;
class LoginServerAcceptConnection;
class PacketBuilder;
class PacketReader;
class UserConnection;
class World;

class Worlds {
public:
	auto channelSelect(UserConnection *user, PacketReader &reader) -> void;
	auto selectWorld(UserConnection *user, PacketReader &reader) -> void;
	auto showWorld(UserConnection *user) -> void;
	auto send(world_id_t id, const PacketBuilder &builder) -> void;
	auto send(const vector_t<world_id_t> &worlds, const PacketBuilder &builder) -> void;
	auto send(const PacketBuilder &builder) -> void;

	auto addWorld(World *world) -> void;
	auto calculatePlayerLoad(World *world) -> void;
	auto runFunction(function_t<bool (World *)> func) -> void;
	auto setEventMessages(const string_t &message) -> void;

	auto getWorld(world_id_t id) -> World *;

	// Inter-server
	auto addWorldServer(LoginServerAcceptConnection *connection) -> world_id_t;
	auto addChannelServer(LoginServerAcceptConnection *connection) -> world_id_t;
private:
	ord_map_t<world_id_t, World *> m_worlds;
};