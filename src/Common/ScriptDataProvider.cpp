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
#include "ScriptDataProvider.h"
#include "Database.h"
#include "FileUtilities.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <iostream>
#include <string>

using FileUtilities::fileExists;
using Initializing::outputWidth;

ScriptDataProvider * ScriptDataProvider::singleton = nullptr;

void ScriptDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Scripts... ";

	npcscripts.clear();
	reactorscripts.clear();
	questscripts.clear();
	mapentryscripts.clear();
	firstmapentryscripts.clear();
	itemscripts.clear();

	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM scripts");
	mysqlpp::UseQueryResult res = query.use();
	int8_t modifier;
	int32_t objectid;
	string script;
	string scripttype;

	enum ScriptData {
		ScriptType = 0,
		Modifier, ObjectId, Script
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		objectid = atoi(row[ObjectId]);
		scripttype = row[ScriptType];
		script = row[Script];
		modifier = atoi(row[Modifier]);

		if (scripttype == "npc") npcscripts[objectid] = script;
		else if (scripttype == "reactor") reactorscripts[objectid] = script;
		else if (scripttype == "map_enter") mapentryscripts[objectid] = script;
		else if (scripttype == "map_first_enter") firstmapentryscripts[objectid] = script;
		else if (scripttype == "item") itemscripts[objectid] = script;
		else if (scripttype == "quest") {
			questscripts[static_cast<int16_t>(objectid)][modifier] = script;
		}

		scripts[script] = objectid;
	}

	std::cout << "DONE" << std::endl;
}

string ScriptDataProvider::getNpcScript(int32_t npcid) {
	if (hasNpcScript(npcid)) {
		string s = "scripts/npcs/" + npcscripts[npcid] + ".lua";
		if (fileExists(s)) {
			return s;
		}
	}
	std::ostringstream filestream;
	filestream << "scripts/npcs/" << npcid << ".lua";
	string g(filestream.str());
	return g;
}

string ScriptDataProvider::getReactorScript(int32_t reactorid) {
	if (hasReactorScript(reactorid)) {
		string s = "scripts/reactors/" + reactorscripts[reactorid] + ".lua";
		if (fileExists(s)) {
			return s;
		}
	}
	std::ostringstream filestream;
	filestream << "scripts/reactors/" << reactorid << ".lua";
	string g(filestream.str());
	return g;
}

string ScriptDataProvider::getQuestScript(int16_t questid, int8_t state) {
	if (hasQuestScript(questid, state)) {
		string s = "scripts/quests/" + questscripts[questid][state] + ".lua";
		if (fileExists(s)) {
			return s;
		}
	}
	std::ostringstream filestream;
	filestream << "scripts/quests/" << questid << (state == 0 ? "s" : "e") << ".lua";
	string g(filestream.str());
	return g;
}

string ScriptDataProvider::getItemScript(int32_t itemid) {
	if (hasItemScript(itemid)) {
		string s = "scripts/items/" + itemscripts[itemid] + ".lua";
		if (fileExists(s)) {
			return s;
		}
	}
	std::ostringstream filestream;
	filestream << "scripts/items/" << itemid << ".lua";
	string g(filestream.str());
	return g;
}

string ScriptDataProvider::getMapEntryScript(int32_t mapid) {
	if (hasMapEntryScript(mapid)) {
		string s = "scripts/map_entry/" + mapentryscripts[mapid] + ".lua";
		if (fileExists(s)) {
			return s;
		}
	}
	std::ostringstream filestream;
	filestream << "scripts/map_entry/" << mapid << ".lua";
	string g(filestream.str());
	return g;
}

string ScriptDataProvider::getFirstMapEntryScript(int32_t mapid) {
	if (hasMapEntryScript(mapid)) {
		string s = "scripts/first_map_entry/" + firstmapentryscripts[mapid] + ".lua";
		if (fileExists(s)) {
			return s;
		}
	}
	std::ostringstream filestream;
	filestream << "scripts/first_map_entry/" << mapid << ".lua";
	string g(filestream.str());
	return g;
}

bool ScriptDataProvider::hasNpcScript(int32_t npcid) {
	return (npcscripts.find(npcid) != npcscripts.end());
}

bool ScriptDataProvider::hasReactorScript(int32_t reactorid) {
	return (reactorscripts.find(reactorid) != reactorscripts.end());
}

bool ScriptDataProvider::hasQuestScript(int16_t questid, int8_t state) {
	return (questscripts.find(questid) != questscripts.end());
}

bool ScriptDataProvider::hasItemScript(int32_t itemid) {
	return (itemscripts.find(itemid) != itemscripts.end());
}

bool ScriptDataProvider::hasMapEntryScript(int32_t mapid) {
	return (mapentryscripts.find(mapid) != mapentryscripts.end());
}

bool ScriptDataProvider::hasFirstMapEntryScript(int32_t mapid) {
	return (firstmapentryscripts.find(mapid) != firstmapentryscripts.end());
}