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
#include "PetDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "MiscUtilities.h"

using Initializing::outputWidth;

PetDataProvider * PetDataProvider::singleton = 0;

void PetDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Pets... ";

	mysqlpp::Query query = Database::getDataDB().query("SELECT id, name, hunger FROM petdata");
	mysqlpp::UseQueryResult res = query.use();
	petsInfo.clear();

	MYSQL_ROW Row;
	PetInfo pet;

	while (Row = res.fetch_raw_row()) {
		// 0 : Pet id
		// 1 : Pet breed name
		// 2 : Pet hunger level

		pet.name = Row[1];
		pet.hunger = atoi(Row[2]);
		petsInfo[atoi(Row[0])] = pet;
	}
	
	// Pet command info
	query << "SELECT * FROM petinteractdata";
	res = query.use();
	PetInteractInfo petinteract;
	petsInteractInfo.clear();

	while (Row = res.fetch_raw_row()) {
		// 0 : Id
		// 1 : Command
		// 2 : Increase
		// 3 : Prob

		petinteract.increase = atoi(Row[2]);
		petinteract.prob = atoi(Row[3]);
		petsInteractInfo[atoi(Row[0])][atoi(Row[1])] = petinteract;
	}

	std::cout << "DONE" << std::endl;
}

PetInteractInfo * PetDataProvider::getInteraction(int32_t itemid, int32_t action) {
	if (petsInteractInfo.find(itemid) != petsInteractInfo.end()) {
		if (petsInteractInfo[itemid].find(action) != petsInteractInfo[itemid].end()) {
			return &petsInteractInfo[itemid][action];
		}
		return 0;
	}
	return 0;
}