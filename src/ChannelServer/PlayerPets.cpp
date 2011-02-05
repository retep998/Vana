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
#include "PlayerPets.h"
#include "Database.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "Player.h"

void PlayerPets::addPet(Pet *pet) {
	m_playerpets[pet->getId()] = pet;

	if (pet->getIndex() != -1) {
		setSummoned(pet->getIndex(), pet->getId());
	}
}

Pet * PlayerPets::getPet(int32_t petid) {
	return m_playerpets.find(petid) != m_playerpets.end() ? m_playerpets[petid] : nullptr;
}

void PlayerPets::setSummoned(int8_t index, int32_t petid) {
	m_summoned[index] = petid;
}

Pet * PlayerPets::getSummoned(int8_t index) {
	return m_summoned[index] > 0 ? m_playerpets[m_summoned[index]] : nullptr;
}

void PlayerPets::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	for (unordered_map<int32_t, Pet *>::iterator iter = m_playerpets.begin(); iter != m_playerpets.end(); iter++) {
		query << "UPDATE pets SET "
			<< "`index` = " << (int16_t) iter->second->getIndex() << ","
			<< "name = " << mysqlpp::quote << iter->second->getName() << ","
			<< "level = " << (int16_t) iter->second->getLevel() << ","
			<< "closeness = " << iter->second->getCloseness() << ","
			<< "fullness = " << (int16_t) iter->second->getFullness()
			<< " WHERE id = " << iter->second->getId();
		query.exec();
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
			it = m_player->getInventory()->getItem(Inventories::EquipInventory, -114 - (i == 1 ? 16 : (i == 2 ? 24 : 0)));
			packet.add<int32_t>(it != nullptr ? it->getId() : 0);
		}
	}
	packet.add<int8_t>(0); // End of pets / start of taming mob
}