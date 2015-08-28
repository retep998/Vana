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
#include "PlayerPacketHelper.hpp"
#include "FileTime.hpp"
#include "GameLogicUtilities.hpp"
#include "Inventory.hpp"
#include "ItemConstants.hpp"
#include "Pet.hpp"
#include "Player.hpp"
#include "TimeUtilities.hpp"
#include <cmath>

namespace Vana {
namespace Packets {
namespace Helpers {

PACKET_IMPL(addItemInfo, inventory_slot_t slot, Item *item, bool shortSlot) {
	PacketBuilder builder;
	if (slot != 0) {
		if (shortSlot) {
			builder.add<inventory_slot_t>(slot);
		}
		else {
			slot = std::abs(slot);
			if (slot > 100) {
				slot -= 100;
			}
			builder.add<int8_t>(static_cast<int8_t>(slot));
		}
	}
	bool equip = GameLogicUtilities::isEquip(item->getId());
	builder
		.add<int8_t>(equip ? 1 : 2)
		.add<item_id_t>(item->getId());

	if (false) { //item->getCashId() != 0) {
		builder
			.add<int8_t>(1)
			.add<int64_t>(0); //item->getCashId());
	}
	else {
		builder.add<int8_t>(0);
	}
	builder.add<FileTime>(item->getExpirationTime());
	if (equip) {
		builder
			.add<int8_t>(item->getSlots())
			.add<int8_t>(item->getScrolls())
			.add<stat_t>(item->getStr())
			.add<stat_t>(item->getDex())
			.add<stat_t>(item->getInt())
			.add<stat_t>(item->getLuk())
			.add<health_t>(item->getHp())
			.add<health_t>(item->getMp())
			.add<stat_t>(item->getWatk())
			.add<stat_t>(item->getMatk())
			.add<stat_t>(item->getWdef())
			.add<stat_t>(item->getMdef())
			.add<stat_t>(item->getAccuracy())
			.add<stat_t>(item->getAvoid())
			.add<stat_t>(item->getHands())
			.add<stat_t>(item->getSpeed())
			.add<stat_t>(item->getJump())
			.add<string_t>(item->getName()) // Owner string
			.add<int16_t>(item->getFlags()); // Lock, shoe spikes, cape cold protection, etc.

		if (false) { //item->getCashId() != 0) {
			builder
				.addBytes("91174826F700") // Always the same for cash equips
				.unk<int32_t>();
		}
		else {
			builder
				.unk<int8_t>()
				.unk<int8_t>() // Item level
				.unk<int16_t>()
				.unk<int16_t>() // Item EXP of.. some sort
				.add<int32_t>(item->getHammers())
				.unk<int64_t>(-1);
		}
		builder
			.addBytes("0040E0FD3B374F01") // Always the same?
			.unk<int32_t>(-1);
	}
	else {
		builder
			.add<slot_qty_t>(item->getAmount())
			.add<string_t>(item->getName()) // Specially made by <IGN>
			.add<int16_t>(item->getFlags());

		if (GameLogicUtilities::isRechargeable(item->getId())) {
			builder.add<int64_t>(0); // Might be rechargeable ID for internal tracking/duping tracking
		}
	}
	return builder;
}

PACKET_IMPL(addPlayerDisplay, Vana::Player *player) {
	PacketBuilder builder;
	builder
		.add<gender_id_t>(player->getGender())
		.add<skin_id_t>(player->getSkin())
		.add<face_id_t>(player->getFace())
		.unk<int8_t>(1)
		.add<hair_id_t>(player->getHair());

	player->getInventory()->addEquippedPacket(builder);
	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			builder.add<item_id_t>(pet->getItemId());
		}
		else {
			builder.add<item_id_t>(0);
		}
	}
	return builder;
}

}
}
}