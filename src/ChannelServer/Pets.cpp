/*
Copyright (C) 2008 Vana Development Team

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
#include "Pets.h"
#include "Drops.h"
#include "DropsPacket.h"
#include "Inventory.h"
#include "Maps.h"
#include "Movement.h"
#include "MySQLM.h"
#include "PetsPacket.h"
#include "Player.h"
#include "Pos.h"
#include "Randomizer.h"
#include "Reactors.h"
#include "ReadPacket.h"
#include "Timer/Timer.h"
#include <functional>
#include <string>
#include <unordered_map>

using std::string;
using std::tr1::bind;
using std::tr1::unordered_map;

unordered_map<int32_t, PetInfo> Pets::petsInfo;
unordered_map<int32_t, unordered_map<int32_t, PetInteractInfo>> Pets::petsInteractInfo;

int16_t Pets::exps[29] = {1, 3, 6, 14, 31, 60, 108, 181, 287, 434, 632, 891, 1224, 1642, 2161, 2793, 3557, 4467, 5542, 6801, 8263, 9950, 11882, 14084, 16578, 19391, 22548, 26074, 30000};

/* Pet class */
Pet::Pet(Player *player, Item *item) : fullness(100), closeness(0), level(1), summoned(false), index(-1), player(player) {
	this->type = item->id;
	this->name = Pets::petsInfo[type].name;

	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO pets (name) VALUES ("<< mysqlpp::quote << this->name << ")";
	mysqlpp::SimpleResult res = query.execute();
	this->id = (int32_t) res.insert_id();
	item->petid = this->id;
}

void Pet::reduceFullness() {
	setFullness(getFullness() - 1);
	PetsPacket::updatePet(player, this);
}

void Pet::startTimer() {
	Timer::Id id(Timer::Types::PetTimer, getIndex(), 0); // The timer will automatically stop if another pet gets inserted into this index
	clock_t length = (6 - Pets::petsInfo[getType()].hunger)* 1000 * 60; // TODO: Better formula
	new Timer::Timer(bind(&Pet::reduceFullness, this), id, player->getTimers(), length, true);
}

/* Pets namespace */

void Pets::movePet(Player *player, ReadPacket &packet) {
	int32_t petid = packet.getInt();
	Pet *pet = player->getPets()->getPet(petid);
	packet.skipBytes(8);
	Movement::parseMovement(pet, packet);
	packet.reset(10);
	PetsPacket::movePet(player, pet, packet.getBuffer(), packet.getBufferLength() - 9);
}

void Pets::chat(Player *player, ReadPacket &packet) {
	int32_t petid = packet.getInt();
	packet.skipBytes(5);
	int8_t act = packet.getByte();
	string message = packet.getString();
	PetsPacket::showChat(player, player->getPets()->getPet(petid), message, act);
}

void Pets::summonPet(Player *player, ReadPacket &packet) {
	packet.skipBytes(4);
	int16_t slot = packet.getShort();
	bool master = packet.getByte() == 1;
	Pet *pet = player->getPets()->getPet(player->getInventory()->getItem(5, slot)->petid);
	pet->setPos(player->getPos());
	summon(player, pet, master);
}

void Pets::feedPet(Player *player, ReadPacket &packet) {
	packet.skipBytes(4);
	int16_t slot = packet.getShort();
	int32_t item = packet.getInt();
	if (Pet *pet = player->getPets()->getSummoned(0)) {
		bool success = false;
		if (pet->getFullness() < 100) {
			pet->setFullness(pet->getFullness() + 30);
			success = true;
		}
		Inventory::takeItem(player, item, 1);
		PetsPacket::showAnimation(player, pet, 1, success);
		PetsPacket::updatePet(player, pet);
	}
}

void Pets::showAnimation(Player *player, ReadPacket &packet) {
	int32_t petid = packet.getInt();
	packet.skipBytes(5);
	int8_t act = packet.getByte();
	Pet *pet = player->getPets()->getPet(petid);
	bool success = false;
	if (Randomizer::Instance()->randInt(100) < petsInteractInfo[pet->getType()][act].prob) {
		success = true;
		addCloseness(player, pet, petsInteractInfo[pet->getType()][act].increase);
	}
	PetsPacket::showAnimation(player, pet, act, success);
}

void Pets::changeName(Player *player, const string &name) {
	if (Pet *pet = player->getPets()->getSummoned(0)) {
		pet->setName(name);
		PetsPacket::changeName(player, pet);
		PetsPacket::updatePet(player, pet);
	}
}

void Pets::addCloseness(Player *player, Pet *pet, int16_t closeness) {
	if (pet->getLevel() < 30) {
		if (pet->getCloseness() + closeness < 30000)
			pet->setCloseness(pet->getCloseness() + closeness);
		else
			pet->setCloseness(30000);
		if (pet->getCloseness() >= exps[pet->getLevel() - 1]) {
			pet->setLevel(pet->getLevel() + 1);
			PetsPacket::levelUp(player, pet);
		}
	}
	PetsPacket::updatePet(player, pet);
}

void Pets::lootItem(Player *player, ReadPacket &packet) {
	int32_t petid = packet.getInt();
	packet.skipBytes(13);
	int32_t dropid = packet.getInt();
	Drop *drop = Maps::maps[player->getMap()]->getDrop(dropid);
	if (drop == 0) {
		DropsPacket::dontTake(player);
		return;
	}
	if (drop->isQuest()) {
		int32_t request = 0;
		for (size_t i = 0; i < Quests::quests[drop->getQuest()].rewards.size(); i++) {
			if (Quests::quests[drop->getQuest()].rewards[i].id == drop->getObjectID()) {
				request = Quests::quests[drop->getQuest()].rewards[i].count;
			}
		}
		if (player->getInventory()->getItemAmount(drop->getObjectID()) > request || !player->getQuests()->isQuestActive(drop->getQuest())) {
			DropsPacket::takeNote(player, 0, false, 0);
			DropsPacket::dontTake(player);
			return;
		}
	}
	if (drop->isMesos()) {
		bool success = player->getInventory()->modifyMesos(drop->getObjectID(), true);
		if (success)
			DropsPacket::takeNote(player, drop->getObjectID(), true, 0);
	}
	else {
		Item *item = new Item(drop->getItem());
		int16_t dropAmount = drop->getAmount();
		int16_t amount = Inventory::addItem(player, item, true);
		if (amount > 0) {
			if (dropAmount - amount > 0) {
				DropsPacket::takeNote(player, drop->getObjectID(), false, dropAmount - amount);
				drop->setItemAmount(amount);
			}
			DropsPacket::takeNote(player, 0, 0, 0);
			DropsPacket::dontTake(player);
			return;
		}
		DropsPacket::takeNote(player, drop->getObjectID(), false, drop->getAmount());
	}
	Reactors::checkLoot(drop);
	Maps::maps[player->getMap()]->removeDrop(drop->getID());
	DropsPacket::takeDropPet(player, drop, player->getPets()->getPet(petid));
}

void Pets::showPets(Player *player) {
	for (int8_t i = 0; i < 3; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			if (!pet->isSummoned()) {
				if (pet->getIndex() == 0) {
					pet->startTimer();
				}
				pet->setSummoned(true);
			}
			pet->setPos(player->getPos());
			PetsPacket::petSummoned(player, pet, false, true);
		}
	}
	PetsPacket::updateSummonedPets(player);
}

void Pets::summon(Player *player, Pet *pet, bool master) {
	if (player->getSkills()->getSkillLevel(8) == 1) {
		if (pet->isSummoned()) {
			player->getPets()->setSummoned(0, pet->getIndex());
			for (int8_t i = (pet->getIndex() + 1); i < 3; i++) {
				if (Pet *move = player->getPets()->getSummoned(i)) {
					move->setIndex(i - 1);
					player->getPets()->setSummoned(move->getId(), i - 1);
					player->getPets()->setSummoned(0, i);
					if (i - 1 == 0) {
						pet->startTimer();
					}
				}
			}
			if (pet->getIndex() == 0) {
				Timer::Id id(Timer::Types::PetTimer, pet->getId(), 0);
				player->getTimers()->removeTimer(id);
			}
			pet->setSummoned(false);
			PetsPacket::petSummoned(player, pet);
			pet->setIndex(-1);
		}
		else {
			if (master) {
				for (int8_t k = 2; k > 0; k--) {
					if (player->getPets()->getSummoned(k - 1) && !player->getPets()->getSummoned(k)) {
						Pet *move = player->getPets()->getSummoned(k - 1);
						player->getPets()->setSummoned(0, k - 1);
						player->getPets()->setSummoned(move->getId(), k);
						move->setIndex(k);
					}
				}
				pet->setIndex(0);
				pet->setSummoned(true);
				player->getPets()->setSummoned(pet->getId(), 0);
				PetsPacket::petSummoned(player, pet);
			}
			else {
				for (int8_t i = 0; i < 3; i++) {
					if (!player->getPets()->getSummoned(i)) {
						player->getPets()->setSummoned(pet->getId(), i);
						pet->setIndex(i);
						pet->setSummoned(true);
						PetsPacket::petSummoned(player, pet);
						pet->startTimer();
						break;
					}
				}
			}
		}
	}
	else {
		if (pet->isSummoned()) {
			player->getPets()->setSummoned(0, 0);
			pet->setSummoned(false);
			PetsPacket::petSummoned(player, pet);
			pet->setIndex(-1);
			Timer::Id id(Timer::Types::PetTimer, pet->getId(), 0);
			player->getTimers()->removeTimer(id);
		}
		else {
			pet->setIndex(0);
			pet->setSummoned(true);
			if (Pet *kicked = player->getPets()->getSummoned(0)) {
				kicked->setIndex(-1);
				kicked->setSummoned(false);
				Timer::Id id(Timer::Types::PetTimer, kicked->getId(), 0);
				player->getTimers()->removeTimer(id);
				PetsPacket::petSummoned(player, pet, true);
			}
			else
				PetsPacket::petSummoned(player, pet);
			player->getPets()->setSummoned(pet->getId(), 0);
			pet->startTimer();
 		}
	}
	PetsPacket::blankUpdate(player);
}
