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
#include "DropDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <string>

using std::string;
using Initializing::outputWidth;
using StringUtilities::runFlags;

DropDataProvider * DropDataProvider::singleton = nullptr;

void DropDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Drops... ";

	loadDrops();
	loadGlobalDrops();

	std::cout << "DONE" << std::endl;
}

namespace Functors {
	struct DropFlags {
		void operator() (const string &cmp) {
			if (cmp == "is_mesos") drop->ismesos = true;
		}
		DropInfo *drop;
	};
}

void DropDataProvider::loadDrops() {
	dropdata.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM drop_data");
	mysqlpp::UseQueryResult res = query.use();
	DropInfo drop;
	int32_t dropper;
	MYSQL_ROW row;

	using namespace Functors;

	enum DropData {
		Id = 0,
		DropperId, Flags, ItemId, Minimum, Maximum,
		Quest, Chance
	};

	while (row = res.fetch_raw_row()) {
		drop = DropInfo();
		DropFlags whoo = {&drop};
		runFlags(row[Flags], whoo);

		dropper = atoi(row[DropperId]);
		drop.itemid = atoi(row[ItemId]);
		drop.minamount = atoi(row[Minimum]);
		drop.maxamount = atoi(row[Maximum]);
		drop.questid = atoi(row[Quest]);
		drop.chance = atoi(row[Chance]);
		dropdata[dropper].push_back(drop);
	}

	query << "SELECT * FROM user_drop_data ORDER BY dropperid";
	res = query.use();
	int32_t lastdropperid = -1;
	bool dropped = false;

	while (row = res.fetch_raw_row()) {
		drop = DropInfo();
		DropFlags whoo = {&drop};
		runFlags(row[Flags], whoo);

		dropper = atoi(row[DropperId]);
		if (dropper != lastdropperid) {
			dropped = false;
		}
		drop.itemid = atoi(row[ItemId]);
		drop.minamount = atoi(row[Minimum]);
		drop.maxamount = atoi(row[Maximum]);
		drop.questid = atoi(row[Quest]);
		drop.chance = atoi(row[Chance]);
		if (!dropped && dropdata.find(dropper) != dropdata.end()) {
			dropdata.erase(dropper);
			dropped = true;
		}
		dropdata[dropper].push_back(drop);
		lastdropperid = dropper;
	}
}

namespace Functors {
	struct GlobalDropFlags {
		void operator() (const string &cmp) {
			if (cmp == "is_mesos") drop->ismesos = true;
		}
		GlobalDrop *drop;
	};
}

void DropDataProvider::loadGlobalDrops() {
	globaldrops.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM drop_global_data");
	mysqlpp::UseQueryResult res = query.use();
	GlobalDrop drop;

	using namespace Functors;

	enum DropData {
		Id = 0,
		Continent, Flags, ItemId, MinLevel, MaxLevel,
		Minimum, Maximum, Quest, Chance
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		drop = GlobalDrop();
		GlobalDropFlags whoo = {&drop};
		runFlags(row[Flags], whoo);

		drop.continent = atoi(row[Continent]);
		drop.itemid = atoi(row[ItemId]);
		drop.minamount = atoi(row[Minimum]);
		drop.maxamount = atoi(row[Maximum]);
		drop.minlevel = atoi(row[MinLevel]);
		drop.maxlevel = atoi(row[MaxLevel]);
		drop.questid = atoi(row[Quest]);
		drop.chance = atoi(row[Chance]);
		globaldrops.push_back(drop);
	}
}