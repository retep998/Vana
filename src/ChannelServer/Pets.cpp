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
#include "Map.h"
#include "Maps.h"
#include "MySQLM.h"
#include "PetsPacket.h"
#include "Player.h"
#include "Pos.h"
#include "Randomizer.h"
#include "Reactors.h"
#include "ReadPacket.h"
#include "Timer.h"

hash_map<int, PetInfo> Pets::petsInfo;
hash_map<int, hash_map<int, PetInteractInfo>> Pets::petsInteractInfo;

short Pets::exps[29] = {1, 3, 6, 14, 31, 60, 108, 181, 287, 434, 632, 891, 1224, 1642, 2161, 2793, 3557, 4467, 5542, 6801, 8263, 9950, 11882, 14084, 16578, 19391, 22548, 26074, 30000};

class PetTimer : public Timer::TimerHandler {
public:
	static PetTimer * Instance() {
		if (singleton == 0)
			singleton = new PetTimer;
		return singleton;
	}
	void setPetTimer (Player *player, int petid, int time) {
		PTimer timer;
		timer.id = Timer::Instance()->setTimer(time, this);
		timer.player = player;
		timer.petid = petid;
		timer.time = time;
		timers.push_back(timer);
	}
	void stop (Player *player, int petid) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (player == timers[i].player && timers[i].petid == petid) {
				Timer::Instance()->cancelTimer(timers[i].id);
				break;
			}
		}
	}
	void stop (Player *player) {
		for (size_t i = timers.size(); i > 0; i--) {
			if (player == timers[i-1].player) {
				Timer::Instance()->cancelTimer(timers[i-1].id);
			}
		}
	}
private:
	static PetTimer *singleton;
	PetTimer() {};
	PetTimer(const PetTimer&);
	PetTimer& operator=(const PetTimer&);
	struct PTimer {
		int id;
		Player *player;
		int petid;
		int time;
	};
	vector <PTimer> timers;
	void handle (Timer *timer, int id) {
		int petid;
		Player *player;

		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {
				player = timers[i].player;
				petid = timers[i].petid;
				break;
			}
		}
		player->getPets()->getPet(petid)->setFullness(player->getPets()->getPet(petid)->getFullness()-1);
		PetsPacket::updatePet(player, player->getPets()->getPet(petid));
		this->setPetTimer(player, petid, (6-Pets::petsInfo[player->getPets()->getPet(petid)->getType()].hunger)*1000*60);
	}
	void remove (int id) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {
				timers.erase(timers.begin()+i);
				return;
			}
		}
	}
};

PetTimer * PetTimer::singleton = 0;

/* Pet class */
Pet::Pet(Item *item) {
	this->fullness = 100;
	this->closeness = 0;
	this->level = 1;
	this->type = item->id;
	this->summoned = false;
	this->name = Pets::petsInfo[type].name;
	this->index = -1;
	mysqlpp::Query query = Database::chardb.query();
	query << "INSERT INTO pets (name) VALUES ("<< mysqlpp::quote << this->name << ")";
	mysqlpp::SimpleResult res = query.execute();
	this->id = (int) res.insert_id();
	item->petid = this->id;
}

/* Pets namespace */
void Pets::stopTimers(Player *player) {
	PetTimer::Instance()->stop(player);
}

void Pets::movePet(Player *player, ReadPacket *packet) {
	int petid = packet->getInt();
	packet->skipBytes(4);
	PetsPacket::movePet(player, player->getPets()->getPet(petid), packet->getBuffer(), packet->getBufferLength() - 9);
}

void Pets::chat(Player *player, ReadPacket *packet) {
	int petid = packet->getInt();
	packet->skipBytes(5);
	char act = packet->getByte();
	string message = packet->getString();
	PetsPacket::showChat(player, player->getPets()->getPet(petid), message, act);
}

void Pets::summonPet(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	short slot = packet->getShort();
	bool master = packet->getByte() == 1;
	Pet *pet = player->getPets()->getPet(player->getInventory()->getItem(5, slot)->petid);
	pet->setPos(player->getPos());
	summon(player, pet, master);
}

void Pets::feedPet(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	short slot = packet->getShort();
	int item = packet->getInt();
	if (player->getPets()->getSummoned(0)) {
		Pet *pet = player->getPets()->getPet(player->getPets()->getSummoned(0));
		bool success = false;
		if (pet->getFullness() < 100) {
			if (pet->getFullness() + 30 < 100)
				pet->setFullness(pet->getFullness() + 30);
			else
				pet->setFullness(100);
			success = true;
		}
		Inventory::takeItem(player, item, 1);
		PetsPacket::showAnimation(player, pet, 1, success);
		PetsPacket::updatePet(player, pet);
	}
}

void Pets::showAnimation(Player *player, ReadPacket *packet) {
	int petid = packet->getInt();
	packet->skipBytes(5);
	char act = packet->getByte();
	Pet *pet = player->getPets()->getPet(petid);
	bool success = false;
	if (Randomizer::Instance()->randInt(100) < petsInteractInfo[pet->getType()][act].prob) {
		success = true;
		addCloseness(player, pet, petsInteractInfo[pet->getType()][act].increase);
	}
	PetsPacket::showAnimation(player, pet, act, success);
}

void Pets::changeName(Player *player, const string &name) {
	if (player->getPets()->getSummoned(0)) {
		Pet *pet = player->getPets()->getPet(player->getPets()->getSummoned(0));
		pet->setName(name);
		PetsPacket::changeName(player, pet);
		PetsPacket::updatePet(player, pet);
	}
}

void Pets::addCloseness(Player *player, Pet *pet, short closeness) {
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

void Pets::lootItem(Player *player, ReadPacket *packet) {
	int petid = packet->getInt();
	packet->skipBytes(13);
	int itemid = packet->getInt();
	Drop* drop = Maps::maps[player->getMap()]->getDrop(itemid);
	if (drop == 0) {
		DropsPacket::dontTake(player);
		return;
	}
	if (drop->isQuest()) {
		int request = 0;
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
		player->getInventory()->setMesos(player->getInventory()->getMesos() + drop->getObjectID(), true);
		DropsPacket::takeNote(player, drop->getObjectID(), true, 0);
	}
	else {
		Item *item = new Item(drop->getItem());
		short dropAmount = drop->getAmount();
		short amount = Inventory::addItem(player, item, true);
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
	for (char i = 0; i < 3; i++) {
		if (player->getPets()->getSummoned(i) != 0) {
			Pet *pet = player->getPets()->getPet(player->getPets()->getSummoned(i));
			if (pet->isSummoned()) {
				PetsPacket::petSummoned(player, pet);
				PetsPacket::showPet(player, pet);
			}
			else {
				if (pet->getIndex() == 0)
					PetTimer::Instance()->setPetTimer(player, pet->getId(), (6-petsInfo[pet->getType()].hunger)*1000*60);
				pet->setSummoned(true);
				PetsPacket::petSummoned(player, pet);
			}
		}
	}
	PetsPacket::updateSummonedPets(player);
}

void Pets::summon(Player *player, Pet *pet, bool master) {
	if (player->getSkills()->getSkillLevel(8) == 1) {
		if (pet->isSummoned()) {
			player->getPets()->setSummoned(0, pet->getIndex());
			for (char i = (pet->getIndex() + 1); i < 3; i++) {
				if (player->getPets()->getSummoned(i)) {
					Pet *move = player->getPets()->getPet(player->getPets()->getSummoned(i));
					move->setIndex(i - 1);
					player->getPets()->setSummoned(move->getId(), i - 1);
					player->getPets()->setSummoned(0, i);
					if (i - 1 == 0)
						PetTimer::Instance()->setPetTimer(player, move->getId(), (6-petsInfo[move->getType()].hunger)*1000*60);
				}
			}
			if (pet->getIndex() == 0)
				PetTimer::Instance()->stop(player, pet->getId());
			pet->setSummoned(false);
			PetsPacket::petSummoned(player, pet);
			pet->setIndex(-1);
		}
		else {
			if (master) {
				for (char k = 2; k > 0; k--) {
					if (player->getPets()->getSummoned(k - 1) && !player->getPets()->getSummoned(k)) {
						Pet *move = player->getPets()->getPet(player->getPets()->getSummoned(k - 1));
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
				for (char i = 0; i < 3; i++) {
					if (!player->getPets()->getSummoned(i)) {
						player->getPets()->setSummoned(pet->getId(), i);
						pet->setIndex(i);
						pet->setSummoned(true);
						PetsPacket::petSummoned(player, pet);
						PetTimer::Instance()->setPetTimer(player, pet->getId(), (6 - petsInfo[pet->getType()].hunger) * 1000 * 60); // TODO: Improve formula
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
			PetTimer::Instance()->stop(player, pet->getId());
		}
		else {
			pet->setIndex(0);
			pet->setSummoned(true);
			if (player->getPets()->getSummoned(0)) {
				Pet *kicked = player->getPets()->getPet(player->getPets()->getSummoned(0));
				kicked->setIndex(-1);
				kicked->setSummoned(false);
				PetTimer::Instance()->stop(player, kicked->getId());
				PetsPacket::petSummoned(player, pet, true);
			}
			else
				PetsPacket::petSummoned(player, pet);
			player->getPets()->setSummoned(pet->getId(), 0);
			PetTimer::Instance()->setPetTimer(player, pet->getId(), (6 - petsInfo[pet->getType()].hunger) * 1000 * 60);
 		}
	}
	PetsPacket::blankUpdate(player);
}
