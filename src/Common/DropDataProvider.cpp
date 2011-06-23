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
using Initializing::OutputWidth;
using StringUtilities::runFlags;

DropDataProvider * DropDataProvider::singleton = nullptr;

void DropDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Drops... ";

	loadDrops();
	loadGlobalDrops();

	std::cout << "DONE" << std::endl;
}

namespace Functors {
	struct DropFlags {
		void operator() (const string &cmp) {
			if (cmp == "is_mesos") drop->isMesos = true;
		}
		DropInfo *drop;
	};
}

void DropDataProvider::loadDrops() {
	m_dropInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM drop_data");
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
		drop.itemId = atoi(row[ItemId]);
		drop.minAmount = atoi(row[Minimum]);
		drop.maxAmount = atoi(row[Maximum]);
		drop.questId = atoi(row[Quest]);
		drop.chance = atoi(row[Chance]);
		m_dropInfo[dropper].push_back(drop);
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
		drop.itemId = atoi(row[ItemId]);
		drop.minAmount = atoi(row[Minimum]);
		drop.maxAmount = atoi(row[Maximum]);
		drop.questId = atoi(row[Quest]);
		drop.chance = atoi(row[Chance]);
		if (!dropped && m_dropInfo.find(dropper) != m_dropInfo.end()) {
			m_dropInfo.erase(dropper);
			dropped = true;
		}
		m_dropInfo[dropper].push_back(drop);
		lastdropperid = dropper;
	}
}

namespace Functors {
	struct GlobalDropFlags {
		void operator() (const string &cmp) {
			if (cmp == "is_mesos") drop->isMesos = true;
		}
		GlobalDrop *drop;
	};
}

void DropDataProvider::loadGlobalDrops() {
	m_globalDrops.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM drop_global_data");
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
		drop.itemId = atoi(row[ItemId]);
		drop.minAmount = atoi(row[Minimum]);
		drop.maxAmount = atoi(row[Maximum]);
		drop.minLevel = atoi(row[MinLevel]);
		drop.maxLevel = atoi(row[MaxLevel]);
		drop.questId = atoi(row[Quest]);
		drop.chance = atoi(row[Chance]);
		m_globalDrops.push_back(drop);
	}
}