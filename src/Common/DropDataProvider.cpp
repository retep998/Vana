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
#include "DropDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <iomanip>
#include <iostream>
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

void DropDataProvider::loadDrops() {
	m_dropInfo.clear();
	DropInfo drop;
	int32_t dropper;

	auto dropFlags = [&drop](const opt_string &flags) {
		// Inner lambda workaround for VS2010
		auto &d = drop;
		runFlags(flags, [&d](const string &cmp) {
			if (cmp == "is_mesos") d.isMesos = true;
		});
	};

	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM drop_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		const soci::row &row = *i;

		drop = DropInfo();
		dropFlags(row.get<opt_string>("flags"));

		dropper = row.get<int32_t>("dropperid");
		drop.itemId = row.get<int32_t>("itemid");
		drop.minAmount = row.get<int32_t>("minimum_quantity");
		drop.maxAmount = row.get<int32_t>("maximum_quantity");
		drop.questId = row.get<int16_t>("questid");
		drop.chance = row.get<uint32_t>("chance");
		m_dropInfo[dropper].push_back(drop);
	}

	rs = (sql.prepare << "SELECT * FROM user_drop_data ORDER BY dropperid");
	int32_t lastDropperId = -1;
	bool dropped = false;

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		const soci::row &row = *i;

		drop = DropInfo();
		dropFlags(row.get<opt_string>("flags"));

		dropper = row.get<int32_t>("dropperid");
		if (dropper != lastDropperId) {
			dropped = false;
		}
		drop.itemId = row.get<int32_t>("itemid");
		drop.minAmount = row.get<int32_t>("minimum_quantity");
		drop.maxAmount = row.get<int32_t>("maximum_quantity");
		drop.questId = row.get<int16_t>("questid");
		drop.chance = row.get<uint32_t>("chance");
		if (!dropped && m_dropInfo.find(dropper) != m_dropInfo.end()) {
			m_dropInfo.erase(dropper);
			dropped = true;
		}
		m_dropInfo[dropper].push_back(drop);
		lastDropperId = dropper;
	}
}

void DropDataProvider::loadGlobalDrops() {
	m_globalDrops.clear();
	GlobalDrop drop;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM drop_global_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		const soci::row &row = *i;

		drop = GlobalDrop();
		runFlags(row.get<opt_string>("flags"), [&drop](const string &cmp) {
			if (cmp == "is_mesos") drop.isMesos = true;
		});

		drop.continent = row.get<int8_t>("continent");
		drop.itemId = row.get<int32_t>("itemid");
		drop.minAmount = row.get<int16_t>("minimum_quantity");
		drop.maxAmount = row.get<int16_t>("maximum_quantity");
		drop.minLevel = row.get<uint8_t>("minimum_level");
		drop.maxLevel = row.get<uint8_t>("maximum_level");
		drop.questId = row.get<int16_t>("questid");
		drop.chance = row.get<uint32_t>("chance");
		m_globalDrops.push_back(drop);
	}
}