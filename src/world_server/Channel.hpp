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

#include "common_temp/Ip.hpp"
#include "common_temp/ExternalIp.hpp"
#include "common_temp/ExternalIpResolver.hpp"
#include "common_temp/Types.hpp"
#include <vector>

namespace vana {
	class packet_builder;

	namespace world_server {
		class world_server_accepted_session;

		class channel : public external_ip_resolver {
			NONCOPYABLE(channel);
		public:
			channel(ref_ptr<world_server_accepted_session> session, game_channel_id id, connection_port port);

			auto increase_players() -> int32_t;
			auto decrease_players() -> int32_t;
			auto get_id() const -> game_channel_id;
			auto get_port() const -> connection_port;
			auto send(const packet_builder &builder) -> void;
			auto disconnect() -> void;
		private:
			game_channel_id m_id = 0;
			connection_port m_port = 0;
			int32_t m_players = 0;
			ref_ptr<world_server_accepted_session> m_session = nullptr;
		};
	}
}