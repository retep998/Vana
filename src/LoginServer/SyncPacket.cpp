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
#include "SyncPacket.hpp"
#include "Common/InterHeader.hpp"
#include "Common/InterHelper.hpp"

namespace vana {
namespace login_server {
namespace packets {
namespace interserver {
namespace player {

PACKET_IMPL(character_created, game_player_id player_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::character_created)
		.add<game_player_id>(player_id);
	return builder;
}

PACKET_IMPL(character_deleted, game_player_id player_id) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::character_deleted)
		.add<game_player_id>(player_id);
	return builder;
}

}
}
}
}
}