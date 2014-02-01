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
#include "StoragePacket.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "Inventory.hpp"
#include "Player.hpp"
#include "PlayerPacketHelper.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace StoragePacket {

PACKET_IMPL(showStorage, Player *player, int32_t npcId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_STORAGE)
		.add<int8_t>(0x16) // Type of storage action
		.add<int32_t>(npcId)
		.add<int8_t>(player->getStorage()->getSlots())
		.add<int32_t>(0x7e)
		.add<int32_t>(0)
		.add<int32_t>(player->getStorage()->getMesos())
		.add<int16_t>(0)
		.add<int8_t>(player->getStorage()->getNumItems());

	for (int8_t i = 0; i < player->getStorage()->getNumItems(); i++) {
		builder.addBuffer(PlayerPacketHelper::addItemInfo(0, player->getStorage()->getItem(i)));
	}

	builder
		.add<int16_t>(0)
		.add<int8_t>(0);
	return builder;
}

PACKET_IMPL(addItem, Player *player, int8_t inv) {
	PacketBuilder builder;
	int8_t type = static_cast<int8_t>(pow(2.f, static_cast<int32_t>(inv))) * 2; // Gotta work some magic on type, which starts as inventory
	builder
		.add<header_t>(SMSG_STORAGE)
		.add<int8_t>(0x0d)
		.add<int8_t>(player->getStorage()->getSlots())
		.add<int32_t>(type)
		.add<int32_t>(0)
		.add<int8_t>(player->getStorage()->getNumItems(inv));

	for (int8_t i = 0; i < player->getStorage()->getNumItems(); i++) {
		Item *item = player->getStorage()->getItem(i);
		if (GameLogicUtilities::getInventory(item->getId()) == inv) {
			builder.addBuffer(PlayerPacketHelper::addItemInfo(0, item));
		}
	}
	return builder;
}

PACKET_IMPL(takeItem, Player *player, int8_t inv) {
	PacketBuilder builder;
	int8_t type = static_cast<int8_t>(pow(2.f, static_cast<int32_t>(inv))) * 2;
	builder
		.add<header_t>(SMSG_STORAGE)
		.add<int8_t>(0x09)
		.add<int8_t>(player->getStorage()->getSlots())
		.add<int32_t>(type)
		.add<int32_t>(0)
		.add<int8_t>(player->getStorage()->getNumItems(inv));

	for (int8_t i = 0; i < player->getStorage()->getNumItems(); i++) {
		Item *item = player->getStorage()->getItem(i);
		if (GameLogicUtilities::getInventory(item->getId()) == inv) {
			builder.addBuffer(PlayerPacketHelper::addItemInfo(0, item));
		}
	}
	return builder;
}

PACKET_IMPL(changeMesos, int8_t slotCount, int32_t mesos) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_STORAGE)
		.add<int8_t>(0x13)
		.add<int8_t>(slotCount)
		.add<int16_t>(2)
		.add<int16_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(mesos);
	return builder;
}

PACKET_IMPL(storageFull) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_STORAGE)
		.add<int8_t>(0x11);
	return builder;
}

PACKET_IMPL(noMesos) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_STORAGE)
		.add<int8_t>(0x10);
	return builder;
}

}