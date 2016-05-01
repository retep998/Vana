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
#pragma once

#include "Common/Types.hpp"
#include <functional>
#include <map>
#include <string>

namespace Vana {
	class PacketBuilder;
	class PacketReader;

	namespace LoginServer {
		class Channel;
		class LoginServerAcceptedSession;
		class User;
		class World;

		class Worlds {
		public:
			auto channelSelect(ref_ptr_t<User> user, PacketReader &reader) -> void;
			auto selectWorld(ref_ptr_t<User> user, PacketReader &reader) -> void;
			auto showWorld(ref_ptr_t<User> user) -> void;
			auto send(world_id_t id, const PacketBuilder &builder) -> void;
			auto send(const vector_t<world_id_t> &worlds, const PacketBuilder &builder) -> void;
			auto send(const PacketBuilder &builder) -> void;

			auto addWorld(World *world) -> void;
			auto calculatePlayerLoad(World *world) -> void;
			auto runFunction(function_t<bool (World *)> func) -> void;
			auto setEventMessages(const string_t &message) -> void;

			auto getWorld(world_id_t id) -> World *;

			// Inter-server
			auto addWorldServer(ref_ptr_t<LoginServerAcceptedSession> session) -> optional_t<world_id_t>;
			auto addChannelServer(ref_ptr_t<LoginServerAcceptedSession> session) -> optional_t<world_id_t>;
		private:
			ord_map_t<world_id_t, World *> m_worlds;
		};
	}
}