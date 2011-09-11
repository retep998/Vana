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
#include "Trade.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Trades.h"
#include "VersionConstants.h"

ActiveTrade::ActiveTrade(Player *sender, Player *receiver, int32_t id) :
	m_sender(new TradeInfo()),
	m_receiver(new TradeInfo()),
	m_id(id)
{
	sender->setTrading(true);
	receiver->setTrading(false);
	sender->setTradeId(id);
	receiver->setTradeId(id);
	m_senderId = sender->getId();
	m_receiverId = receiver->getId();
}

bool ActiveTrade::bothCanTrade() {
	if (!canTrade(getSender(), getReceiverTrade())) {
		return false;
	}
	if (!canTrade(getReceiver(), getSenderTrade())) {
		return false;
	}
	return true;
}

bool ActiveTrade::canTrade(Player *target, TradeInfo *unit) {
	bool yes = true;
	int32_t cmesos = unit->mesos + target->getInventory()->getMesos();
	if (cmesos < 0) {
		yes = false;
	}
	if (yes && unit->count > 0) {
		boost::array<int8_t, Inventories::InventoryCount> totals = {0};
		unordered_map<int32_t, int16_t> added;
		for (uint8_t i = 0; i < TradeInfo::TradeSize; i++) {
			// Create item structure to determine needed slots among stackable items
			// Also, determine needed slots for nonstackables
			if (unit->slot[i]) {
				Item *check = unit->items[i];
				int32_t itemId = check->getId();
				int8_t inv = GameLogicUtilities::getInventory(itemId);
				if (!GameLogicUtilities::isStackable(itemId)) {
					// No need to clutter unordered map
					totals[inv - 1]++;
				}
				else {
					if (added.find(itemId) != added.end()) {
						// Already initialized this item
						added[itemId] += check->getAmount();
					}
					else {
						added[itemId] = check->getAmount();
					}
				}
			}
		}
		for (uint8_t i = 0; i < TradeInfo::TradeSize; i++) {
			// Determine precisely how many slots are needed for stackables
			if (unit->slot[i]) {
				Item *check = unit->items[i];
				int32_t itemId = check->getId();
				int8_t inv = GameLogicUtilities::getInventory(itemId);
				if (GameLogicUtilities::isStackable(itemId)) {
					// Already did these
					if (added.find(itemId) == added.end()) {
						// Already did this item
						continue;
					}
					int16_t maxSlot = ItemDataProvider::Instance()->getMaxSlot(itemId);
					int32_t currentAmount = target->getInventory()->getItemAmount(itemId);
					int32_t lastSlot = (currentAmount % maxSlot); // Get the number of items in the last slot
					int32_t itemSum = lastSlot + added[itemId];
					bool needSlots = false;
					if (lastSlot > 0) {
						// Items in the last slot, potential for needing slots
						if (itemSum > maxSlot) {
							needSlots = true;
						}
					}
					else {
						// Full in the last slot, for sure need all slots
						needSlots = true;
					}
					if (needSlots) {
						uint8_t numSlots = static_cast<uint8_t>(itemSum / maxSlot);
						uint8_t remainder = static_cast<uint8_t>(itemSum % maxSlot);
						if (remainder > 0) {
							totals[inv - 1]++;
						}
						totals[inv - 1] += numSlots;
					}
					added.erase(itemId);
				}
			}
		}
		for (uint8_t i = 0; i < Inventories::InventoryCount; i++) {
			// Determine if needed slots are available
			if (totals[i] > 0) {
				int8_t incrementor = 0;
				for (int8_t g = 1; g <= target->getInventory()->getMaxSlots(i + 1); g++) {
					if (target->getInventory()->getItem(i + 1, g) == nullptr) {
						incrementor++;
					}
					if (incrementor >= totals[i]) {
						break;
					}
				}
				if (incrementor < totals[i]) {
					yes = false;
					break;
				}
			}
		}
	}
	return yes;
}

void ActiveTrade::giveItems(Player *player, TradeInfo *info) {
	if (info->count > 0) {
		for (uint8_t i = 0; i < TradeInfo::TradeSize; i++) {
			if (info->slot[i]) {
				Item *item = info->items[i];
				if (item->hasKarma()) {
					item->setKarma(false);
					item->setTradeBlock(true);
				}
				Inventory::addItem(player, new Item(item));
				delete item;
			}
		}
	}
}

void ActiveTrade::giveMesos(Player *player, TradeInfo *info, bool traded) {
	if (info->mesos > 0) {
		int32_t taxlevel = TradeHandler::getTaxLevel(info->mesos);
		if (traded && taxlevel != 0) {
			int64_t mesos = info->mesos * taxlevel / 10000;
			info->mesos -= static_cast<int32_t>(mesos);
		}
		player->getInventory()->modifyMesos(info->mesos);
	}
}

void ActiveTrade::returnTrade() {
	TradeInfo *send = getSenderTrade();
	TradeInfo *recv = getReceiverTrade();
	Player *one = getSender();
	Player *two = getReceiver();
	if (one != nullptr) {
		giveItems(one, send);
		giveMesos(one, send);
	}
	if (two != nullptr) {
		giveItems(two, recv);
		giveMesos(two, recv);
	}
}

void ActiveTrade::swapTrade() {
	TradeInfo *send = getSenderTrade();
	TradeInfo *recv = getReceiverTrade();
	Player *one = getSender();
	Player *two = getReceiver();
	giveItems(one, recv);
	giveItems(two, send);
	giveMesos(one, recv, true);
	giveMesos(two, send, true);
}

bool ActiveTrade::bothAccepted() {
	return (getSenderTrade()->accepted && getReceiverTrade()->accepted);
}

void ActiveTrade::accept(TradeInfo *unit) {
	unit->accepted = true;
}

int32_t ActiveTrade::addMesos(Player *holder, TradeInfo *unit, int32_t amount) {
	unit->mesos += amount;
	holder->getInventory()->modifyMesos(-amount, true);
	return unit->mesos;
}

Item * ActiveTrade::addItem(Player *holder, TradeInfo *unit, Item *item, uint8_t tradeSlot, int16_t inventorySlot, int8_t inventory, int16_t amount) {
	Item *use = new Item(item);
	if (amount == item->getAmount() || GameLogicUtilities::isEquip(item->getId())) {
		holder->getInventory()->setItem(inventory, inventorySlot, nullptr);
		InventoryPacket::moveItem(holder, inventory, inventorySlot, 0);
		holder->getInventory()->deleteItem(inventory, inventorySlot);
	}
	else {
		item->decAmount(amount);
		holder->getInventory()->changeItemAmount(item->getId(), item->getAmount());
		InventoryPacket::updateItemAmounts(holder, inventory, inventorySlot, item->getAmount(), 0, 0);
		use->setAmount(amount);
	}
	InventoryPacket::blankUpdate(holder); // Should prevent locking up in .70, don't know why it locks
	unit->count++;
	uint8_t index = tradeSlot - 1;
	unit->items[index] = use;
	unit->slot[index] = true;
	return use;
}

Player * ActiveTrade::getSender() {
	return PlayerDataProvider::Instance()->getPlayer(m_senderId);
}

Player * ActiveTrade::getReceiver() {
	return PlayerDataProvider::Instance()->getPlayer(m_receiverId);
}