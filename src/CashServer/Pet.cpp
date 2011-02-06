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
#include "Pet.h"
#include "Database.h"
#include "GameConstants.h"
#include "ItemDataProvider.h"
#include "PetsPacket.h"
#include "Player.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

Pet::Pet(Player *player, Item *item) :
player(player), itemid(item->getId()),
index(-1),
name(ItemDataProvider::Instance()->getItemName(itemid)),
level(1),
fullness(100),
closeness(0)
{
	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO pets (name) VALUES ("<< mysqlpp::quote << this->name << ")";
	mysqlpp::SimpleResult res = query.execute();
	this->id = (int32_t) res.insert_id();
	item->setPetId(this->id);
	this->cashId = item->getCashId();
}

Pet::Pet(Player *player, Item *item, int8_t index, const string &name, int8_t level, int16_t closeness, int8_t fullness, int8_t inventorySlot) :
player(player),
id(item->getPetId()),
itemid(item->getId()),
cashId(item->getCashId()),
index(index),
name(name),
level(level),
fullness(fullness),
closeness(closeness),
inventorySlot(inventorySlot)
{
	if (isSummoned()) {
		if (index == 1)
			startTimer();
		player->getPets()->setSummoned(index, id);
	}
}

void Pet::levelUp() {
	level += 1;
}

void Pet::setName(const string &name) {
	this->name = name;
}

void Pet::addCloseness(int16_t amount) {
	closeness += amount;
	if (closeness > Stats::MaxCloseness)
		closeness = Stats::MaxCloseness;

	while (closeness >= Stats::PetExp[level - 1] && level < Stats::PetLevels) {
		levelUp();
	}
}

void Pet::modifyFullness(int8_t offset, bool sendPacket) {
	fullness += offset;

	if (fullness > Stats::MaxFullness)
		fullness = Stats::MaxFullness;
	else if (fullness < Stats::MinFullness)
		fullness = Stats::MinFullness;
}

void Pet::startTimer() {
	Timer::Id id(Timer::Types::PetTimer, getIndex(), 0); // The timer will automatically stop if another pet gets inserted into this index
	clock_t length = (6 - ItemDataProvider::Instance()->getHunger(getItemId())) * 60000; // TODO: Better formula
	new Timer::Timer(bind(&Pet::modifyFullness, this, -1, true), id, player->getTimers(), 0, length);
}
