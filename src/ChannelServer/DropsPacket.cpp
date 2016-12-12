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
#include "DropsPacket.hpp"
#include "Drop.hpp"
#include "GameLogicUtilities.hpp"
#include "ItemConstants.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace DropsPacket {

PACKET_IMPL(showDrop, Drop *drop, int8_t type, const Point &origin) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_DROP_ITEM)
		.add<int8_t>(type)
		.add<map_object_t>(drop->getId())
		.add<bool>(drop->isMesos())
		.add<int32_t>(drop->getObjectId())
		.add<int32_t>(drop->getOwner()) // Owner of drop
		.add<int8_t>(drop->getType())
		.add<Point>(drop->getPos())
		.add<int32_t>(drop->getTime());

	if (type != DropTypes::ShowExisting) {
		// Give the point of origin for things that are just being dropped
		builder
			.add<Point>(origin)
			.add<int16_t>(0);
	}
	if (!drop->isMesos()) {
		builder.add<int64_t>(Items::NoExpiration);
	}
	builder.add<bool>(!drop->isPlayerDrop()); // Determines whether pets can pick item up or not
	return builder;
}

PACKET_IMPL(takeDrop, player_id_t playerId, map_object_t dropId, int8_t petIndex) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_DROP_PICKUP)
		.add<int8_t>(petIndex != -1 ? 5 : 2)
		.add<map_object_t>(dropId)
		.add<player_id_t>(playerId);
	return builder;
}

PACKET_IMPL(dontTake) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_INVENTORY_OPERATION)
		.add<int16_t>(1);
	return builder;
}

PACKET_IMPL(removeDrop, map_object_t dropId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_DROP_PICKUP)
		.add<int8_t>(0)
		.add<map_object_t>(dropId);
	return builder;
}

PACKET_IMPL(explodeDrop, map_object_t dropId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_DROP_PICKUP)
		.add<int8_t>(4)
		.add<map_object_t>(dropId)
		.add<int16_t>(655);
	return builder;
}

PACKET_IMPL(dropNotAvailableForPickup) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(0)
		.add<int8_t>(-2);
	return builder;
}

PACKET_IMPL(cantGetAnymoreItems) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(0)
		.add<int8_t>(-1);
	return builder;
}

PACKET_IMPL(pickupDrop, map_object_t id, int32_t amount, bool isMesos, int16_t cafeBonus) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(0)
		.add<bool>(isMesos)
		.add<map_object_t>(id);

	if (isMesos) {
		builder.add<int16_t>(cafeBonus);
	}
	else if (GameLogicUtilities::getInventory(id) != Inventories::EquipInventory) {
		builder.add<slot_qty_t>(static_cast<slot_qty_t>(amount));
	}
	if (!isMesos) {
		builder
			.add<int32_t>(0)
			.add<int32_t>(0);
	}
	return builder;
}

PACKET_IMPL(pickupDropSpecial, map_object_t id) {
	PacketBuilder builder;
	// This builder is used for PQ drops (maybe, got it from the Wing of the Wind item) and monster cards
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(0)
		.add<int8_t>(2)
		.add<map_object_t>(id);
	return builder;
}

}