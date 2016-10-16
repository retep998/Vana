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

#include "common/ip.hpp"
#include "common/packet_builder.hpp"
#include "common/types.hpp"
#include "common/util/optional.hpp"

namespace vana {
	class ip;

	namespace login_server {
		class world;

		namespace packets {
			namespace interserver {
				PACKET(connect, world *world_value);
				PACKET(no_more_world);
				PACKET(connect_channel, optional<game_world_id> world_id, optional<ip> ip_value, optional<connection_port> port);
				PACKET(player_connecting_to_channel, game_channel_id chan_id, game_player_id char_id, const ip &ip_value);
				PACKET(rehash_config, world *world_value);
			}
		}
	}
}