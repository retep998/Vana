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

#include "Common/Ip.hpp"
#include "Common/ExternalIp.hpp"
#include "Common/ExternalIpResolver.hpp"
#include "Common/Types.hpp"
#include <vector>

namespace Vana {
	class PacketBuilder;

	namespace WorldServer {
		class WorldServerAcceptedSession;

		class Channel : public ExternalIpResolver {
			NONCOPYABLE(Channel);
		public:
			Channel(ref_ptr_t<WorldServerAcceptedSession> session, channel_id_t id, port_t port);

			auto increasePlayers() -> int32_t;
			auto decreasePlayers() -> int32_t;
			auto getId() const -> channel_id_t;
			auto getPort() const -> port_t;
			auto send(const PacketBuilder &builder) -> void;
			auto disconnect() -> void;
		private:
			channel_id_t m_id = 0;
			port_t m_port = 0;
			int32_t m_players = 0;
			ref_ptr_t<WorldServerAcceptedSession> m_session = nullptr;
		};
	}
}