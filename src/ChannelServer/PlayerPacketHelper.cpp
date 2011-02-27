/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "Player.h"
#include <cmath>

void PlayerPacketHelper::addItemInfo(PacketCreator &packet, int16_t slot, Item *item, bool shortSlot) {
	if (slot != 0) {
		if (shortSlot)
			packet.add<int16_t>(slot);
		else {
			slot = abs(slot);
			if (slot > 100) slot -= 100;
			packet.add<int8_t>((int8_t) slot);
		}
	}
	bool equip = GameLogicUtilities::isEquip(item->id);
	packet.add<int8_t>(equip ? 1 : 2);
	packet.add<int32_t>(item->id);
	packet.add<int8_t>(0);
	packet.add<int64_t>(Items::NoExpiration); // Expiration, I think
	if (equip) {
		packet.add<int8_t>(item->slots); // Slots
		packet.add<int8_t>(item->scrolls); // Scrolls
		packet.add<int16_t>(item->istr); // STR
		packet.add<int16_t>(item->idex); // DEX
		packet.add<int16_t>(item->iint); // INT
		packet.add<int16_t>(item->iluk); // LUK
		packet.add<int16_t>(item->ihp); // HP
		packet.add<int16_t>(item->imp); // MP
		packet.add<int16_t>(item->iwatk); // W.Atk
		packet.add<int16_t>(item->imatk); // M.Atk
		packet.add<int16_t>(item->iwdef); // W.Def
		packet.add<int16_t>(item->imdef); // M.Def
		packet.add<int16_t>(item->iacc); // Acc
		packet.add<int16_t>(item->iavo); // Avo
		packet.add<int16_t>(item->ihand); // Hands
		packet.add<int16_t>(item->ispeed); // Speed
		packet.add<int16_t>(item->ijump); // Jump
		packet.addString(item->name); // Owner string
		packet.add<int16_t>(item->flags); // Lock, shoe spikes, cape cold protection, etc.
		packet.add<int8_t>(0);
		packet.add<int8_t>(0); // Item level
		packet.add<int16_t>(0);
		packet.add<int16_t>(0); // Item EXP of.. some sort
		packet.add<int32_t>(item->hammers); // Vicious' Hammer
		packet.add<int64_t>(-1);
		packet.addBytes("0040E0FD3B374F01"); // Always the same?
		packet.add<int32_t>(-1);
	}
	else {
		packet.add<int16_t>(item->amount); // Amount
		packet.addString(item->name); // Specially made by <IGN>
		packet.add<int16_t>(item->flags);
		if (GameLogicUtilities::isRechargeable(item->id)) {
			packet.add<int64_t>(0); // Might be rechargeable ID for internal tracking/duping tracking
		}
	}
}

void PlayerPacketHelper::addPlayerDisplay(PacketCreator &packet, Player *player) {
	packet.add<int8_t>(player->getGender());
	packet.add<int8_t>(player->getSkin());
	packet.add<int32_t>(player->getEyes());
	packet.add<int8_t>(1);
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
}
