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
#include "Trade.hpp"
#include "ChannelServer.hpp"
#include "GameLogicUtilities.hpp"
#include "Inventory.hpp"
#include "InventoryPacket.hpp"
#include "ItemDataProvider.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "TradeHandler.hpp"
#include "Trades.hpp"

ActiveTrade::ActiveTrade(Player *sender, Player *receiver, trade_id_t id) :
	m_id{id}
{
	m_sender = make_owned_ptr<TradeInfo>();
	m_receiver = make_owned_ptr<TradeInfo>();

	sender->setTrading(true);
	receiver->setTrading(false);
	sender->setTradeId(id);
	receiver->setTradeId(id);
	m_senderId = sender->getId();
	m_receiverId = receiver->getId();
}

auto ActiveTrade::bothCanTrade() -> bool {
	if (!canTrade(getSender(), getReceiverTrade())) {
		return false;
	}
	if (!canTrade(getReceiver(), getSenderTrade())) {
		return false;
	}
	return true;
}

auto ActiveTrade::canTrade(Player *target, TradeInfo *unit) -> bool {
	bool canTrade = true;
	mesos_t currentMesos = unit->mesos + target->getInventory()->getMesos();
	if (currentMesos < 0) {
		canTrade = false;
	}
	if (canTrade && unit->count > 0) {
		array_t<trade_slot_t, Inventories::InventoryCount> totals = {0};
		hash_map_t<item_id_t, slot_qty_t> added;
		for (trade_slot_t i = 0; i < TradeInfo::TradeSize; ++i) {
			// Create item structure to determine needed slots among stackable items
			// Also, determine needed slots for nonstackables
			if (unit->items[i] != nullptr) {
				Item *check = unit->items[i];
				item_id_t itemId = check->getId();
				inventory_t inv = GameLogicUtilities::getInventory(itemId);
				if (!GameLogicUtilities::isStackable(itemId)) {
					// No need to clutter unordered map
					totals[inv - 1]++;
				}
				else {
					if (added.find(itemId) != std::end(added)) {
						// Already initialized this item
						added[itemId] += check->getAmount();
					}
					else {
						added[itemId] = check->getAmount();
					}
				}
			}
		}
		for (trade_slot_t i = 0; i < TradeInfo::TradeSize; ++i) {
			// Determine precisely how many slots are needed for stackables
			if (unit->items[i] != nullptr) {
				Item *check = unit->items[i];
				item_id_t itemId = check->getId();
				inventory_t inv = GameLogicUtilities::getInventory(itemId);
				if (GameLogicUtilities::isStackable(itemId)) {
					// Already did these
					if (added.find(itemId) == std::end(added)) {
						// Already did this item
						continue;
					}
					auto itemInfo = ChannelServer::getInstance().getItemDataProvider().getItemInfo(itemId);
					slot_qty_t maxSlot = itemInfo->maxSlot;
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
						trade_slot_t numSlots = static_cast<trade_slot_t>(itemSum / maxSlot);
						trade_slot_t remainder = static_cast<trade_slot_t>(itemSum % maxSlot);
						if (remainder > 0) {
							totals[inv - 1]++;
						}
						totals[inv - 1] += numSlots;
					}
					added.erase(itemId);
				}
			}
		}
		for (inventory_t i = 0; i < Inventories::InventoryCount; ++i) {
			// Determine if needed slots are available
			if (totals[i] > 0) {
				trade_slot_t incrementor = 0;
				for (inventory_slot_count_t g = 1; g <= target->getInventory()->getMaxSlots(i + 1); ++g) {
					if (target->getInventory()->getItem(i + 1, g) == nullptr) {
						incrementor++;
					}
					if (incrementor >= totals[i]) {
						break;
					}
				}
				if (incrementor < totals[i]) {
					canTrade = false;
					break;
				}
			}
		}
	}
	return canTrade;
}

auto ActiveTrade::giveItems(Player *player, TradeInfo *info) -> void {
	if (info->count > 0) {
		for (trade_slot_t i = 0; i < TradeInfo::TradeSize; ++i) {
			if (info->items[i] != nullptr) {
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

auto ActiveTrade::giveMesos(Player *player, TradeInfo *info, bool traded) -> void {
	if (info->mesos > 0) {
		int32_t taxLevel = TradeHandler::getTaxLevel(info->mesos);
		if (traded && taxLevel != 0) {
			int64_t mesos = info->mesos * taxLevel / 10000;
			info->mesos -= static_cast<mesos_t>(mesos);
		}
		player->getInventory()->modifyMesos(info->mesos);
	}
}

auto ActiveTrade::returnTrade() -> void {
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

auto ActiveTrade::swapTrade() -> void {
	TradeInfo *send = getSenderTrade();
	TradeInfo *recv = getReceiverTrade();
	Player *one = getSender();
	Player *two = getReceiver();
	giveItems(one, recv);
	giveItems(two, send);
	giveMesos(one, recv, true);
	giveMesos(two, send, true);
}

auto ActiveTrade::bothAccepted() -> bool {
	return getSenderTrade()->accepted && getReceiverTrade()->accepted;
}

auto ActiveTrade::accept(TradeInfo *unit) -> void {
	unit->accepted = true;
}

auto ActiveTrade::addMesos(Player *holder, TradeInfo *unit, mesos_t amount) -> mesos_t {
	unit->mesos += amount;
	holder->getInventory()->modifyMesos(-amount, true);
	return unit->mesos;
}

auto ActiveTrade::addItem(Player *holder, TradeInfo *unit, Item *item, trade_slot_t tradeSlot, inventory_slot_t inventorySlot, inventory_t inventory, slot_qty_t amount) -> Item * {
	Item *use = new Item(item);
	if (amount == item->getAmount() || GameLogicUtilities::isEquip(item->getId())) {
		holder->getInventory()->setItem(inventory, inventorySlot, nullptr);

		vector_t<InventoryPacketOperation> ops;
		ops.emplace_back(InventoryPacket::OperationTypes::ModifySlot, item, inventorySlot);
		holder->send(InventoryPacket::inventoryOperation(true, ops));

		holder->getInventory()->deleteItem(inventory, inventorySlot);
	}
	else {
		item->decAmount(amount);
		holder->getInventory()->changeItemAmount(item->getId(), item->getAmount());

		vector_t<InventoryPacketOperation> ops;
		ops.emplace_back(InventoryPacket::OperationTypes::ModifyQuantity, item, inventorySlot);
		holder->send(InventoryPacket::inventoryOperation(true, ops));

		use->setAmount(amount);
	}
	holder->send(InventoryPacket::blankUpdate()); // Should prevent locking up in .70, don't know why it locks
	unit->count++;
	trade_slot_t index = tradeSlot - 1;
	unit->items[index] = use;
	return use;
}

auto ActiveTrade::getSender() -> Player * {
	return ChannelServer::getInstance().getPlayerDataProvider().getPlayer(m_senderId);
}

auto ActiveTrade::getReceiver() -> Player * {
	return ChannelServer::getInstance().getPlayerDataProvider().getPlayer(m_receiverId);
}