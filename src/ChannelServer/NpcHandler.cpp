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
#include "NpcHandler.hpp"
#include "GameLogicUtilities.hpp"
#include "Inventory.hpp"
#include "InventoryPacket.hpp"
#include "ItemDataProvider.hpp"
#include "Map.hpp"
#include "Maps.hpp"
#include "Npc.hpp"
#include "NpcDataProvider.hpp"
#include "NpcPacket.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "ShopDataProvider.hpp"
#include "StoragePacket.hpp"

namespace ShopOpcodes {
	enum Opcodes : int8_t {
		Buy = 0x00,
		Sell = 0x01,
		Recharge = 0x02,
		ExitShop = 0x03,
		// Storage
		TakeItem = 0x04,
		StoreItem = 0x05,
		MesoTransaction = 0x07,
		ExitStorage = 0x08
	};
}

auto NpcHandler::handleNpc(Player *player, PacketReader &reader) -> void {
	if (player->getNpc() != nullptr) {
		return;
	}
	uint32_t npcId = Map::makeNpcId(reader.get<uint32_t>());

	if (!player->getMap()->isValidNpcIndex(npcId)) {
		// Shouldn't ever happen except in edited packets
		return;
	}

	NpcSpawnInfo npcs = player->getMap()->getNpc(npcId);
	if (player->getNpc() == nullptr && Npc::hasScript(npcs.id, 0, false)) {
		Npc *npc = new Npc(npcs.id, player, npcs.pos);
		npc->run();
		return;
	}
	if (player->getShop() == 0) {
		if (NpcHandler::showShop(player, npcs.id)) {
			return;
		}
		if (NpcHandler::showStorage(player, npcs.id)) {
			return;
		}
		if (NpcHandler::showGuildRank(player, npcs.id)) {
			return;
		}
	}
}

auto NpcHandler::handleQuestNpc(Player *player, int32_t npcId, bool start, int16_t questId) -> void {
	if (player->getNpc() != nullptr) {
		return;
	}

	Npc *npc = new Npc(npcId, player, questId, start);
	npc->run();
}

auto NpcHandler::handleNpcIn(Player *player, PacketReader &reader) -> void {
	Npc *npc = player->getNpc();
	if (npc == nullptr) {
		return;
	}

	int8_t type = reader.get<int8_t>();
	if (type != npc->getSentDialog()) {
		// Hacking
		return;
	}

	if (type == NpcPacket::Dialogs::Quiz || type == NpcPacket::Dialogs::Question) {
		npc->proceedText(reader.get<string_t>());
		npc->checkEnd();
		return;
	}

	int8_t what = reader.get<int8_t>();

	switch (type) {
		case NpcPacket::Dialogs::Normal:
			switch (what) {
				case 0: npc->proceedBack(); break;
				case 1:	npc->proceedNext(); break;
				default: npc->end(); break;
			}
			break;
		case NpcPacket::Dialogs::YesNo:
		case NpcPacket::Dialogs::AcceptDecline:
		case NpcPacket::Dialogs::AcceptDeclineNoExit:
			switch (what) {
				case 0: npc->proceedSelection(0); break;
				case 1:	npc->proceedSelection(1); break;
				default: npc->end(); break;
			}
			break;
		case NpcPacket::Dialogs::GetText:
			if (what != 0) {
				npc->proceedText(reader.get<string_t>());
			}
			else {
				npc->end();
			}
			break;
		case NpcPacket::Dialogs::GetNumber:
			if (what == 1) {
				npc->proceedNumber(reader.get<int32_t>());
			}
			else {
				npc->end();
			}
			break;
		case NpcPacket::Dialogs::Simple:
			if (what == 0) {
				npc->end();
			}
			else {
				npc->proceedSelection(reader.get<uint8_t>());
			}
			break;
		case NpcPacket::Dialogs::Style:
			if (what == 1) {
				npc->proceedSelection(reader.get<uint8_t>());
			}
			else {
				npc->end();
			}
			break;
		default:
			npc->end();
	}
	npc->checkEnd();
}

auto NpcHandler::handleNpcAnimation(Player *player, PacketReader &reader) -> void {
	player->send(NpcPacket::animateNpc(reader));
}

auto NpcHandler::useShop(Player *player, PacketReader &reader) -> void {
	if (player->getShop() == 0) {
		// Hacking
		return;
	}
	int8_t type = reader.get<int8_t>();
	switch (type) {
		case ShopOpcodes::Buy: {
			uint16_t itemIndex = reader.get<uint16_t>();
			reader.skipBytes(4); // Item ID, no reason to trust this
			uint16_t quantity = reader.get<uint16_t>();
			reader.skipBytes(4); // Price, don't want to trust this
			auto shopItem = ShopDataProvider::getInstance().getShopItem(player->getShop(), itemIndex);
			if (shopItem == nullptr) {
				// Hacking
				return;
			}

			int16_t amount = shopItem->quantity;
			int32_t itemId = shopItem->itemId;
			int32_t price = shopItem->price;
			uint32_t totalAmount = quantity * amount; // The game doesn't let you purchase more than 1 slot worth of items; if they're grouped, it buys them in single units, if not, it only allows you to go up to maxSlot
			int32_t totalPrice = quantity * price;
			auto itemInfo = ItemDataProvider::getInstance().getItemInfo(itemId);

			if (price == 0 || totalAmount > itemInfo->maxSlot || player->getInventory()->getMesos() < totalPrice) {
				// Hacking
				player->send(NpcPacket::bought(NpcPacket::BoughtMessages::NotEnoughMesos));
				return;
			}
			bool haveSlot = player->getInventory()->hasOpenSlotsFor(itemId, static_cast<int16_t>(totalAmount), true);
			if (!haveSlot) {
				player->send(NpcPacket::bought(NpcPacket::BoughtMessages::NoSlots));
				return;
			}
			Inventory::addNewItem(player, itemId, static_cast<int16_t>(totalAmount));
			player->getInventory()->modifyMesos(-totalPrice);
			player->send(NpcPacket::bought(NpcPacket::BoughtMessages::Success));
			break;
		}
		case ShopOpcodes::Sell: {
			int16_t slot = reader.get<int16_t>();
			int32_t itemId = reader.get<int32_t>();
			int16_t amount = reader.get<int16_t>();
			int8_t inv = GameLogicUtilities::getInventory(itemId);
			Item *item = player->getInventory()->getItem(inv, slot);
			if (item == nullptr || item->getId() != itemId || (!GameLogicUtilities::isRechargeable(itemId) && amount > item->getAmount())) {
				// Hacking
				player->send(NpcPacket::bought(NpcPacket::BoughtMessages::NotEnoughInStock));
				return;
			}
			int32_t price = ItemDataProvider::getInstance().getItemInfo(itemId)->price;

			player->getInventory()->modifyMesos(price * amount);
			if (GameLogicUtilities::isRechargeable(itemId)) {
				Inventory::takeItemSlot(player, inv, slot, item->getAmount(), true);
			}
			else {
				Inventory::takeItemSlot(player, inv, slot, amount, true);
			}
			player->send(NpcPacket::bought(NpcPacket::BoughtMessages::Success));
			break;
		}
		case ShopOpcodes::Recharge: {
			int16_t slot = reader.get<int16_t>();
			Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
			if (item == nullptr || !GameLogicUtilities::isRechargeable(item->getId())) {
				// Hacking
				return;
			}

			auto itemInfo = ItemDataProvider::getInstance().getItemInfo(item->getId());
			uint16_t maxSlot = itemInfo->maxSlot;
			if (GameLogicUtilities::isRechargeable(item->getId())) {
				maxSlot += player->getSkills()->getRechargeableBonus();
			}
			int32_t modifiedMesos = ShopDataProvider::getInstance().getRechargeCost(player->getShop(), item->getId(), maxSlot - item->getAmount());
			if (modifiedMesos < 0 && player->getInventory()->getMesos() > -modifiedMesos) {
				player->getInventory()->modifyMesos(modifiedMesos);
				item->setAmount(maxSlot);

				vector_t<InventoryPacketOperation> ops;
				ops.emplace_back(InventoryPacket::OperationTypes::ModifyQuantity, item, slot);
				player->send(InventoryPacket::inventoryOperation(true, ops));

				player->send(NpcPacket::bought(NpcPacket::BoughtMessages::Success));
			}
			break;
		}
		case ShopOpcodes::ExitShop:
			player->setShop(0);
			break;
	}
}

auto NpcHandler::useStorage(Player *player, PacketReader &reader) -> void {
	if (player->getShop() == 0) {
		// Hacking
		return;
	}
	int8_t type = reader.get<int8_t>();
	int32_t cost = NpcDataProvider::getInstance().getStorageCost(player->getShop());
	if (cost == 0) {
		// Hacking
		return;
	}
	switch (type) {
		case ShopOpcodes::TakeItem: {
			int8_t inv = reader.get<int8_t>();
			int8_t slot = reader.get<int8_t>();
			Item *item = player->getStorage()->getItem(slot);
			if (item == nullptr) {
				// Hacking
				return;
			}
			Inventory::addItem(player, new Item(item));
			player->getStorage()->takeItem(slot);
			player->send(StoragePacket::takeItem(player, inv));
			break;
		}
		case ShopOpcodes::StoreItem: {
			int16_t slot = reader.get<int16_t>();
			int32_t itemId = reader.get<int32_t>();
			int16_t amount = reader.get<int16_t>();
			if (player->getInventory()->getMesos() < cost) {
				// Player doesn't have enough mesos to store this item
				player->send(StoragePacket::noMesos());
				return;
			}
			if (player->getStorage()->isFull()) {
				// Storage is full, so tell the player and abort the mission
				player->send(StoragePacket::storageFull());
				return;
			}
			int8_t inv = GameLogicUtilities::getInventory(itemId);
			Item *item = player->getInventory()->getItem(inv, slot);
			if (item == nullptr) {
				// Hacking
				return;
			}
			if (!GameLogicUtilities::isStackable(itemId)) {
				amount = 1;
			}
			else if (amount <= 0 || amount > item->getAmount()) {
				// Hacking
				return;
			}
			player->getStorage()->addItem(!GameLogicUtilities::isStackable(itemId) ? new Item(item) : new Item(itemId, amount));
			// For equips or rechargeable items (stars/bullets) we create a
			// new object for storage with the inventory object, and allow
			// the one in the inventory to go bye bye.
			// Else: For items we just create a new item based on the ID and amount.
			Inventory::takeItemSlot(player, inv, slot, GameLogicUtilities::isRechargeable(itemId) ? item->getAmount() : amount, true);
			player->getInventory()->modifyMesos(-cost);
			player->send(StoragePacket::addItem(player, inv));
			break;
		}
		case ShopOpcodes::MesoTransaction: {
			int32_t mesos = reader.get<int32_t>();
			// Amount of mesos to remove. Deposits are negative, and withdrawals are positive
			if (player->getInventory()->modifyMesos(mesos)) {
				player->getStorage()->changeMesos(mesos);
			}
			break;
		}
		case ShopOpcodes::ExitStorage:
			player->setShop(0);
			break;
	}
}

auto NpcHandler::showShop(Player *player, int32_t shopId) -> bool {
	if (ShopDataProvider::getInstance().isShop(shopId)) {
		player->setShop(shopId);
		player->send(NpcPacket::showShop(ShopDataProvider::getInstance().getShop(shopId), player->getSkills()->getRechargeableBonus()));
		return true;
	}
	return false;
}

auto NpcHandler::showStorage(Player *player, int32_t npcId) -> bool {
	if (NpcDataProvider::getInstance().getStorageCost(npcId)) {
		player->setShop(npcId);
		player->send(StoragePacket::showStorage(player, npcId));
		return true;
	}
	return false;
}

auto NpcHandler::showGuildRank(Player *player, int32_t npcId) -> bool {
	if (NpcDataProvider::getInstance().isGuildRank(npcId)) {
		// To be implemented later
	}
	return false;
}