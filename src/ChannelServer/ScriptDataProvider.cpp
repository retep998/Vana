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
#include "ScriptDataProvider.h"
#include "Database.h"
#include "FileUtilities.h"
#include "InitializeCommon.h"
#include "MiscUtilities.h"
#include <iostream>
#include <string>

using MiscUtilities::atob;
using Initializing::outputWidth;

ScriptDataProvider * ScriptDataProvider::singleton = 0;

void ScriptDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Scripts... ";

	npcscripts.clear();
	reactorscripts.clear();
	questscripts.clear();

	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM scriptdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW Row;
	enum ScriptColumns {
		Id = 0,
		IsNpc, IsReactor, IsQuest, State, ObjectId,
		Script
	};
	while (Row = res.fetch_raw_row()) {
		int32_t oid = atoi(Row[ObjectId]);
		string script = Row[Script];

		if (atob(Row[IsNpc])) {
			npcscripts[oid] = script;
		}
		else if (atob(Row[IsReactor])) {
			reactorscripts[oid] = script;
		}
		else if (atob(Row[IsQuest])) {
			int8_t state = atoi(Row[State]);
			questscripts[static_cast<int16_t>(oid)][state] = script;
		}
	}

	std::cout << "DONE" << std::endl;
}

string ScriptDataProvider::getNpcScript(int32_t npcid) {
	string s = "scripts/npcs/" + (npcscripts.find(npcid) != npcscripts.end() ? npcscripts[npcid] : "") + ".lua";
	if (s == "scripts/npcs/.lua" || !FileUtilities::fileExists(s)) { // File doesn't exist or NPC has no script replacement
		std::ostringstream filestream;
		filestream << "scripts/npcs/" << npcid << ".lua";
		s = filestream.str();
	}
	return s;
}

string ScriptDataProvider::getReactorScript(int32_t reactorid) {
	string s = "scripts/reactors/" + (reactorscripts.find(reactorid) != reactorscripts.end() ? reactorscripts[reactorid] : "") + ".lua";
	if (s == "scripts/reactors/.lua" || !FileUtilities::fileExists(s)) { // File doesn't exist or reactor has no script replacement
		std::ostringstream filestream;
		filestream << "scripts/reactors/" << reactorid << ".lua";
		s = filestream.str();
	}
	return s;
}

string ScriptDataProvider::getQuestScript(int16_t questid, int8_t state) {
	string s = "scripts/npcs/" + (questscripts.find(questid) != questscripts.end() && questscripts[questid].find(state) != questscripts[questid].end() ? questscripts[questid][state] : "") + ".lua";
	if (s == "scripts/npcs/.lua" || !FileUtilities::fileExists(s)) { // File doesn't exist or quest NPC has no script replacement
		std::ostringstream filestream;
		filestream << "scripts/npcs/" << questid << (state == 0 ? "s" : "e") << ".lua";
		s = filestream.str();
	}
	return s;	
}