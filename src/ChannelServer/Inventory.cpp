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
#include "Inventory.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/ItemDataProvider.hpp"
#include "Common/Randomizer.hpp"
#include "Common/SkillDataProvider.hpp"
#include "ChannelServer/Buffs.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/InventoryPacket.hpp"
#include "ChannelServer/Map.hpp"
#include "ChannelServer/MonsterBookPacket.hpp"
#include "ChannelServer/Pet.hpp"
#include "ChannelServer/PetsPacket.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerMonsterBook.hpp"

namespace Vana {
namespace ChannelServer {

auto Inventory::addItem(Player *player, Item *item, bool fromDrop) -> slot_qty_t {
	inventory_t inv = GameLogicUtilities::getInventory(item->getId());
	inventory_slot_t freeSlot = 0;
	for (inventory_slot_t s = 1; s <= player->getInventory()->getMaxSlots(inv); s++) {
		Item *oldItem = player->getInventory()->getItem(inv, s);
		if (oldItem != nullptr) {
			auto itemInfo = ChannelServer::getInstance().getItemDataProvider().getItemInfo(item->getId());
			slot_qty_t maxSlot = itemInfo->maxSlot;
			if (GameLogicUtilities::isStackable(item->getId()) && oldItem->getId() == item->getId() && oldItem->getAmount() < maxSlot) {
				if (item->getAmount() + oldItem->getAmount() > maxSlot) {
					slot_qty_t amount = maxSlot - oldItem->getAmount();
					item->decAmount(amount);
					oldItem->setAmount(maxSlot);

					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(Packets::Inventory::OperationTypes::AddItem, oldItem, s);
					player->send(Packets::Inventory::inventoryOperation(fromDrop, ops));
				}
				else {
					item->incAmount(oldItem->getAmount());
					player->getInventory()->deleteItem(inv, s);
					player->getInventory()->addItem(inv, s, item);
					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(Packets::Inventory::OperationTypes::AddItem, item, s);
					player->send(Packets::Inventory::inventoryOperation(fromDrop, ops));
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
		ops.emplace_back(Packets::Inventory::OperationTypes::AddItem, item, freeSlot);
		player->send(Packets::Inventory::inventoryOperation(fromDrop, ops));

		if (GameLogicUtilities::isPet(item->getId())) {
			Pet *pet = new Pet(player, item);
			player->getPets()->addPet(pet);
			pet->setInventorySlot(static_cast<int8_t>(freeSlot));
			player->send(Packets::Pets::updatePet(pet, item));
		}

		return 0;
	}
	return item->getAmount();
}

auto Inventory::addNewItem(Player *player, item_id_t itemId, slot_qty_t amount, Items::StatVariance variancePolicy) -> void {
	auto itemInfo = ChannelServer::getInstance().getItemDataProvider().getItemInfo(itemId);
	if (itemInfo == nullptr) {
		return;
	}

	slot_qty_t max = itemInfo->maxSlot;
	slot_qty_t thisAmount = 0;
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
		item = new Item{
			ChannelServer::getInstance().getEquipDataProvider(),
			itemId,
			variancePolicy,
			player->hasGmBenefits()
		};
		if (GameLogicUtilities::isMount(itemId)) {
			player->getMounts()->addMount(itemId);
		}
	}
	else {
		item = new Item{itemId, thisAmount};
	}
	if (addItem(player, item, GameLogicUtilities::isPet(itemId)) == 0 && amount > 0) {
		addNewItem(player, itemId, amount);
	}
}

auto Inventory::takeItem(Player *player, item_id_t itemId, slot_qty_t howMany) -> void {
	if (player->hasGmBenefits()) {
		player->send(Packets::Inventory::blankUpdate());
		return;
	}

	player->getInventory()->changeItemAmount(itemId, -howMany);
	inventory_t inv = GameLogicUtilities::getInventory(itemId);
	for (inventory_slot_t i = 1; i <= player->getInventory()->getMaxSlots(inv); i++) {
		Item *item = player->getInventory()->getItem(inv, i);
		if (item == nullptr) {
			continue;
		}
		if (item->getId() == itemId) {
			if (item->getAmount() >= howMany) {
				item->decAmount(howMany);
				if (item->getAmount() == 0 && !GameLogicUtilities::isRechargeable(item->getId())) {
					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, item, i);
					player->send(Packets::Inventory::inventoryOperation(true, ops));

					player->getInventory()->deleteItem(inv, i);
				}
				else {
					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(Packets::Inventory::OperationTypes::ModifyQuantity, item, i);
					player->send(Packets::Inventory::inventoryOperation(true, ops));
				}
				break;
			}
			else if (!GameLogicUtilities::isRechargeable(item->getId())) {
				howMany -= item->getAmount();
				item->setAmount(0);

				vector_t<InventoryPacketOperation> ops;
				ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, item, i);
				player->send(Packets::Inventory::inventoryOperation(true, ops));

				player->getInventory()->deleteItem(inv, i);
			}
		}
	}
}

auto Inventory::takeItemSlot(Player *player, inventory_t inv, inventory_slot_t slot, slot_qty_t amount, bool takeStar, bool overrideGmBenefits) -> void {
	if (!overrideGmBenefits && player->hasGmBenefits()) {
		return;
	}

	Item *item = player->getInventory()->getItem(inv, slot);
	if (item == nullptr || item->getAmount() - amount < 0) {
		return;
	}
	item->decAmount(amount);
	if ((item->getAmount() == 0 && !GameLogicUtilities::isRechargeable(item->getId())) || (takeStar && GameLogicUtilities::isRechargeable(item->getId()))) {
		vector_t<InventoryPacketOperation> ops;
		ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, item, slot);
		player->send(Packets::Inventory::inventoryOperation(true, ops));

		player->getInventory()->deleteItem(inv, slot);
	}
	else {
		player->getInventory()->changeItemAmount(item->getId(), -amount);

		vector_t<InventoryPacketOperation> ops;
		ops.emplace_back(Packets::Inventory::OperationTypes::ModifyQuantity, item, slot);
		player->send(Packets::Inventory::inventoryOperation(true, ops));
	}
}

auto Inventory::useItem(Player *player, item_id_t itemId) -> void {
	auto item = ChannelServer::getInstance().getItemDataProvider().getConsumeInfo(itemId);
	if (item == nullptr) {
		// Not a consume
		return;
	}

	int16_t potency = 100;
	skill_id_t alchemist = player->getSkills()->getAlchemist();

	if (player->getSkills()->getSkillLevel(alchemist) > 0) {
		potency = player->getSkills()->getSkillInfo(alchemist)->x;
	}

	auto buffs = player->getActiveBuffs();
	if (item->hp > 0) {
		player->getStats()->modifyHp(item->hp * buffs->getZombifiedPotency(potency) / 100);
	}
	if (item->mp > 0) {
		player->getStats()->modifyMp(item->mp * potency / 100);
	}
	else {
		player->getStats()->setMp(player->getStats()->getMp(), true);
	}
	if (item->hpr != 0) {
		player->getStats()->modifyHp(item->hpr * buffs->getZombifiedPotency(player->getStats()->getMaxHp()) / 100);
	}
	if (item->mpr != 0) {
		player->getStats()->modifyMp(item->mpr * player->getStats()->getMaxMp() / 100);
	}
	if (item->ailment > 0) {
		player->getActiveBuffs()->useDebuffHealingItem(item->ailment);
	}
	if (item->buffTime.count() > 0 && item->chance == 0) {
		seconds_t time{item->buffTime.count() * potency / 100};
		Buffs::addBuff(player, itemId, time);
	}
	if (GameLogicUtilities::isMonsterCard(itemId)) {
		bool isFull = player->getMonsterBook()->addCard(itemId); // Has a special buff for being full?
		player->send(Packets::MonsterBook::addCard(itemId, player->getMonsterBook()->getCardLevel(itemId), isFull));
		if (!isFull) {
			player->sendMap(Packets::MonsterBook::addCardEffect(player->getId()));
		}
		Buffs::addBuff(player, itemId, item->buffTime);
	}
}

}
}