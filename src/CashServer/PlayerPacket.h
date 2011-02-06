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
#pragma once

#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class CashItem;
class KeyMaps;
class PacketCreator;
class Player;
class SkillMacros;
class Item;

namespace PlayerPacket {
	namespace BlockMessages {
		enum Messages {
			CannotGo = 0x01,
			NoCashShop = 0x02,
			MtsUnavailable = 0x03,
			MtsUserLimit = 0x04,
			LevelTooLow = 0x05
		};
	}
	namespace NoticeTypes {
		enum Types {
			Notice = 0x00,
			Box = 0x01,
			Red = 0x05,
			Blue = 0x06
		};
	}
	void connectData(Player *player);
	void changeChannel(Player *player, uint32_t ip, uint16_t port);
	void disconnectPlayer(Player *player);
	void sendBlockedMessage(Player *player, int8_t type);
	void showNX(Player *player, bool update = false);
	void showWishList(Player *player, bool update = false);
	void showCashInventory(Player *player);
	void sendWishListFailed(Player *player);
	void showBoughtItem(Player *player, CashItem *item);
	void showGiftItems(Player *player);
	void sendItemMovedToInventory(Player *player, Item *item, int16_t pos);
	void sendItemMovedToStorage(Player *player, CashItem *item);
	void showFailure(Player *player, uint8_t reason);
	void sendIncreasedInventorySlots(Player *player, int8_t inventory, int8_t slots);
	void sendIncreasedStorageSlots(Player *player, int8_t slots);
	void sendSentItem(Player *player, const string &receiver, int32_t itemid, int32_t cashSpent);
	void sendBoughtPackage(Player *player, vector<CashItem *> *items);
	void sendGotCouponRewards(Player *player, vector<CashItem *> items, int32_t mesos, int32_t maplePoints);
	void sendItemExpired(Player *player, int64_t cashId);
	void sendBoughtNonCashItem(Player *player, int16_t amount, int16_t slot, int32_t itemid);
	void sendChangeNameResult(Player *player, const string &name, int8_t result);
};
