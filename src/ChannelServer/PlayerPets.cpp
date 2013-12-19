/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "PlayerPets.h"
#include "Pet.h"
#include "Database.h"
#include "PacketCreator.h"
#include "Player.h"

void PlayerPets::addPet(Pet *pet) {
	m_pets[pet->getId()] = pet;

	if (pet->isSummoned()) {
		setSummoned(pet->getIndex().get(), pet->getId());
	}
}

Pet * PlayerPets::getPet(int64_t petId) {
	return m_pets.find(petId) != m_pets.end() ? m_pets[petId] : nullptr;
}

void PlayerPets::setSummoned(int8_t index, int64_t petId) {
	m_summoned[index] = petId;
}

Pet * PlayerPets::getSummoned(int8_t index) {
	return m_summoned[index] > 0 ? m_pets[m_summoned[index]] : nullptr;
}

void PlayerPets::save() {
	if (m_pets.size() > 0) {
		soci::session &sql = Database::getCharDb();
		opt_int8_t index = 0;
		string name = "";
		int8_t level = 0;
		int16_t closeness = 0;
		int8_t fullness = 0;
		int64_t petId = 0;

		soci::statement st = (sql.prepare
			<< "UPDATE pets "
			<< "SET "
			<< "	`index` = :index, "
			<< "	name = :name, "
			<< "	level = :level, "
			<< "	closeness = :closeness, "
			<< "	fullness = :fullness "
			<< "WHERE pet_id = :pet",
			soci::use(petId, "pet"),
			soci::use(index, "index"),
			soci::use(name, "name"),
			soci::use(level, "level"),
			soci::use(closeness, "closeness"),
			soci::use(fullness, "fullness"));

		for (const auto &kvp : m_pets) {
			Pet *p = kvp.second;
			index = p->getIndex();
			name = p->getName();
			level = p->getLevel();
			closeness = p->getCloseness();
			fullness = p->getFullness();
			petId = p->getId();
			st.execute(true);
		}
	}
}

void PlayerPets::petInfoPacket(PacketCreator &packet) {
	Item *it;
	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = getSummoned(i)) {
			packet.add<int8_t>(1);
			packet.add<int32_t>(pet->getItemId());
			packet.addString(pet->getName());
			packet.add<int8_t>(pet->getLevel());
			packet.add<int16_t>(pet->getCloseness());
			packet.add<int8_t>(pet->getFullness());
			packet.add<int16_t>(0);
			int16_t slot = 0;
			switch (i) {
				case 0: slot = EquipSlots::PetEquip1;
				case 1: slot = EquipSlots::PetEquip2;
				case 2: slot = EquipSlots::PetEquip3;
			}

			it = m_player->getInventory()->getItem(Inventories::EquipInventory, slot);
			packet.add<int32_t>(it != nullptr ? it->getId() : 0);
		}
	}
	packet.add<int8_t>(0); // End of pets / start of taming mob
}

void PlayerPets::connectData(PacketCreator &packet) {
	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = getSummoned(i)) {
			packet.add<int64_t>(pet->getId()); //pet->getCashId() != 0 ? pet->getCashId() : pet->getId());
		}
		else {
			packet.add<int64_t>(0);
		}
	}
}