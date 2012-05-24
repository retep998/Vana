/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "PlayerPacketHelper.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "ItemConstants.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "Player.h"
#include "TimeUtilities.h"
#include <cmath>

void PlayerPacketHelper::addItemInfo(PacketCreator &packet, int16_t slot, Item *item, bool shortSlot) {
	slot = abs(slot);
	if (slot != 0) {
		if (shortSlot) {
			packet.add<int16_t>(slot);
		}
		else {
			slot = abs(slot);
			if (slot > 100) slot -= 100;
			packet.add<int8_t>((int8_t) slot);
		}
	}
	bool equip = GameLogicUtilities::isEquip(item->getId());
	packet.add<int8_t>(equip ? 1 : 2);
	packet.add<int32_t>(item->getId());
	if (false) { //item->getCashId() != 0) {
		packet.add<int8_t>(1);
		packet.add<int64_t>(0); //item->getCashId());
	}
	else {
		packet.add<int8_t>(0);
	}
	packet.add<int64_t>(item->getExpirationTime());
	packet.add<uint32_t>(0xFFFFFFFF);
	if (equip) {
		packet.add<int8_t>(item->getSlots());
		packet.add<int8_t>(item->getScrolls());
		packet.add<int16_t>(item->getStr());
		packet.add<int16_t>(item->getDex());
		packet.add<int16_t>(item->getInt());
		packet.add<int16_t>(item->getLuk());
		packet.add<int16_t>(item->getHp());
		packet.add<int16_t>(item->getMp());
		packet.add<int16_t>(item->getWatk());
		packet.add<int16_t>(item->getMatk());
		packet.add<int16_t>(item->getWdef());
		packet.add<int16_t>(item->getMdef());
		packet.add<int16_t>(item->getAccuracy());
		packet.add<int16_t>(item->getAvoid());
		packet.add<int16_t>(item->getHands());
		packet.add<int16_t>(item->getSpeed());
		packet.add<int16_t>(item->getJump());
		packet.addString(item->getName()); // Owner string
		packet.add<int16_t>(item->getFlags()); // Lock, shoe spikes, cape cold protection, etc.

		packet.add<int8_t>(0);
		packet.add<int8_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(item->getHammers());
		packet.add<int16_t>(0);
		packet.add<int8_t>(1); // Is a potential
		packet.add<int8_t>(6); // Amount of stars
		packet.add<int16_t>(0); // Rare
		packet.add<int16_t>(0); // Epic
		packet.add<int16_t>(0); // Unique
		packet.add<int16_t>(0); // Legendary
		packet.add<int16_t>(0); // HP Rate
		packet.add<int16_t>(0); // MP Rate
		packet.add<int16_t>(-1);
		packet.add<int16_t>(-1);
		packet.add<int16_t>(-1);
		/*
		if (false) { //item->getCashId() != 0) {
			packet.addBytes("91174826F700"); // Always the same for cash equips
			packet.add<int32_t>(0);
		}
		else {
			packet.add<int8_t>(0);
			packet.add<int8_t>(0); // Item level
			packet.add<int16_t>(0);
			packet.add<int16_t>(0); // Item EXP of.. some sort
			packet.add<int32_t>(item->getHammers()); // Vicious' Hammer
			packet.add<int64_t>(-1);
		}
		*/

		packet.add<int64_t>(-1);

		packet.addBytes("0040E0FD3B374F01"); // Always the same?
		packet.add<int32_t>(-1);
	}
	else {
		packet.add<int16_t>(item->getAmount()); // Amount
		packet.addString(item->getName()); // Specially made by <IGN>
		packet.add<int16_t>(item->getFlags());
		if (GameLogicUtilities::isRechargeable(item->getId())) {
			packet.add<int64_t>(0); // Might be rechargeable ID for internal tracking/duping tracking
		}
	}
}

void PlayerPacketHelper::addPlayerDisplay(PacketCreator &packet, Player *player) {
	int16_t jobid = player->getStats()->getJob();
	packet.add<int8_t>(player->getGender());
	packet.add<int8_t>(player->getSkin());
	packet.add<int32_t>(player->getEyes());
	packet.add<int32_t>(jobid); // New
	packet.add<int8_t>(0);
	packet.add<int32_t>(player->getHair());
	player->getInventory()->addEquippedPacket(packet);
	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			packet.add<int32_t>(pet->getItemId());
		}
		else {
			packet.add<int32_t>(0);
		}
	}
	

	if (jobid / 100 == Jobs::JobTracks::DemonSlayer || jobid == Jobs::JobIds::DemonSlayer) {
		packet.add<int32_t>(1012280);
	}
}