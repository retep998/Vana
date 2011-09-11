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
#include "TimeUtilities.h"
#include "Timer.h"
#include <functional>

using std::tr1::bind;

Pet::Pet(Player *player, Item *item) :
	m_player(player),
	m_itemId(item->getId()),
	m_index(-1),
	m_name(ItemDataProvider::Instance()->getItemName(m_itemId)),
	m_level(1),
	m_fullness(Stats::MaxFullness),
	m_closeness(0)
{
	mysqlpp::Query query = Database::getCharDb().query();
	query << "INSERT INTO pets (name) VALUES (" << mysqlpp::quote << m_name << ")";
	mysqlpp::SimpleResult res = query.execute();
	m_id = static_cast<int64_t>(res.insert_id());
	item->setPetId(m_id);
}

Pet::Pet(Player *player, Item *item, int8_t index, const string &name, int8_t level, int16_t closeness, int8_t fullness, int8_t inventorySlot) :
	m_player(player),
	m_id(item->getPetId()),
	m_itemId(item->getId()),
	m_index(index),
	m_name(name),
	m_level(level),
	m_fullness(fullness),
	m_closeness(closeness),
	m_inventorySlot(inventorySlot)
{
	if (isSummoned()) {
		if (m_index == 1) {
			startTimer();
		}
		player->getPets()->setSummoned(m_index, m_id);
	}
}

void Pet::levelUp() {
	m_level += 1;
	PetsPacket::levelUp(m_player, this);
}

void Pet::setName(const string &name) {
	m_name = name;
	PetsPacket::changeName(m_player, this);
}

void Pet::addCloseness(int16_t amount) {
	m_closeness += amount;
	if (m_closeness > Stats::MaxCloseness) {
		m_closeness = Stats::MaxCloseness;
	}
	while (m_closeness >= Stats::PetExp[m_level - 1] && m_level < Stats::PetLevels) {
		levelUp();
	}
	PetsPacket::updatePet(m_player, this);
}

void Pet::modifyFullness(int8_t offset, bool sendPacket) {
	if (m_fullness + offset > Stats::MaxFullness) {
		m_fullness = Stats::MaxFullness;
	}
	else {
		m_fullness += offset;
	}

	if (m_fullness < Stats::MinFullness) {
		m_fullness = Stats::MinFullness;
	}

	if (sendPacket) {
		PetsPacket::updatePet(m_player, this);
	}
}

void Pet::startTimer() {
	Timer::Id id(Timer::Types::PetTimer, getIndex(), 0); // The timer will automatically stop if another pet gets inserted into this index
	clock_t length = (6 - ItemDataProvider::Instance()->getHunger(getItemId())) * 60000; // TODO: Better formula
	new Timer::Timer(bind(&Pet::modifyFullness, this, -1, true), id, m_player->getTimers(), 0, length);
}

bool Pet::hasNameTag() const {
	switch (m_index) {
		case 0: return m_player->getInventory()->getEquippedId(EquipSlots::PetLabelRing1, true) != 0;
		case 1: return m_player->getInventory()->getEquippedId(EquipSlots::PetLabelRing2, true) != 0;
		case 2: return m_player->getInventory()->getEquippedId(EquipSlots::PetLabelRing3, true) != 0;
	}
	return false;
}

bool Pet::hasQuoteItem() const {
	switch (m_index) {
		case 0: return m_player->getInventory()->getEquippedId(EquipSlots::PetQuoteRing1, true) != 0;
		case 1: return m_player->getInventory()->getEquippedId(EquipSlots::PetQuoteRing2, true) != 0;
		case 2: return m_player->getInventory()->getEquippedId(EquipSlots::PetQuoteRing3, true) != 0;
	}
	return false;
}