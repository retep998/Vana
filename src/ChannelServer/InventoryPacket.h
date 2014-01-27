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

#include "Types.h"
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
	int16_t oldSlot = 0;
	int16_t currentSlot = 0;
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

	auto updatePlayer(Player *player) -> void;
	auto inventoryOperation(Player *player, bool unk, const vector_t<InventoryPacketOperation> &operations) -> void;
	auto sitChair(Player *player, int32_t chairId) -> void;
	auto sitMapChair(Player *player, int16_t chairId) -> void;
	auto stopChair(Player *player, bool showMap = true) -> void;
	auto useSkillbook(Player *player, int32_t skillId, int32_t newMaxLevel, bool use, bool succeed) -> void;
	auto useScroll(Player *player, int8_t succeed, bool destroy, bool legendarySpirit) -> void;
	auto showMegaphone(Player *player, const string_t &msg) -> void;
	auto showSuperMegaphone(Player *player, const string_t &msg, bool whisper = false) -> void;
	auto showMessenger(Player *player, const string_t &msg, const string_t &msg2, const string_t &msg3, const string_t &msg4, unsigned char *displayInfo, int32_t displayInfoSize, int32_t itemId) -> void;
	auto showItemMegaphone(Player *player, const string_t &msg, bool whisper = false, Item *item = nullptr) -> void;
	auto showTripleMegaphone(Player *player, int8_t lines, const string_t &line1, const string_t &line2, const string_t &line3, bool whisper) -> void;
	auto useItemEffect(Player *player, int32_t itemId) -> void;
	auto updateSlots(Player *player, int8_t inventory, int8_t slots) -> void;
	auto sendRockUpdate(Player *player, int8_t mode, int8_t type, const vector_t<int32_t> &maps) -> void;
	auto sendRockError(Player *player, int8_t code, int8_t type) -> void;
	auto useCharm(Player *player, uint8_t charmsLeft, uint8_t daysLeft = 99) -> void;
	auto sendMesobagSucceed(Player *player, int32_t mesos) -> void;
	auto sendMesobagFailed(Player *player) -> void;
	auto sendHammerSlots(Player *player, int32_t slots) -> void;
	auto sendHulkSmash(Player *player, int16_t slot, Item *hammered) -> void;
	auto sendHammerUpdate(Player *player) -> void;
	auto sendChalkboardUpdate(Player *player, const string_t &msg = "") -> void;
	auto playCashSong(int32_t map, int32_t itemId, const string_t &playerName) -> void;
	auto sendRewardItemAnimation(Player *player, int32_t itemId, const string_t &effect) -> void;
	auto blankUpdate(Player *player) -> void;
	auto sendItemExpired(Player *player, const vector_t<int32_t> &items) -> void;
}