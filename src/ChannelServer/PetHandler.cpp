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
#include "PetHandler.h"
#include "GameConstants.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "ItemConstants.h"
#include "ItemDataProvider.h"
#include "MovementHandler.h"
#include "PacketReader.h"
#include "Pet.h"
#include "PetsPacket.h"
#include "Player.h"
#include "Randomizer.h"
#include "SkillConstants.h"

void PetHandler::handleMovement(Player *player, PacketReader &packet) {
	int32_t petid = (int32_t)packet.get<int64_t>();
	Pet *pet = player->getPets()->getPet(petid);
	if (pet == nullptr) {
		// Hacks
		return;
	}
	packet.skipBytes(4);
	MovementHandler::parseMovement(pet, packet);
	packet.reset(10);
	PetsPacket::showMovement(player, pet, packet.getBuffer(), packet.getBufferLength() - 9);
}

void PetHandler::handleChat(Player *player, PacketReader &packet) {
	int32_t petid = (int32_t)packet.get<int64_t>();
	if (player->getPets()->getPet(petid) == nullptr) {
		// Hacks
		return;
	}
	packet.skipBytes(1);
	int8_t act = packet.get<int8_t>();
	string message = packet.getString();
	PetsPacket::showChat(player, player->getPets()->getPet(petid), message, act);
}

void PetHandler::handleSummon(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int16_t slot = packet.get<int16_t>();
	bool master = packet.get<int8_t>() == 1; // Might possibly fit under getBool criteria
	bool multipet = player->getSkills()->getSkillLevel(Jobs::Beginner::FollowTheLead) > 0;
	Pet *pet = player->getPets()->getPet(player->getInventory()->getItem(Inventories::CashInventory, slot)->getPetId());

	if (pet->isSummoned()) { // Removing a pet
		player->getPets()->setSummoned(pet->getIndex(), 0);
		if (pet->getIndex() == 0) {
			Timer::Id id(Timer::Types::PetTimer, pet->getIndex(), 0);
			player->getTimers()->removeTimer(id);
		}
		if (multipet) {
			for (int8_t i = pet->getIndex(); i < Inventories::MaxPetCount; i++) { // Shift around pets if using multipet
				if (Pet *move = player->getPets()->getSummoned(i)) {
					move->setIndex(i - 1);
					player->getPets()->setSummoned(move->getIndex(), move->getId());
					player->getPets()->setSummoned(i, 0);
					if (move->getIndex() == 0)
						move->startTimer();
				}
			}
		}
		int8_t index = pet->getIndex();
		pet->setIndex(-1);
		PetsPacket::petSummoned(player, pet, false, false, index);
	}
	else { // Summoning a Pet
		pet->setPos(player->getPos());
		if (!multipet || master) {
			pet->setIndex(0);
			if (multipet) {
				for (int8_t i = Inventories::MaxPetCount - 1; i > 0; i--) {
					if (player->getPets()->getSummoned(i - 1) && !player->getPets()->getSummoned(i)) {
						Pet *move = player->getPets()->getSummoned(i - 1);
						player->getPets()->setSummoned(i, move->getId());
						player->getPets()->setSummoned(i - 1, 0);
						move->setIndex(i);
					}
				}
				PetsPacket::petSummoned(player, pet);
			}
			else if (Pet *kicked = player->getPets()->getSummoned(0)) {
				kicked->setIndex(-1);
				Timer::Id id(Timer::Types::PetTimer, kicked->getIndex(), 0);
				player->getTimers()->removeTimer(id);
				PetsPacket::petSummoned(player, pet, true);
			}
			else
				PetsPacket::petSummoned(player, pet);

			player->getPets()->setSummoned(0, pet->getId());
			pet->startTimer();
		}
		else {
			for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
				if (!player->getPets()->getSummoned(i)) {
					player->getPets()->setSummoned(i, pet->getId());
					pet->setIndex(i);
					PetsPacket::petSummoned(player, pet);
					pet->startTimer();
					break;
				}
			}
		}
	}
	PetsPacket::blankUpdate(player);
}

void PetHandler::handleFeed(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int16_t slot = packet.get<int16_t>();
	int32_t itemid = packet.get<int32_t>();
	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	Pet *pet = player->getPets()->getSummoned(0);
	if (pet != nullptr && item != nullptr && item->getId() == itemid) {
		Inventory::takeItem(player, itemid, 1);

		bool success = (pet->getFullness() < Stats::MaxFullness);
		if (success) {
			PetsPacket::showAnimation(player, pet, 1);
			pet->modifyFullness(Stats::PetFeedFullness, false);
			if (Randomizer::Instance()->randInt(99) < 60) {
				// 60% chance for feed to add closeness
				pet->addCloseness(1);
			}
		}
	}
	else {
		InventoryPacket::blankUpdate(player);
	}
}

void PetHandler::handleCommand(Player *player, PacketReader &packet) {
	int32_t petid = (int32_t)packet.get<int64_t>();
	Pet *pet = player->getPets()->getPet(petid);
	if (pet == nullptr) {
		// Hacks
		return;
	}
	packet.skipBytes(1);
	int8_t act = packet.get<int8_t>();
	PetInteractInfo *action = ItemDataProvider::Instance()->getInteraction(pet->getItemId(), act);
	if (action == nullptr) {
		// Hacks or no action info available.
		return;
	}
	bool success = (Randomizer::Instance()->randInt(100) < action->prob);
	if (success) {
		pet->addCloseness(action->increase);
	}
	PetsPacket::showAnimation(player, pet, act);
}

void PetHandler::handleConsumePotion(Player *player, PacketReader &packet) {
	int32_t petid = (int32_t)packet.get<int64_t>();
	Pet *pet = player->getPets()->getPet(petid);
	if (pet == nullptr || !pet->isSummoned() || player->getStats()->getHp() == 0) {
		// Hacking
		return;
	}
	packet.skipBytes(1); // It MIGHT be some flag for Meso/Power/Magic Guard...?
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int16_t slot = packet.get<int16_t>();
	int32_t itemid = packet.get<int32_t>();
	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	ConsumeInfo *info = ItemDataProvider::Instance()->getConsumeInfo(itemid);
	if (item == nullptr || item->getId() != itemid) {
		// Hacking
		return;
	}

	// Check if the MP potion IS a MP potion set
	if ((info->mp != 0 || info->mpr != 0) && player->getInventory()->getAutoMpPot() != itemid) {
		// Hacking
		return;
	}
	// Check if the HP potion IS a HP potion set
	if ((info->hp != 0 || info->hpr != 0) && player->getInventory()->getAutoHpPot() != itemid) {
		// Hacking
		return;
	}

	Inventory::useItem(player, itemid);
	Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);
}

void PetHandler::changeName(Player *player, const string &name) {
	if (Pet *pet = player->getPets()->getSummoned(0)) {
		pet->setName(name);
	}
}

void PetHandler::showPets(Player *player) {
	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			pet->setPos(player->getPos());
			PetsPacket::petSummoned(player, pet, false, true);
		}
	}
	PetsPacket::updateSummonedPets(player);
}