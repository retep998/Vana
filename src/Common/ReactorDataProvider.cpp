/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "ReactorDataProvider.hpp"
#include "Database.hpp"
#include "InitializeCommon.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>
#include <string>

auto ReactorDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Reactors... ";

	loadReactors();
	loadStates();
	loadTriggerSkills();

	std::cout << "DONE" << std::endl;
}

auto ReactorDataProvider::loadReactors() -> void {
	m_reactorInfo.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM reactor_data");

	for (const auto &row : rs) {
		ReactorData reactor;
		int32_t id = row.get<int32_t>("reactorid");
		reactor.maxStates = row.get<int8_t>("max_states");
		reactor.link = row.get<int32_t>("link");

		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&reactor](const string_t &cmp) {
			if (cmp == "remove_in_field_set") reactor.removeInFieldSet = true;
			else if (cmp == "activate_by_touch") reactor.activateByTouch = true;
		});

		m_reactorInfo[id] = reactor;
	}
}

auto ReactorDataProvider::loadStates() -> void {
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM reactor_events ORDER BY reactorId, state ASC");

	for (const auto &row : rs) {
		ReactorStateInfo state;
		int32_t id = row.get<int32_t>("reactorid");
		int8_t stateId = row.get<int8_t>("state");
		state.itemId = row.get<int32_t>("itemid");
		state.itemQuantity = row.get<int16_t>("quantity");
		state.dimensions = Rect(Pos(row.get<int16_t>("ltx"), row.get<int16_t>("lty")),
								Pos(row.get<int16_t>("rbx"), row.get<int16_t>("rby")));
		state.nextState = row.get<int8_t>("next_state");
		state.timeout = row.get<int32_t>("timeout");

		StringUtilities::runEnum(row.get<string_t>("event_type"), [&state](const string_t &cmp) {
			if (cmp == "plain_advance_state") state.type = 0;
			else if (cmp == "no_clue") state.type = 0;
			else if (cmp == "no_clue2") state.type = 0;
			else if (cmp == "hit_from_left") state.type = 2;
			else if (cmp == "hit_from_right") state.type = 3;
			else if (cmp == "hit_by_skill") state.type = 5;
			else if (cmp == "hit_by_item") state.type = 100;
		});

		m_reactorInfo[id].states[stateId].push_back(state);
	}
}

auto ReactorDataProvider::loadTriggerSkills() -> void {
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM reactor_event_trigger_skills");

	for (const auto &row : rs) {
		int32_t id = row.get<int32_t>("reactorid");
		int8_t state = row.get<int8_t>("state");
		int32_t skillId = row.get<int32_t>("skillid");

		for (size_t j = 0; j < m_reactorInfo[id].states[state].size(); ++j) {
			m_reactorInfo[id].states[state][j].triggerSkills.push_back(skillId);
		}
	}
}

auto ReactorDataProvider::getReactorData(int32_t reactorId, bool respectLink) const -> const ReactorData & {
	auto kvp = m_reactorInfo.find(reactorId);
	if (respectLink && kvp->second.link != 0) {
		kvp = m_reactorInfo.find(kvp->second.link);
	}
	return kvp->second;
}