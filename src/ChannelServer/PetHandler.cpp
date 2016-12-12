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
#include "PetHandler.hpp"
#include "ChannelServer.hpp"
#include "GameConstants.hpp"
#include "Inventory.hpp"
#include "InventoryPacket.hpp"
#include "ItemConstants.hpp"
#include "ItemDataProvider.hpp"
#include "Map.hpp"
#include "MovementHandler.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "Pet.hpp"
#include "PetsPacket.hpp"
#include "Player.hpp"
#include "Randomizer.hpp"
#include "SkillConstants.hpp"

auto PetHandler::handleMovement(Player *player, PacketReader &reader) -> void {
	pet_id_t petId = reader.get<pet_id_t>();
	Pet *pet = player->getPets()->getPet(petId);
	if (pet == nullptr) {
		// Hacking
		return;
	}
	reader.skipBytes(4); // Ticks?
	MovementHandler::parseMovement(pet, reader);
	reader.reset(10);
	player->sendMap(PetsPacket::showMovement(player->getId(), pet, reader.getBuffer(), reader.getBufferLength() - 9));
}

auto PetHandler::handleChat(Player *player, PacketReader &reader) -> void {
	pet_id_t petId = reader.get<pet_id_t>();
	if (player->getPets()->getPet(petId) == nullptr) {
		// Hacking
		return;
	}
	reader.skipBytes(1);
	int8_t act = reader.get<int8_t>();
	string_t message = reader.get<string_t>();
	player->sendMap(PetsPacket::showChat(player->getId(), player->getPets()->getPet(petId), message, act));
}

auto PetHandler::handleSummon(Player *player, PacketReader &reader) -> void {
	tick_count_t ticks = reader.get<tick_count_t>();
	inventory_slot_t slot = reader.get<inventory_slot_t>();
	bool master = reader.get<int8_t>() == 1; // Might possibly fit under getBool criteria
	bool multipet = player->getSkills()->hasFollowTheLead();
	Pet *pet = player->getPets()->getPet(player->getInventory()->getItem(Inventories::CashInventory, slot)->getPetId());

	if (pet == nullptr) {
		// Hacking
		return;
	}

	if (pet->isSummoned()) {
		// Removing a pet
		int8_t index = pet->getIndex().get();
		player->getPets()->setSummoned(index, 0);
		if (index == 0) {
			Timer::Id id(TimerType::PetTimer, index);
			player->getTimerContainer()->removeTimer(id);
		}
		if (multipet) {
			for (int8_t i = index; i < Inventories::MaxPetCount; ++i) {
				// Shift around pets if using multipet
				if (Pet *move = player->getPets()->getSummoned(i)) {
					move->summon(i - 1);
					int8_t moveIndex = move->getIndex().get();
					player->getPets()->setSummoned(moveIndex, move->getId());
					player->getPets()->setSummoned(i, 0);
					if (moveIndex == 0) {
						move->startTimer();
					}
				}
			}
		}

		pet->desummon();
		player->sendMap(PetsPacket::petSummoned(player->getId(), pet, false, index));
	}
	else {
		// Summoning a Pet
		pet->setPos(player->getPos());
		if (!multipet || master) {
			pet->summon(0);
			if (multipet) {
				for (int8_t i = Inventories::MaxPetCount - 1; i > 0; --i) {
					if (player->getPets()->getSummoned(i - 1) && !player->getPets()->getSummoned(i)) {
						Pet *move = player->getPets()->getSummoned(i - 1);
						player->getPets()->setSummoned(i, move->getId());
						player->getPets()->setSummoned(i - 1, 0);
						move->summon(i);
					}
				}
				player->sendMap(PetsPacket::petSummoned(player->getId(), pet));
			}
			else if (Pet *kicked = player->getPets()->getSummoned(0)) {
				Timer::Id id(TimerType::PetTimer, kicked->getIndex().get());
				player->getTimerContainer()->removeTimer(id);
				kicked->desummon();
				player->sendMap(PetsPacket::petSummoned(player->getId(), pet, true));
			}
			else {
				player->sendMap(PetsPacket::petSummoned(player->getId(), pet));
			}
			player->getPets()->setSummoned(0, pet->getId());
			pet->startTimer();
		}
		else {
			for (int8_t i = 0; i < Inventories::MaxPetCount; ++i) {
				if (!player->getPets()->getSummoned(i)) {
					player->getPets()->setSummoned(i, pet->getId());
					pet->summon(i);
					player->sendMap(PetsPacket::petSummoned(player->getId(), pet));
					pet->startTimer();
					break;
				}
			}
		}
	}
	player->send(PetsPacket::blankUpdate());
}

auto PetHandler::handleFeed(Player *player, PacketReader &reader) -> void {
	tick_count_t ticks = reader.get<tick_count_t>();
	inventory_slot_t slot = reader.get<inventory_slot_t>();
	item_id_t itemId = reader.get<item_id_t>();
	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	Pet *pet = player->getPets()->getSummoned(0);
	if (pet != nullptr && item != nullptr && item->getId() == itemId) {
		Inventory::takeItem(player, itemId, 1);

		bool success = (pet->getFullness() < Stats::MaxFullness);
		if (success) {
			player->send(PetsPacket::showAnimation(player->getId(), pet, 1));

			pet->modifyFullness(Stats::PetFeedFullness, false);
			if (Randomizer::rand<int32_t>(99) < 60) {
				// 60% chance for feed to add closeness
				pet->addCloseness(1);
			}
		}
	}
	else {
		player->send(InventoryPacket::blankUpdate());
	}
}

auto PetHandler::handleCommand(Player *player, PacketReader &reader) -> void {
	pet_id_t petId = reader.get<pet_id_t>();
	Pet *pet = player->getPets()->getPet(petId);
	if (pet == nullptr) {
		// Hacking
		return;
	}
	reader.skipBytes(1);
	int8_t act = reader.get<int8_t>();
	auto action = ChannelServer::getInstance().getItemDataProvider().getInteraction(pet->getItemId(), act);
	if (action == nullptr) {
		// Hacking or no action info available
		return;
	}

	if (Randomizer::rand<uint32_t>(100) < action->prob) {
		pet->addCloseness(action->increase);
	}

	player->send(PetsPacket::showAnimation(player->getId(), pet, act));
}

auto PetHandler::handleConsumePotion(Player *player, PacketReader &reader) -> void {
	pet_id_t petId = reader.get<pet_id_t>();
	Pet *pet = player->getPets()->getPet(petId);
	if (pet == nullptr || !pet->isSummoned() || player->getStats()->isDead()) {
		// Hacking
		return;
	}
	reader.skipBytes(1); // It MIGHT be some flag for Meso/Power/Magic Guard...?
	tick_count_t ticks = reader.get<tick_count_t>();
	inventory_slot_t slot = reader.get<inventory_slot_t>();
	item_id_t itemId = reader.get<item_id_t>();
	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	auto info = ChannelServer::getInstance().getItemDataProvider().getConsumeInfo(itemId);
	if (item == nullptr || item->getId() != itemId) {
		// Hacking
		return;
	}

	// Check if the MP potion is an MP potion set
	if ((info->mp != 0 || info->mpr != 0) && player->getInventory()->getAutoMpPot() != itemId) {
		// Hacking
		return;
	}

	// Check if the HP potion is an HP potion set
	if ((info->hp != 0 || info->hpr != 0) && player->getInventory()->getAutoHpPot() != itemId) {
		// Hacking
		return;
	}

	Inventory::useItem(player, itemId);
	Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);
}

auto PetHandler::changeName(Player *player, const string_t &name) -> void {
	if (Pet *pet = player->getPets()->getSummoned(0)) {
		pet->setName(name);
	}
}

auto PetHandler::showPets(Player *player) -> void {
	for (int8_t i = 0; i < Inventories::MaxPetCount; ++i) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			pet->setPos(player->getPos());
			player->send(PetsPacket::petSummoned(player->getId(), pet));
		}
	}
	player->send(PetsPacket::updateSummonedPets(player));
}