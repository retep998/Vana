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
#include "NpcHandler.hpp"
#include "ChannelServer.hpp"
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
	size_t npcId = Map::makeNpcId(reader.get<map_object_t>());

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
		if (NpcHandler::showShop(player, npcs.id) == Result::Successful) {
			return;
		}
		if (NpcHandler::showStorage(player, npcs.id) == Result::Successful) {
			return;
		}
		if (NpcHandler::showGuildRank(player, npcs.id) == Result::Successful) {
			return;
		}
	}
}

auto NpcHandler::handleQuestNpc(Player *player, npc_id_t npcId, bool start, quest_id_t questId) -> void {
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

	int8_t choice = reader.get<int8_t>();

	switch (type) {
		case NpcPacket::Dialogs::Normal:
			switch (choice) {
				case 0: npc->proceedBack(); break;
				case 1:	npc->proceedNext(); break;
				default: npc->end(); break;
			}
			break;
		case NpcPacket::Dialogs::YesNo:
		case NpcPacket::Dialogs::AcceptDecline:
		case NpcPacket::Dialogs::AcceptDeclineNoExit:
			switch (choice) {
				case 0: npc->proceedSelection(0); break;
				case 1:	npc->proceedSelection(1); break;
				default: npc->end(); break;
			}
			break;
		case NpcPacket::Dialogs::GetText:
			if (choice != 0) {
				npc->proceedText(reader.get<string_t>());
			}
			else {
				npc->end();
			}
			break;
		case NpcPacket::Dialogs::GetNumber:
			if (choice == 1) {
				npc->proceedNumber(reader.get<int32_t>());
			}
			else {
				npc->end();
			}
			break;
		case NpcPacket::Dialogs::Simple:
			if (choice == 0) {
				npc->end();
			}
			else {
				npc->proceedSelection(reader.get<uint8_t>());
			}
			break;
		case NpcPacket::Dialogs::Style:
			if (choice == 1) {
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
			reader.skip<item_id_t>(); // No reason to trust this
			slot_qty_t quantity = reader.get<slot_qty_t>();
			reader.skip<mesos_t>(); // Price, don't want to trust this
			auto shopItem = ChannelServer::getInstance().getShopDataProvider().getShopItem(player->getShop(), itemIndex);
			if (shopItem == nullptr) {
				// Hacking
				return;
			}

			slot_qty_t amount = shopItem->quantity;
			item_id_t itemId = shopItem->itemId;
			mesos_t price = shopItem->price;
			slot_qty_t totalAmount = quantity * amount; // The game doesn't let you purchase more than 1 slot worth of items; if they're grouped, it buys them in single units, if not, it only allows you to go up to maxSlot
			mesos_t totalPrice = quantity * price;
			auto itemInfo = ChannelServer::getInstance().getItemDataProvider().getItemInfo(itemId);

			if (price == 0 || totalAmount > itemInfo->maxSlot || totalAmount < 0 || player->getInventory()->getMesos() < totalPrice) {
				// Hacking
				player->send(NpcPacket::bought(NpcPacket::BoughtMessages::NotEnoughMesos));
				return;
			}
			bool haveSlot = player->getInventory()->hasOpenSlotsFor(itemId, totalAmount, true);
			if (!haveSlot) {
				player->send(NpcPacket::bought(NpcPacket::BoughtMessages::NoSlots));
				return;
			}
			Inventory::addNewItem(player, itemId, totalAmount);
			player->getInventory()->modifyMesos(-totalPrice);
			player->send(NpcPacket::bought(NpcPacket::BoughtMessages::Success));
			break;
		}
		case ShopOpcodes::Sell: {
			inventory_slot_t slot = reader.get<inventory_slot_t>();
			item_id_t itemId = reader.get<item_id_t>();
			slot_qty_t amount = reader.get<slot_qty_t>();
			inventory_t inv = GameLogicUtilities::getInventory(itemId);
			Item *item = player->getInventory()->getItem(inv, slot);
			if (item == nullptr || item->getId() != itemId || (!GameLogicUtilities::isRechargeable(itemId) && amount > item->getAmount())) {
				// Hacking
				player->send(NpcPacket::bought(NpcPacket::BoughtMessages::NotEnoughInStock));
				return;
			}
			mesos_t price = ChannelServer::getInstance().getItemDataProvider().getItemInfo(itemId)->price;

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
			inventory_slot_t slot = reader.get<inventory_slot_t>();
			Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
			if (item == nullptr || !GameLogicUtilities::isRechargeable(item->getId())) {
				// Hacking
				return;
			}

			auto itemInfo = ChannelServer::getInstance().getItemDataProvider().getItemInfo(item->getId());
			slot_qty_t maxSlot = itemInfo->maxSlot;
			if (GameLogicUtilities::isRechargeable(item->getId())) {
				maxSlot += player->getSkills()->getRechargeableBonus();
			}
			mesos_t modifiedMesos = ChannelServer::getInstance().getShopDataProvider().getRechargeCost(player->getShop(), item->getId(), maxSlot - item->getAmount());
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
	mesos_t cost = ChannelServer::getInstance().getNpcDataProvider().getStorageCost(player->getShop());
	if (cost == 0) {
		// Hacking
		return;
	}
	switch (type) {
		case ShopOpcodes::TakeItem: {
			inventory_t inv = reader.get<inventory_t>();
			storage_slot_t slot = reader.get<storage_slot_t>();
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
			inventory_slot_t slot = reader.get<inventory_slot_t>();
			item_id_t itemId = reader.get<item_id_t>();
			slot_qty_t amount = reader.get<slot_qty_t>();
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
			inventory_t inv = GameLogicUtilities::getInventory(itemId);
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

			if (item->hasKarma()) {
				item->setKarma(false);
			}
			else {
				// TODO FIXME hacking
				// Must validate the Karma state of items here
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
			mesos_t mesos = reader.get<mesos_t>();
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

auto NpcHandler::showShop(Player *player, shop_id_t shopId) -> Result {
	if (ChannelServer::getInstance().getShopDataProvider().isShop(shopId)) {
		player->setShop(shopId);
		player->send(NpcPacket::showShop(ChannelServer::getInstance().getShopDataProvider().getShop(shopId), player->getSkills()->getRechargeableBonus()));
		return Result::Successful;
	}
	return Result::Failure;
}

auto NpcHandler::showStorage(Player *player, npc_id_t npcId) -> Result {
	if (ChannelServer::getInstance().getNpcDataProvider().getStorageCost(npcId)) {
		player->setShop(npcId);
		player->send(StoragePacket::showStorage(player, npcId));
		return Result::Successful;
	}
	return Result::Failure;
}

auto NpcHandler::showGuildRank(Player *player, npc_id_t npcId) -> Result {
	if (ChannelServer::getInstance().getNpcDataProvider().isGuildRank(npcId)) {
		// To be implemented later
	}
	return Result::Failure;
}