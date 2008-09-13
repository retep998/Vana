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
#ifndef PETS_H
#define PETS_H

#include "MovableLife.h"
#include <unordered_map>
#include <string>

using std::string;
using std::tr1::unordered_map;

class Pet;
class Player;
class ReadPacket;
class MovableLife;
struct Item;

struct PetInfo {
	string name;
	int32_t hunger;
};

struct PetInteractInfo {
	uint32_t prob;
	int16_t increase;
};

namespace Pets {
	extern unordered_map<int32_t, PetInfo> petsInfo;
	extern unordered_map<int32_t, unordered_map<int32_t, PetInteractInfo>> petsInteractInfo;
	extern int16_t exps[29];
	void showPets(Player *player);
	void chat(Player *player, ReadPacket *packet);
	void feedPet(Player *player, ReadPacket *packet);
	void movePet(Player *player, ReadPacket *packet);
	void lootItem(Player *player, ReadPacket *packet);
	void summonPet(Player *player, ReadPacket *packet);
	void showAnimation(Player *player, ReadPacket *packet);
	void summon(Player *player, Pet *pet, bool master);
	void changeName(Player *player, const string &name);
	void addCloseness(Player *player, Pet *pet, int16_t closeness);
};

class Pet : public MovableLife {
public:
	Pet(Player *player) : player(player) {}
	Pet(Player *player, Item *item);
	void setIndex(char index) { this->index = index; }
	char getIndex() { return this->index; }
	void setName(const string &name) { this->name = name; }
	string getName() { return this->name; }
	void setType(int32_t type) { this->type = type; }
	int32_t getType() { return this->type; }
	void setId(int32_t id) { this->id = id; }
	int32_t getId() { return this->id; }
	void setFullness(char fullness) { this->fullness = fullness; }
	char getFullness() { return this->fullness; }
	bool isSummoned() {	return this->summoned; }
	void setSummoned(bool summoned) { this->summoned = summoned; }
	void setLevel(char level) {	this->level = level; }
	char getLevel() { return this->level; }
	int16_t getCloseness() { return this->closeness; }
	void setCloseness(int16_t closeness) { this->closeness = closeness; }
	void setInventorySlot(char slot) { this->inventorySlot = slot; }
	char getInventorySlot() { return this->inventorySlot; }

	void reduceFullness();
	void startTimer();
private:
	string name;
	int32_t id;
	int32_t type;
	char index;
	char level;
	char fullness;
	char inventorySlot;
	int16_t closeness;
	bool summoned;
	Player *player;
};

#endif
