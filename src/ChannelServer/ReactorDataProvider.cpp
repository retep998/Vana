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
#include "ReactorDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "MiscUtilities.h"

using Initializing::outputWidth;
using MiscUtilities::atob;

ReactorDataProvider * ReactorDataProvider::singleton = 0;

void ReactorDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Reactors... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM reactoreventdata ORDER BY reactorid, state ASC");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW reactorRow;
	ReactorEventInfo revent;

	enum ReactorEvent {
		RowId = 0,
		ReactorId, State, Type, ItemId, LTX,
		LTY, RBX, RBY, NextState, Repeat,
		Timeout
	};

	while (reactorRow = res.fetch_raw_row()) {
		int32_t id = atoi(reactorRow[ReactorId]);
		int8_t state = atoi(reactorRow[State]);

		revent.type = atoi(reactorRow[Type]);
		revent.itemid = atoi(reactorRow[ItemId]);
		revent.lt.x = atoi(reactorRow[LTX]);
		revent.lt.y = atoi(reactorRow[LTY]);
		revent.rb.x = atoi(reactorRow[RBX]);
		revent.rb.y = atoi(reactorRow[RBY]);
		revent.nextstate = atoi(reactorRow[NextState]);
		revent.repeat = atob(reactorRow[Repeat]);
		revent.timeout = atoi(reactorRow[Timeout]);

		if (maxstates.find(id) == maxstates.end()) {
			maxstates[id] = revent.nextstate;	
		}
		else if (maxstates[id] < revent.nextstate) {
			maxstates[id] = revent.nextstate;
		}
		reactorinfo[id][state].push_back(revent);
	}

	std::cout << "DONE" << std::endl;
}

int8_t ReactorDataProvider::getEventCount(int32_t reactorid, int8_t state) {
	if (reactorinfo.find(reactorid) != reactorinfo.end()) {
		if (reactorinfo[reactorid].find(state) != reactorinfo[reactorid].end()) {
			return reactorinfo[reactorid][state].size();
		}
		return 0;
	}
	return 0;
}

ReactorEventInfo * ReactorDataProvider::getEvent(int32_t reactorid, int8_t state, int8_t specific) {
	if (reactorinfo.find(reactorid) != reactorinfo.end()) {
		if (reactorinfo[reactorid].find(state) != reactorinfo[reactorid].end()) {
			return &reactorinfo[reactorid][state][specific];
		}
		return 0;
	}
	return 0;
}