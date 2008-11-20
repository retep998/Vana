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
#include "DropDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "MiscUtilities.h"

using MiscUtilities::atob;
using Initializing::outputWidth;

DropDataProvider *DropDataProvider::singleton = 0;

void DropDataProvider::loadData() {
	// Mob/Reactor drops
	std::cout << std::setw(outputWidth) << std::left << "Initializing Drops... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT mobid, ismesos, itemid, min, max, questid, chance FROM dropdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW dropRow;
	DropInfo drop;
	while ((dropRow = res.fetch_raw_row())) {
		// Col0 : Mob ID
		//    1 : Is Mesos?
		//    2 : Item ID
		//    3 : Minimum Amount (of mesos or of item)
		//    4 : Maximum Amount
		//    5 : Quest ID
		//    6 : Chance of dropping
		drop.ismesos = atob(dropRow[1]);
		drop.itemid = atoi(dropRow[2]);
		drop.minamount = atoi(dropRow[3]);
		drop.maxamount = atoi(dropRow[4]);
		drop.questid = atoi(dropRow[5]);
		drop.chance = atoi(dropRow[6]);
		dropdata[atoi(dropRow[0])].push_back(drop);
	}

	std::cout << "DONE" << std::endl;
}
