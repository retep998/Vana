/*
Copyright (C) 2008-2012 Vana Development Team

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
#include <iomanip>
#include <iostream>
#include <string>

using std::string;
using Initializing::OutputWidth;
using StringUtilities::runFlags;

ReactorDataProvider * ReactorDataProvider::singleton = nullptr;

void ReactorDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Reactors... ";

	loadReactors();
	loadStates();
	loadTriggerSkills();

	std::cout << "DONE" << std::endl;
}

void ReactorDataProvider::loadReactors() {
	m_reactorInfo.clear();
	ReactorData reactor;
	int32_t id;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM reactor_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		id = row.get<int32_t>("reactorid");
		reactor = ReactorData();
		runFlags(row.get<opt_string>("flags"), [&reactor](const string &cmp) {
			if (cmp == "remove_in_field_set") reactor.removeInFieldSet = true;
			else if (cmp == "activate_by_touch") reactor.activateByTouch = true;
		});

		reactor.maxStates = row.get<int8_t>("max_states");
		reactor.link = row.get<int32_t>("link");

		m_reactorInfo[id] = reactor;
	}
}

void ReactorDataProvider::loadStates() {
	ReactorStateInfo state;
	int32_t id;
	int8_t stateId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM reactor_events ORDER BY reactorId, state ASC");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		id = row.get<int32_t>("reactorid");
		stateId = row.get<int8_t>("state");
		state = ReactorStateInfo();

		runFlags(row.get<opt_string>("event_type"), [&state](const string &cmp) {
			if (cmp == "plain_advance_state") state.type = 0;
			else if (cmp == "no_clue") state.type = 0;
			else if (cmp == "no_clue2") state.type = 0;
			else if (cmp == "hit_from_left") state.type = 2;
			else if (cmp == "hit_from_right") state.type = 3;
			else if (cmp == "hit_by_skill") state.type = 5;
			else if (cmp == "hit_by_item") state.type = 100;
		});

		state.itemId = row.get<int32_t>("itemid");
		state.itemQuantity = row.get<int16_t>("quantity");
		state.lt = Pos(row.get<int16_t>("ltx"), row.get<int16_t>("lty"));
		state.rb = Pos(row.get<int16_t>("rbx"), row.get<int16_t>("rby"));
		state.nextState = row.get<int8_t>("next_state");
		state.timeout = row.get<int32_t>("timeout");

		m_reactorInfo[id].states[stateId].push_back(state);
	}
}

void ReactorDataProvider::loadTriggerSkills() {
	int32_t id;
	int8_t state;
	int32_t skillId;
	size_t j;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM reactor_event_trigger_skills");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		id = row.get<int32_t>("reactorid");
		state = row.get<int8_t>("state");
		skillId = row.get<int32_t>("skillid");

		for (j = 0; j < m_reactorInfo[id].states[state].size(); ++j) {
			m_reactorInfo[id].states[state][j].triggerSkills.push_back(skillId);
		}
	}
}

ReactorData * ReactorDataProvider::getReactorData(int32_t reactorId, bool respectLink) {
	if (m_reactorInfo.find(reactorId) !=m_reactorInfo.end()) {
		ReactorData *retval = &m_reactorInfo[reactorId];
		if (respectLink && retval->link) {
			return &m_reactorInfo[retval->link];
		}
		return retval;
	}
	return nullptr;
}