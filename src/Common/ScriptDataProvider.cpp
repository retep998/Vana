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
#include "ScriptDataProvider.hpp"
#include "Database.hpp"
#include "FileUtilities.hpp"
#include "InitializeCommon.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>
#include <string>

auto ScriptDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Scripts... ";

	m_npcScripts.clear();
	m_reactorScripts.clear();
	m_questScripts.clear();
	m_mapEntryScripts.clear();
	m_firstMapEntryScripts.clear();
	m_itemScripts.clear();
	int8_t modifier;
	int32_t objectId;
	string_t script;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM scripts");

	for (const auto &row : rs) {
		objectId = row.get<int32_t>("objectid");
		script = row.get<string_t>("script");
		modifier = row.get<int8_t>("helper");

		StringUtilities::runEnum(row.get<string_t>("script_type"), [&](const string_t &cmp) {
			if (cmp == "npc") m_npcScripts[objectId] = script;
			else if (cmp == "reactor") m_reactorScripts[objectId] = script;
			else if (cmp == "map_enter") m_mapEntryScripts[objectId] = script;
			else if (cmp == "map_first_enter") m_firstMapEntryScripts[objectId] = script;
			else if (cmp == "item") m_itemScripts[objectId] = script;
			else if (cmp == "quest") m_questScripts[static_cast<int16_t>(objectId)][modifier] = script;
		});
		m_scripts[script] = objectId;
	}

	std::cout << "DONE" << std::endl;
}

auto ScriptDataProvider::getScript(int32_t objectId, ScriptTypes::ScriptTypes type) -> string_t {
	if (hasScript(objectId, type)) {
		string_t s = "scripts/" + resolvePath(type) + "/" + resolve(type)[objectId] + ".lua";
		if (FileUtilities::fileExists(s)) {
			return s;
		}
	}
	out_stream_t filestream;
	filestream << "scripts/" << resolvePath(type) << "/" << objectId << ".lua";
	string_t g(filestream.str());
	return g;
}

auto ScriptDataProvider::getQuestScript(int16_t questId, int8_t state) -> string_t {
	if (hasQuestScript(questId, state)) {
		string_t s = "scripts/quests/" + m_questScripts[questId][state] + ".lua";
		if (FileUtilities::fileExists(s)) {
			return s;
		}
	}
	out_stream_t filestream;
	filestream << "scripts/quests/" << questId << (state == 0 ? "s" : "e") << ".lua";
	string_t g(filestream.str());
	return g;
}

auto ScriptDataProvider::hasScript(int32_t objectId, ScriptTypes::ScriptTypes type) -> bool {
	hash_map_t<int32_t, string_t> &map = resolve(type);
	return map.find(objectId) != std::end(map);
}

auto ScriptDataProvider::hasQuestScript(int16_t questId, int8_t state) -> bool {
	return m_questScripts.find(questId) != std::end(m_questScripts);
}

auto ScriptDataProvider::resolve(ScriptTypes::ScriptTypes type) -> hash_map_t<int32_t, string_t> & {
	switch (type) {
		case ScriptTypes::Item: return m_itemScripts; break;
		case ScriptTypes::MapEntry: return m_mapEntryScripts; break;
		case ScriptTypes::FirstMapEntry: return m_firstMapEntryScripts; break;
		case ScriptTypes::Npc: return m_npcScripts; break;
		case ScriptTypes::Reactor: return m_reactorScripts; break;
	}
	// No point, just return something in global scope
	return m_itemScripts;
}

auto ScriptDataProvider::resolvePath(ScriptTypes::ScriptTypes type) -> string_t {
	switch (type) {
		case ScriptTypes::Item: return "items"; break;
		case ScriptTypes::MapEntry: return "map_entry"; break;
		case ScriptTypes::FirstMapEntry: return "first_map_entry"; break;
		case ScriptTypes::Npc: return "npcs"; break;
		case ScriptTypes::Reactor: return "reactors"; break;
	}
	return "";
}