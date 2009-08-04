/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "Database.h"
#include "GameConstants.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "Maps.h"
#include "MovementHandler.h"
#include "PacketReader.h"
#include "PetDataProvider.h"
#include "PetsPacket.h"
#include "Player.h"
#include "Pos.h"
#include "Randomizer.h"
#include "Reactors.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>
#include <string>

using std::string;
using std::tr1::bind;

int16_t Pets::exps[Stats::PetLevels - 1] = {
	1, 3, 6, 14, 31, 60, 108, 181, 287, 434,
	632, 891, 1224, 1642, 2161, 2793, 3557, 4467, 5542, 6801,
	8263, 9950, 11882, 14084, 16578, 19391, 22548, 26074, 30000
};

/* Pet class */
Pet::Pet(Player *player, Item *item) : player(player), itemid(item->id), index(-1), name(PetDataProvider::Instance()->getName(itemid)), level(1), fullness(100), closeness(0) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO pets (name) VALUES ("<< mysqlpp::quote << this->name << ")";
	mysqlpp::SimpleResult res = query.execute();
	this->id = (int32_t) res.insert_id();
	item->petid = this->id;
}

Pet::Pet(Player *player, Item *item, int8_t index, string name, int8_t level, int16_t closeness, int8_t fullness, int8_t inventorySlot) :
player(player),
id(item->petid),
itemid(item->id),
index(index),
name(name),
level(level),
fullness(fullness),
closeness(closeness),
inventorySlot(inventorySlot) {
	if (isSummoned()) {
		if (index == 1)
			startTimer();
		player->getPets()->setSummoned(index, id);
	}
}

void Pet::levelUp() {
	level += 1;
	PetsPacket::levelUp(player, this);
}

void Pet::setName(const string &name) {
	this->name = name;
	PetsPacket::changeName(player, this);
	PetsPacket::updatePet(player, this);
}

void Pet::addCloseness(int16_t amount) {
	closeness += amount;
	if (closeness > Stats::MaxCloseness)
		closeness = Stats::MaxCloseness;

	while (closeness >= Pets::exps[level - 1] && level < Stats::PetLevels) {
		levelUp();
	}
	PetsPacket::updatePet(player, this);
}

void Pet::modifyFullness(int8_t offset, bool sendPacket) {
	fullness += offset;

	if (fullness > Stats::MaxFullness)
		fullness = Stats::MaxFullness;
	else if (fullness < Stats::MinFullness)
		fullness = Stats::MinFullness;

	if (sendPacket)
		PetsPacket::updatePet(player, this);
}

void Pet::startTimer() {
	Timer::Id id(Timer::Types::PetTimer, getIndex(), 0); // The timer will automatically stop if another pet gets inserted into this index
	clock_t length = (6 - PetDataProvider::Instance()->getHunger(getItemId())) * 60000; // TODO: Better formula
	new Timer::Timer(bind(&Pet::modifyFullness, this, -1, true), id, player->getTimers(), 0, length);
}

/* Pets namespace */
void Pets::handleMovement(Player *player, PacketReader &packet) {
	int32_t petid = packet.get<int32_t>();
	Pet *pet = player->getPets()->getPet(petid);
	packet.skipBytes(8);
	MovementHandler::parseMovement(pet, packet);
	packet.reset(10);
	PetsPacket::showMovement(player, pet, packet.getBuffer(), packet.getBufferLength() - 9);
}

void Pets::handleChat(Player *player, PacketReader &packet) {
	int32_t petid = packet.get<int32_t>();
	packet.skipBytes(5);
	int8_t act = packet.get<int8_t>();
	string message = packet.getString();
	PetsPacket::showChat(player, player->getPets()->getPet(petid), message, act);
}

void Pets::handleSummon(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int16_t slot = packet.get<int16_t>();
	bool master = packet.get<int8_t>() == 1;
	bool multipet = player->getSkills()->getSkillLevel(Jobs::Beginner::FollowTheLead) > 0;
	Pet *pet = player->getPets()->getPet(player->getInventory()->getItem(Inventories::CashInventory, slot)->petid);

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

void Pets::handleFeed(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int16_t slot = packet.get<int16_t>();
	int32_t item = packet.get<int32_t>();
	if (Pet *pet = player->getPets()->getSummoned(0)) {
		Inventory::takeItem(player, item, 1);

		bool success = (pet->getFullness() < Stats::MaxFullness);
		PetsPacket::showAnimation(player, pet, 1, success);
		if (success) {
			pet->modifyFullness(Stats::PetFeedFullness, false);
			if (Randomizer::Instance()->randInt(99) < 60) // 60% chance for feed to add closeness
				pet->addCloseness(1);
		}
	}
	else {
		InventoryPacket::blankUpdate(player);
	}
}

void Pets::handleCommand(Player *player, PacketReader &packet) {
	int32_t petid = packet.get<int32_t>();
	packet.skipBytes(5);
	int8_t act = packet.get<int8_t>();
	Pet *pet = player->getPets()->getPet(petid);
	PetInteractInfo *action = PetDataProvider::Instance()->getInteraction(pet->getItemId(), act);
	if (action == 0)
		return;
	bool success = (Randomizer::Instance()->randInt(100) < action->prob);
	if (success) {
		pet->addCloseness(action->increase);
	}
	PetsPacket::showAnimation(player, pet, act, success);
}

void Pets::changeName(Player *player, const string &name) {
	if (Pet *pet = player->getPets()->getSummoned(0)) {
		pet->setName(name);
	}
}

void Pets::showPets(Player *player) {
	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			pet->setPos(player->getPos());
			PetsPacket::petSummoned(player, pet, false, true);
		}
	}
	PetsPacket::updateSummonedPets(player);
}