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
#include "MonsterBookPacket.hpp"
#include "ItemDataProvider.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "SmsgHeader.hpp"

namespace Vana {
namespace Packets {
namespace MonsterBook {

PACKET_IMPL(addCard, item_id_t cardId, uint8_t level, bool full) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MONSTER_BOOK_ADD)
		.add<bool>(!full)
		.add<item_id_t>(cardId)
		.add<int32_t>(level);
	return builder;
}

SPLIT_PACKET_IMPL(addCardEffect, player_id_t playerId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.add<int8_t>(0x0D);

	// GMS doesnt send the animation for others.
	// If you want to enable displaying it, just uncomment
	/*
	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<player_id_t>(playerId)
		.add<int8_t>(0x0D);
	*/
	return builder;
}

PACKET_IMPL(changeCover, item_id_t cardId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MONSTER_BOOK_COVER)
		.add<item_id_t>(cardId);
	return builder;
}

}
}
}