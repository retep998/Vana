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
#include "Pet.hpp"
#include "ChannelServer.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "ItemDataProvider.hpp"
#include "Map.hpp"
#include "PetsPacket.hpp"
#include "Player.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include <functional>

namespace Vana {

Pet::Pet(Player *player, Item *item) :
	MovableLife{0, Point{}, 0},
	m_player{player},
	m_itemId{item->getId()},
	m_name{ChannelServer::getInstance().getItemDataProvider().getItemInfo(m_itemId)->name},
	m_item{item}
{
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();

	sql.once << "INSERT INTO " << db.makeTable("pets") << " (name) VALUES (:name)", soci::use(m_name, "name");

	m_id = db.getLastId<pet_id_t>();
	item->setPetId(m_id);
}

Pet::Pet(Player *player, Item *item, const soci::row &row) :
	MovableLife{0, Point{}, 0},
	m_player{player},
	m_id{item->getPetId()},
	m_itemId{item->getId()},
	m_item{item}
{
	initializePet(row);
	if (isSummoned()) {
		if (m_index.is_initialized() && m_index.get() == 1) {
			startTimer();
		}
		player->getPets()->setSummoned(m_index.get(), m_id);
	}
}

auto Pet::levelUp() -> void {
	m_level += 1;
	m_player->sendMap(Packets::Pets::levelUp(m_player->getId(), this));
}

auto Pet::setName(const string_t &name) -> void {
	m_name = name;
	m_player->sendMap(Packets::Pets::changeName(m_player->getId(), this));
}

auto Pet::addCloseness(int16_t amount) -> void {
	m_closeness += amount;
	if (m_closeness > Stats::MaxCloseness) {
		m_closeness = Stats::MaxCloseness;
	}
	while (m_closeness >= Stats::PetExp[m_level - 1] && m_level < Stats::PetLevels) {
		levelUp();
	}

	m_player->send(Packets::Pets::updatePet(this, m_item));
}

auto Pet::modifyFullness(int8_t offset, bool sendPacket) -> void {
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
		m_player->send(Packets::Pets::updatePet(this, m_item));
	}
}

auto Pet::startTimer() -> void {
	Timer::Id id{TimerType::PetTimer, getIndex().get()}; // The timer will automatically stop if another pet gets inserted into this index
	duration_t repeat = seconds_t{(6 - ChannelServer::getInstance().getItemDataProvider().getPetInfo(getItemId())->hunger) * 60}; // TODO FIXME formula
	Timer::Timer::create(
		[this](const time_point_t &now) {
			this->modifyFullness(-1, true);
		},
		id,
		m_player->getTimerContainer(),
		seconds_t{0},
		repeat);
}

auto Pet::hasNameTag() const -> bool {
	if (m_index.is_initialized()) {
		switch (m_index.get()) {
			case 0: return m_player->getInventory()->getEquippedId(EquipSlots::PetLabelRing1, true) != 0;
			case 1: return m_player->getInventory()->getEquippedId(EquipSlots::PetLabelRing2, true) != 0;
			case 2: return m_player->getInventory()->getEquippedId(EquipSlots::PetLabelRing3, true) != 0;
		}
	}
	return false;
}

auto Pet::hasQuoteItem() const -> bool {
	if (m_index.is_initialized()) {
		switch (m_index.get()) {
			case 0: return m_player->getInventory()->getEquippedId(EquipSlots::PetQuoteRing1, true) != 0;
			case 1: return m_player->getInventory()->getEquippedId(EquipSlots::PetQuoteRing2, true) != 0;
			case 2: return m_player->getInventory()->getEquippedId(EquipSlots::PetQuoteRing3, true) != 0;
		}
	}
	return false;
}

auto Pet::initializePet(const soci::row &row) -> void {
	m_index = row.get<opt_int8_t>("index");
	m_name = row.get<string_t>("pet_name");
	m_level = row.get<int8_t>("level");
	m_closeness = row.get<int16_t>("closeness");
	m_fullness = row.get<int8_t>("fullness");
	m_inventorySlot = row.get<int8_t>("slot");
}

}