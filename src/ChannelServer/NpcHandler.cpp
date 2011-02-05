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
#include "NpcHandler.h"
#include "GameLogicUtilities.h"
#include "GuildPacket.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "Map.h"
#include "Maps.h"
#include "NpcDataProvider.h"
#include "NpcPacket.h"
#include "Npc.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "ShopDataProvider.h"
#include "StoragePacket.h"

namespace ShopOpcodes {
	enum Opcodes {
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

void NpcHandler::handleNpc(Player *player, PacketReader &packet) {
	if (player->getNpc()) {
		return;
	}

	uint32_t npcid = Map::makeNpcId(packet.get<uint32_t>());

	if (!Maps::getMap(player->getMap())->isValidNpcIndex(npcid)) {
		// Shouldn't ever happen except in edited packets
		return;
	}

	NpcSpawnInfo npcs = Maps::getMap(player->getMap())->getNpc(npcid);
	if (player->getNpc() == nullptr && Npc::hasScript(npcs.id, 0, false)) {
		Npc *npc = new Npc(npcs.id, player, npcs.pos);
		npc->run();
		return;
	}
	else if (player->getShop() == 0) {
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

void NpcHandler::handleQuestNpc(Player *player, int32_t npcid, bool start, int16_t questid) {
	if (player->getNpc()) {
		return;
	}

	Npc *npc = new Npc(npcid, player, questid, start);
	npc->run();
}

void NpcHandler::handleNpcIn(Player *player, PacketReader &packet) {
	Npc *npc = player->getNpc();
	if (npc == nullptr) {
		return;
	}

	int8_t type = packet.get<int8_t>();
	if (type != npc->getSentDialog()) {
		// Hacking
		return;
	}

	if (type == NpcDialogs::Quiz || type == NpcDialogs::Question) {
		npc->proceedText(packet.getString());
		npc->checkEnd();
		return;
	}

	int8_t what = packet.get<int8_t>();

	switch (type) {
		case NpcDialogs::Normal:
			switch (what) {
				case 0: npc->proceedBack(); break;
				case 1:	npc->proceedNext(); break;
				default: npc->end(); break;
			}
			break;
		case NpcDialogs::YesNo:
		case NpcDialogs::AcceptDecline:
		case NpcDialogs::AcceptDeclineNoExit:
			switch (what) {
				case 0: npc->proceedSelection(0); break;
				case 1:	npc->proceedSelection(1); break;
				default: npc->end(); break;
			}
			break;
		case NpcDialogs::GetText:
			if (what != 0) {
				npc->proceedText(packet.getString());
			}
			else {
				npc->end();
			}
			break;
		case NpcDialogs::GetNumber:
			if (what == 1) {
				npc->proceedNumber(packet.get<int32_t>());
			}
			else {
				npc->end();
			}
			break;
		case NpcDialogs::Simple:
			if (what == 0) {
				npc->end();
			}
			else {
				npc->proceedSelection(packet.get<uint8_t>());
			}
			break;
		case NpcDialogs::Style:
			if (what == 1) {
				npc->proceedSelection(packet.get<uint8_t>());
			}
			else  {
				npc->end();
			}
			break;
		default:
			npc->end();
	}
	npc->checkEnd();
}

void NpcHandler::handleNpcAnimation(Player *player, PacketReader &packet) {
	NpcPacket::animateNpc(player, packet);
}

void NpcHandler::useShop(Player *player, PacketReader &packet) {
	if (player->getShop() == 0) {
		// Hacking
		return;
	}
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case ShopOpcodes::Buy: {
			uint16_t itemindex = packet.get<uint16_t>();
			packet.skipBytes(4); // Item ID, no reason to trust this
			uint16_t quantity = packet.get<uint16_t>();
			packet.skipBytes(4); // Price, don't want to trust this
			int16_t amount = ShopDataProvider::Instance()->getAmount(player->getShop(), itemindex);
			int32_t itemid = ShopDataProvider::Instance()->getItemId(player->getShop(), itemindex);
			int32_t price = ShopDataProvider::Instance()->getPrice(player->getShop(), itemindex);
			uint32_t totalamount = quantity * amount; // The game doesn't let you purchase more than 1 slot worth of items; if they're grouped, it buys them in single units, if not, it only allows you to go up to maxslot
			int32_t totalprice = quantity * price;
			if (price == 0 || totalamount > ItemDataProvider::Instance()->getMaxSlot(itemid) || player->getInventory()->getMesos() < totalprice) {
				// Hacking
				return;
			}
			bool haveslot = player->getInventory()->hasOpenSlotsFor(itemid, static_cast<int16_t>(totalamount), true);
			if (haveslot) {
				Inventory::addNewItem(player, itemid, static_cast<int16_t>(totalamount));
				player->getInventory()->modifyMesos(-totalprice);
			}
			NpcPacket::bought(player, haveslot ? 0 : 3);
			break;
		}
		case ShopOpcodes::Sell: {
			int16_t slot = packet.get<int16_t>();
			int32_t itemid = packet.get<int32_t>();
			int16_t amount = packet.get<int16_t>();
			int8_t inv = GameLogicUtilities::getInventory(itemid);
			Item *item = player->getInventory()->getItem(inv, slot);
			if (item == nullptr || (!GameLogicUtilities::isRechargeable(itemid) && amount > item->getAmount())) {
				NpcPacket::bought(player, 1); // Hacking
				return;
			}
			int32_t price = ItemDataProvider::Instance()->getPrice(itemid);

			player->getInventory()->modifyMesos(price * amount);
			if (GameLogicUtilities::isRechargeable(itemid)) {
				Inventory::takeItemSlot(player, inv, slot, item->getAmount(), true);
			}
			else {
				Inventory::takeItemSlot(player, inv, slot, amount, true);
			}
			NpcPacket::bought(player, 0);
			break;
		}
		case ShopOpcodes::Recharge: {
			int16_t slot = packet.get<int16_t>();
			Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
			if (item == nullptr || !GameLogicUtilities::isRechargeable(item->getId())) {
				// Hacking
				return;
			}
			int16_t maxslot = ItemDataProvider::Instance()->getMaxSlot(item->getId());
			if (GameLogicUtilities::isRechargeable(item->getId())) {
				maxslot += player->getSkills()->getRechargeableBonus();
			}
			int32_t modifiedmesos = ShopDataProvider::Instance()->getRechargeCost(player->getShop(), item->getId(), maxslot - item->getAmount());
			if ((modifiedmesos < 0) && (player->getInventory()->getMesos() > -modifiedmesos)) {
				player->getInventory()->modifyMesos(modifiedmesos);
				InventoryPacket::updateItemAmounts(player, Inventories::UseInventory, slot, maxslot, 0, 0);
				item->setAmount(maxslot);
				NpcPacket::bought(player, 0);
			}
			break;
		}
		case ShopOpcodes::ExitShop:
			player->setShop(0);
			break;
	}
}

void NpcHandler::useStorage(Player *player, PacketReader &packet) {
	if (player->getShop() == 0) {
		// Hacking
		return;
	}
	int8_t type = packet.get<int8_t>();
	int32_t cost = NpcDataProvider::Instance()->getStorageCost(player->getShop());
	if (cost == 0) {
		// Hacking
		return;
	}
	switch (type) {
		case ShopOpcodes::TakeItem: {
			int8_t inv = packet.get<int8_t>();
			int8_t slot = packet.get<int8_t>();
			Item *item = player->getStorage()->getItem(slot);
			if (item == nullptr) {
				// Hacking
				return;
			}
			Inventory::addItem(player, new Item(item));
			player->getStorage()->takeItem(slot);
			StoragePacket::takeItem(player, inv);
			break;
		}
		case ShopOpcodes::StoreItem: {
			int16_t slot = packet.get<int16_t>();
			int32_t itemid = packet.get<int32_t>();
			int16_t amount = packet.get<int16_t>();
			if (player->getInventory()->getMesos() < cost) {
				// Player doesn't have enough mesos to store this item
				StoragePacket::noMesos(player);
				return;
			}
			if (player->getStorage()->isFull()) {
				// Storage is full, so tell the player and abort the mission
				StoragePacket::storageFull(player);
				return;
			}
			int8_t inv = GameLogicUtilities::getInventory(itemid);
			Item *item = player->getInventory()->getItem(inv, slot);
			if (item == nullptr) {
				// Hacking
				return;
			}
			if (!GameLogicUtilities::isStackable(itemid)) {
				amount = 1;
			}
			else if (amount <= 0 || amount > item->getAmount()) {
				// Hacking
				return;
			}
			player->getStorage()->addItem(GameLogicUtilities::isStackable(itemid) ? new Item(itemid, amount) : new Item(item));
			// For equips or rechargeable items (stars/bullets) we create a
			// new object for storage with the inventory object, and allow
			// the one in the inventory to go bye bye.
			// Else: For items we just create a new item based on the ID and amount.
			Inventory::takeItemSlot(player, inv, slot, GameLogicUtilities::isRechargeable(itemid) ? item->getAmount() : amount, true);
			player->getInventory()->modifyMesos(-cost);
			StoragePacket::addItem(player, inv);
			break;
		}
		case ShopOpcodes::MesoTransaction: {
			int32_t mesos = packet.get<int32_t>();
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

bool NpcHandler::showShop(Player *player, int32_t shopid) {
	if (ShopDataProvider::Instance()->isShop(shopid)) {
		PacketCreator p;
		ShopDataProvider::Instance()->showShop(shopid, player->getSkills()->getRechargeableBonus(), p);
		player->setShop(shopid);
		player->getSession()->send(p);
		return true;
	}
	return false;
}

bool NpcHandler::showStorage(Player *player, int32_t npcid) {
	if (NpcDataProvider::Instance()->getStorageCost(npcid)) {
		player->setShop(npcid);
		StoragePacket::showStorage(player, npcid);
		return true;
	}
	return false;
}

bool NpcHandler::showGuildRank(Player *player, int32_t npcid) {
	if (NpcDataProvider::Instance()->isGuildRank(npcid)) {
		GuildPacket::displayGuildRankBoard(player->getId(), npcid);
		return true;
	}
	return false;
}