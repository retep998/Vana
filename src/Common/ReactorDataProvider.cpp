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
#include "ReactorDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <string>

using std::string;
using Initializing::outputWidth;
using StringUtilities::runFlags;

ReactorDataProvider * ReactorDataProvider::singleton = nullptr;

void ReactorDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Reactors... ";

	loadReactors();
	loadStates();
	loadTriggerSkills();

	std::cout << "DONE" << std::endl;
}

namespace Functors {
	struct ReactorFlags {
		void operator()(const string &cmp) {
			if (cmp == "remove_in_field_set") reactor->removeinfieldset = true;
			else if (cmp == "activate_by_touch") reactor->activatebytouch = true;
		}
		ReactorData *reactor;
	};
}

void ReactorDataProvider::loadReactors() {
	reactorinfo.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM reactor_data");
	mysqlpp::UseQueryResult res = query.use();
	ReactorData react;
	int32_t id;

	using namespace Functors;

	enum Reactors {
		ReactorId = 0,
		MaxStates, Link, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		id = atoi(row[ReactorId]);
		react = ReactorData();
		ReactorFlags whoo = {&react};
		runFlags(row[Flags], whoo);

		react.maxstates = atoi(row[MaxStates]);
		react.link = atoi(row[Link]);

		reactorinfo[id] = react;
	}
}

namespace Functors {
	struct StateTypeFlags {
		void operator()(const string &cmp) {
			if (cmp == "plain_advance_state") reactor->type = 0;
			else if (cmp == "no_clue") reactor->type = 0;
			else if (cmp == "no_clue2") reactor->type = 0;
			else if (cmp == "hit_from_left") reactor->type = 2;
			else if (cmp == "hit_from_right") reactor->type = 3;
			else if (cmp == "hit_by_skill") reactor->type = 5;
			else if (cmp == "hit_by_item") reactor->type = 100;
		}
		ReactorStateInfo *reactor;
	};
}

void ReactorDataProvider::loadStates() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM reactor_events ORDER BY reactorid, state ASC");
	mysqlpp::UseQueryResult res = query.use();
	ReactorStateInfo revent;
	int32_t id;
	int8_t state;

	using namespace Functors;

	enum ReactorEvent {
		ReactorId = 0,
		State, Type, Timeout, ItemId, Quantity,
		LTX, LTY, RBX, RBY, NextState
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		id = atoi(row[ReactorId]);
		state = atoi(row[State]);
		revent = ReactorStateInfo();

		StateTypeFlags whoo = {&revent};
		runFlags(row[Type], whoo);

		revent.itemid = atoi(row[ItemId]);
		revent.itemquantity = atoi(row[Quantity]);
		revent.lt = Pos(atoi(row[LTX]), atoi(row[LTY]));
		revent.rb = Pos(atoi(row[RBX]), atoi(row[RBY]));
		revent.nextstate = atoi(row[NextState]);
		revent.timeout = atoi(row[Timeout]);

		reactorinfo[id].states[state].push_back(revent);
	}
}

void ReactorDataProvider::loadTriggerSkills() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM reactor_event_trigger_skills");
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	int8_t state;
	int32_t skillId;
	size_t i;

	enum ReactorEvent {
		ReactorId = 0,
		State, SkillId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		id = atoi(row[ReactorId]);
		state = atoi(row[State]);
		skillId = atoi(row[SkillId]);

		for (i = 0; i < reactorinfo[id].states[state].size(); i++) {
			reactorinfo[id].states[state][i].triggerskills.push_back(skillId);
		}
	}
}

ReactorData * ReactorDataProvider::getReactorData(int32_t reactorid, bool respectLink) {
	if (reactorinfo.find(reactorid) != reactorinfo.end()) {
		ReactorData *retval = &reactorinfo[reactorid];
		if (respectLink && retval->link) {
			return &reactorinfo[retval->link];
		}
		return retval;
	}
	return nullptr;
}