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
#include "Inventory.h"
#include "Buffs.h"
#include "GameLogicUtilities.h"
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "MonsterBookPacket.h"
#include "Pet.h"
#include "PetsPacket.h"
#include "Player.h"
#include "PlayerMonsterBook.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"

auto Inventory::addItem(Player *player, Item *item, bool fromDrop) -> int16_t {
	int8_t inv = GameLogicUtilities::getInventory(item->getId());
	int16_t freeSlot = 0;
	for (int16_t s = 1; s <= player->getInventory()->getMaxSlots(inv); s++) {
		Item *oldItem = player->getInventory()->getItem(inv, s);
		if (oldItem != nullptr) {
			uint16_t slotMax = ItemDataProvider::getInstance().getMaxSlot(item->getId());
			if (GameLogicUtilities::isStackable(item->getId()) && oldItem->getId() == item->getId() && oldItem->getAmount() < slotMax) {
				if (item->getAmount() + oldItem->getAmount() > slotMax) {
					int16_t amount = slotMax - oldItem->getAmount();
					item->decAmount(amount);
					oldItem->setAmount(slotMax);

					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(InventoryPacket::OperationTypes::AddItem, oldItem, s);
					InventoryPacket::inventoryOperation(player, fromDrop, ops);
				}
				else {
					item->incAmount(oldItem->getAmount());
					player->getInventory()->deleteItem(inv, s);
					player->getInventory()->addItem(inv, s, item);
					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(InventoryPacket::OperationTypes::AddItem, item, s);
					InventoryPacket::inventoryOperation(player, fromDrop, ops);
					return 0;
				}
			}
		}
		else if (!freeSlot) {
			freeSlot = s;
			if (!GameLogicUtilities::isStackable(item->getId())) {
				break;
			}
		}
	}
	if (freeSlot != 0) {
		player->getInventory()->addItem(inv, freeSlot, item);

		vector_t<InventoryPacketOperation> ops;
		ops.emplace_back(InventoryPacket::OperationTypes::AddItem, item, freeSlot);
		InventoryPacket::inventoryOperation(player, fromDrop, ops);

		if (GameLogicUtilities::isPet(item->getId())) {
			Pet *pet = new Pet(player, item);
			player->getPets()->addPet(pet);
			pet->setInventorySlot(static_cast<int8_t>(freeSlot));
			PetsPacket::updatePet(player, pet, item);
		}
		return 0;
	}
	return item->getAmount();
}

auto Inventory::addNewItem(Player *player, int32_t itemId, int16_t amount, bool random) -> void {
	if (!ItemDataProvider::getInstance().itemExists(itemId)) {
		return;
	}

	int16_t max = ItemDataProvider::getInstance().getMaxSlot(itemId);
	int16_t thisAmount = 0;
	if (GameLogicUtilities::isRechargeable(itemId)) {
		thisAmount = max + player->getSkills()->getRechargeableBonus();
		amount -= 1;
	}
	else if (GameLogicUtilities::isEquip(itemId) || GameLogicUtilities::isPet(itemId)) {
		thisAmount = 1;
		amount -= 1;
	}
	else if (amount > max) {
		thisAmount = max;
		amount -= max;
	}
	else {
		thisAmount = amount;
		amount = 0;
	}

	Item *item = nullptr;
	if (GameLogicUtilities::isEquip(itemId)) {
		item = new Item(itemId, random, player->hasGmBenefits());
		if (GameLogicUtilities::isMount(itemId)) {
			player->getMounts()->addMount(itemId);
		}
	}
	else {
		item = new Item(itemId, thisAmount);
	}
	if (addItem(player, item, GameLogicUtilities::isPet(itemId)) == 0 && amount > 0) {
		addNewItem(player, itemId, amount);
	}
}

auto Inventory::takeItem(Player *player, int32_t itemId, uint16_t howMany) -> void {
	if (player->hasGmBenefits()) {
		return;
	}

	player->getInventory()->changeItemAmount(itemId, -howMany);
	int8_t inv = GameLogicUtilities::getInventory(itemId);
	for (int16_t i = 1; i <= player->getInventory()->getMaxSlots(inv); i++) {
		Item *item = player->getInventory()->getItem(inv, i);
		if (item == nullptr) {
			continue;
		}
		if (item->getId() == itemId) {
			if (item->getAmount() >= howMany) {
				item->decAmount(howMany);
				if (item->getAmount() == 0 && !GameLogicUtilities::isRechargeable(item->getId())) {
					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(InventoryPacket::OperationTypes::ModifySlot, item, i);
					InventoryPacket::inventoryOperation(player, true, ops);

					player->getInventory()->deleteItem(inv, i);
				}
				else {
					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(InventoryPacket::OperationTypes::ModifyQuantity, item, i);
					InventoryPacket::inventoryOperation(player, true, ops);
				}
				break;
			}
			else if (!GameLogicUtilities::isRechargeable(item->getId())) {
				howMany -= item->getAmount();
				item->setAmount(0);

				vector_t<InventoryPacketOperation> ops;
				ops.emplace_back(InventoryPacket::OperationTypes::ModifySlot, item, i);
				InventoryPacket::inventoryOperation(player, true, ops);

				player->getInventory()->deleteItem(inv, i);
			}
		}
	}
}

auto Inventory::takeItemSlot(Player *player, int8_t inv, int16_t slot, int16_t amount, bool takeStar) -> void {
	if (player->hasGmBenefits()) {
		return;
	}

	Item *item = player->getInventory()->getItem(inv, slot);
	if (item == nullptr || item->getAmount() - amount < 0) {
		return;
	}
	item->decAmount(amount);
	if ((item->getAmount() == 0 && !GameLogicUtilities::isRechargeable(item->getId())) || (takeStar && GameLogicUtilities::isRechargeable(item->getId()))) {
		vector_t<InventoryPacketOperation> ops;
		ops.emplace_back(InventoryPacket::OperationTypes::ModifySlot, item, slot);
		InventoryPacket::inventoryOperation(player, true, ops);

		player->getInventory()->deleteItem(inv, slot);
	}
	else {
		player->getInventory()->changeItemAmount(item->getId(), -amount);

		vector_t<InventoryPacketOperation> ops;
		ops.emplace_back(InventoryPacket::OperationTypes::ModifyQuantity, item, slot);
		InventoryPacket::inventoryOperation(player, true, ops);
	}
}

auto Inventory::useItem(Player *player, int32_t itemId) -> void {
	ConsumeInfo *item = ItemDataProvider::getInstance().getConsumeInfo(itemId);

	if (item == nullptr) {
		// No reason not to check
		return;
	}

	int16_t potency = 100;
	int32_t skillId = player->getSkills()->getAlchemist();

	if (player->getSkills()->getSkillLevel(skillId) > 0) {
		potency = player->getSkills()->getSkillInfo(skillId)->x;
	}

	bool zombie = player->getActiveBuffs()->isZombified();

	if (item->hp > 0) {
		player->getStats()->modifyHp(item->hp * (zombie ? (potency / 2) : potency) / 100);
	}
	if (item->mp > 0) {
		player->getStats()->modifyMp(item->mp * potency / 100);
	}
	else {
		player->getStats()->setMp(player->getStats()->getMp(), true);
	}
	if (item->hpr != 0) {
		player->getStats()->modifyHp(item->hpr * (zombie ? (player->getStats()->getMaxHp() / 2) : player->getStats()->getMaxHp()) / 100);
	}
	if (item->mpr != 0) {
		player->getStats()->modifyMp(item->mpr * player->getStats()->getMaxMp() / 100);
	}
	if (item->ailment > 0) {
		player->getActiveBuffs()->useDebuffHealingItem(item->ailment);
	}
	if (item->time > 0 && item->mcProb == 0) {
		seconds_t time(item->time * potency / 100);
		Buffs::addBuff(player, itemId, time);
	}
	if (GameLogicUtilities::isMonsterCard(itemId)) {
		bool isFull = player->getMonsterBook()->addCard(itemId); // Has a special buff for being full?
		MonsterBookPacket::addCard(player, itemId, player->getMonsterBook()->getCardLevel(itemId), isFull);
		if (item->mcProb != 0 && Randomizer::rand<uint16_t>(99) < item->mcProb) {
			Buffs::addBuff(player, itemId, seconds_t(item->time));
		}
	}
}