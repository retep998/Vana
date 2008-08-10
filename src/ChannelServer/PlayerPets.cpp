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
#include "MySQLM.h"
#include "PlayerPets.h"

void PlayerPets::addPet(Pet *pet) {
	if (playerpets.find(pet->getId()) == playerpets.end())
		playerpets[pet->getId()] = pet;
}

Pet * PlayerPets::getPet(int petid) {
	if (playerpets.find(petid) != playerpets.end())
		return playerpets[petid];
	else
		return 0;
}

void PlayerPets::setSummoned(int petid, char index) {
	summoned[index] = petid;
}

int PlayerPets::getSummoned(char index) {
	if (summoned[index])
		return summoned[index];
	else
		return 0;
}

int PlayerPets::getPetAmount() {
	return playerpets.size();
}

Pet * PlayerPets::getPetByIndex(int index) {
	unsigned int i = 0;
	for (hash_map<int, Pet *>::iterator iter = playerpets.begin(); iter != playerpets.end(); iter++) {
		if (i == index)
			return iter->second;
		else
			i = i + 1;
	}
	return 0;
}

void PlayerPets::save() {
	Pet *pet;
	mysqlpp::Query query = Database::chardb.query();
	for (int i = 0; i < getPetAmount(); i++) {
		pet = getPetByIndex(i);
		query << "UPDATE pets SET "
			<< "`index` = "	 << mysqlpp::quote << (short) pet->getIndex() << ","
			<< "name = "		 << mysqlpp::quote << pet->getName() << ","
			<< "level = "		 << mysqlpp::quote << (short) pet->getLevel() << ","
			<< "closeness = "  << mysqlpp::quote << pet->getCloseness() << ","
			<< "fullness = "   << mysqlpp::quote << (short) pet->getFullness()
			<< " WHERE id = "	 << mysqlpp::quote << pet->getId();
		query.exec();
	}
}
