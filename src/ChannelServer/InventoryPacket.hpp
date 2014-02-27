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
#pragma once

#include "PacketBuilder.hpp"
#include "SplitPacketBuilder.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

class Item;
class Player;

struct InventoryPacketOperation {
	InventoryPacketOperation() = delete;
	InventoryPacketOperation(int8_t operationType, Item *item, int16_t currentSlot, int16_t oldSlot = 0) :
		operationType(operationType),
		item(item),
		currentSlot(currentSlot),
		oldSlot(oldSlot)
	{
	}

	int8_t operationType = 0;
	Item *item = nullptr;
	inventory_slot_t oldSlot = 0;
	inventory_slot_t currentSlot = 0;
};

namespace InventoryPacket {
	namespace RockModes {
		enum Modes : int8_t {
			Delete = 0x02,
			Add = 0x03
		};
	}
	namespace RockErrors {
		enum Errors : int8_t {
			Unk = 0x02, // Causes error 38 with current data
			Unk2 = 0x03, // Causes error 38 with current data
			CannotGo2 = 0x05, // This is unused
			DifficultToLocate = 0x06,
			DifficultToLocate2 = 0x07, // This is unused
			CannotGo = 0x08,
			AlreadyThere = 0x09,
			CannotSaveMap = 0x0A,
			NoobsCannotLeaveMapleIsland = 0x0B, // "Users below level 7 are not allowed to go out from Maple Island."
		};
	}
	namespace RockTypes {
		enum Types : int8_t {
			Regular = 0x00,
			Vip = 0x01,
		};
	}
	namespace OperationTypes {
		enum Types : int8_t {
			AddItem = 0x00,
			ModifyQuantity = 0x01,
			ModifySlot = 0x02,
			RemoveItem = 0x03,
		};
	}

	SPLIT_PACKET(updatePlayer, Player *player);
	SPLIT_PACKET(sitChair, player_id_t playerId, item_id_t chairId);
	SPLIT_PACKET(stopChair, player_id_t playerId, bool seatTaken);
	SPLIT_PACKET(useScroll, player_id_t playerId, int8_t succeed, bool destroy, bool legendarySpirit);
	SPLIT_PACKET(sendChalkboardUpdate, player_id_t playerId, const string_t &msg);
	SPLIT_PACKET(useSkillbook, player_id_t playerId, skill_id_t skillId, int32_t newMaxLevel, bool use, bool succeed);
	SPLIT_PACKET(useItemEffect, player_id_t playerId, item_id_t itemId);
	SPLIT_PACKET(sendRewardItemAnimation, player_id_t playerId, item_id_t itemId, const string_t &effect);
	PACKET(inventoryOperation, bool unk, const vector_t<InventoryPacketOperation> &operations);
	PACKET(sitMapChair, seat_id_t chairId);
	PACKET(showMegaphone, const string_t &msg);
	PACKET(showSuperMegaphone, const string_t &msg, bool whisper = false);
	PACKET(showMessenger, const string_t &playerName, const string_t &msg1, const string_t &msg2, const string_t &msg3, const string_t &msg4, unsigned char *displayInfo, int32_t displayInfoSize, item_id_t itemId);
	PACKET(showItemMegaphone, const string_t &msg, bool whisper = false, Item *item = nullptr);
	PACKET(showTripleMegaphone, int8_t lines, const string_t &line1, const string_t &line2, const string_t &line3, bool whisper);
	PACKET(updateSlots, inventory_t inventory, inventory_slot_count_t slots);
	PACKET(sendRockUpdate, int8_t mode, int8_t type, const vector_t<map_id_t> &maps);
	PACKET(sendRockError, int8_t code, int8_t type);
	PACKET(useCharm, uint8_t charmsLeft, uint8_t daysLeft = 99);
	PACKET(sendMesobagSucceed, mesos_t mesos);
	PACKET(sendMesobagFailed);
	PACKET(sendHammerSlots, int32_t slots);
	PACKET(sendHulkSmash, inventory_slot_t slot, Item *hammered);
	PACKET(sendHammerUpdate);
	PACKET(playCashSong, item_id_t itemId, const string_t &playerName);
	PACKET(blankUpdate);
	PACKET(sendItemExpired, const vector_t<item_id_t> &items);
}