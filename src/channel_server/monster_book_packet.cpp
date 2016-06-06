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
#include "monster_book_packet.hpp"
#include "common/item_data_provider.hpp"
#include "channel_server/maps_temp.hpp"
#include "channel_server/player_temp.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace monster_book {

PACKET_IMPL(add_card, game_item_id card_id, uint8_t level, bool full) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MONSTER_BOOK_ADD)
		.add<bool>(!full)
		.add<game_item_id>(card_id)
		.add<int32_t>(level);
	return builder;
}

SPLIT_PACKET_IMPL(add_card_effect, game_player_id player_id) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_THEATRICS)
		.add<int8_t>(0x0D);

	// GMS doesnt send the animation for others.
	// If you want to enable displaying it, just uncomment
	/*
	builder.map
		.add<packet_header>(SMSG_SKILL_SHOW)
		.add<game_player_id>(player_id)
		.add<int8_t>(0x0D);
	*/
	return builder;
}

PACKET_IMPL(change_cover, game_item_id card_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MONSTER_BOOK_COVER)
		.add<game_item_id>(card_id);
	return builder;
}

}
}
}
}