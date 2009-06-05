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
#include "PlayerPets.h"

void PlayerPets::addPet(Pet *pet) {
	playerpets[pet->getId()] = pet;

	if (pet->getIndex() != -1) {
		setSummoned(pet->getIndex(), pet->getId());
	}
}

Pet * PlayerPets::getPet(int32_t petid) {
	return playerpets.find(petid) != playerpets.end() ? playerpets[petid] : 0;
}

void PlayerPets::setSummoned(int8_t index, int32_t petid) {
	summoned[index] = petid;
}

Pet * PlayerPets::getSummoned(int8_t index) {
	return summoned[index] > 0 ? playerpets[summoned[index]] : 0;
}

void PlayerPets::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	for (unordered_map<int32_t, Pet *>::iterator iter = playerpets.begin(); iter != playerpets.end(); iter++) {
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
