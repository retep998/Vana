/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "AbstractServer.hpp"
#include "Algorithm.hpp"
#include "Database.hpp"
#include "FileUtilities.hpp"
#include "InitializeCommon.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

auto ScriptDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Scripts... ";

	m_npcScripts.clear();
	m_reactorScripts.clear();
	m_questScripts.clear();
	m_mapEntryScripts.clear();
	m_firstMapEntryScripts.clear();
	m_itemScripts.clear();

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("scripts"));

	for (const auto &row : rs) {
		int32_t objectId = row.get<int32_t>("objectid");
		string_t script = row.get<string_t>("script");
		int8_t modifier = row.get<int8_t>("helper");

		StringUtilities::runEnum(row.get<string_t>("script_type"), [&](const string_t &cmp) {
			if (cmp == "npc") m_npcScripts[objectId] = script;
			else if (cmp == "reactor") m_reactorScripts[objectId] = script;
			else if (cmp == "map_enter") m_mapEntryScripts[objectId] = script;
			else if (cmp == "map_first_enter") m_firstMapEntryScripts[objectId] = script;
			else if (cmp == "item") m_itemScripts[objectId] = script;
			else if (cmp == "quest") m_questScripts[static_cast<quest_id_t>(objectId)][modifier] = script;
		});
	}

	std::cout << "DONE" << std::endl;
}

auto ScriptDataProvider::getScript(AbstractServer *server, int32_t objectId, ScriptTypes type) const -> string_t {
	if (hasScript(objectId, type)) {
		string_t s = buildScriptPath(type, resolve(type).find(objectId)->second);
		if (FileUtilities::fileExists(s)) {
			return s;
		}
#ifdef DEBUG
		else server->log(LogType::DebugError, "Missing script '" + s + "'");
#endif
	}
	return buildScriptPath(type, std::to_string(objectId));
}

auto ScriptDataProvider::getQuestScript(AbstractServer *server, quest_id_t questId, int8_t state) const -> string_t {
	if (hasQuestScript(questId, state)) {
		string_t s = buildScriptPath(ScriptTypes::Quest, m_questScripts.find(questId)->second.find(state)->second);
		if (FileUtilities::fileExists(s)) {
			return s;
		}
#ifdef DEBUG
		else server->log(LogType::DebugError, "Missing quest script '" + s + "'");
#endif
	}
	return buildScriptPath(ScriptTypes::Quest, std::to_string(questId) + (state == 0 ? "s" : "e"));
}

auto ScriptDataProvider::buildScriptPath(ScriptTypes type, const string_t &location) const -> string_t {
	string_t s = "scripts/" + resolvePath(type) + "/" + location + ".lua";
	return s;
}

auto ScriptDataProvider::hasScript(int32_t objectId, ScriptTypes type) const -> bool {
	return ext::is_element(resolve(type), objectId);
}

auto ScriptDataProvider::hasQuestScript(quest_id_t questId, int8_t state) const -> bool {
	return ext::is_element(m_questScripts, questId);
}

auto ScriptDataProvider::resolve(ScriptTypes type) const -> const hash_map_t<int32_t, string_t> & {
	switch (type) {
		case ScriptTypes::Item: return m_itemScripts;
		case ScriptTypes::MapEntry: return m_mapEntryScripts;
		case ScriptTypes::FirstMapEntry: return m_firstMapEntryScripts;
		case ScriptTypes::Npc: return m_npcScripts;
		case ScriptTypes::Reactor: return m_reactorScripts;
	}
	throw NotImplementedException{"ScriptType"};
}

auto ScriptDataProvider::resolvePath(ScriptTypes type) const -> string_t {
	switch (type) {
		case ScriptTypes::Item: return "items";
		case ScriptTypes::MapEntry: return "map_entry";
		case ScriptTypes::FirstMapEntry: return "first_map_entry";
		case ScriptTypes::Npc: return "npcs";
		case ScriptTypes::Reactor: return "reactors";
		case ScriptTypes::Quest: return "quests";
		case ScriptTypes::Instance: return "instances";
		case ScriptTypes::Portal: return "portals";
	}
	throw NotImplementedException{"ScriptType"};
}