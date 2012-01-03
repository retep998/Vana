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
#pragma once

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>

using std::string;
using std::tr1::unordered_map;

namespace ScriptTypes {
	enum ScriptTypes {
		Npc,
		Reactor,
		Quest,
		Item,
		MapEntry,
		FirstMapEntry
	};
}

class ScriptDataProvider : boost::noncopyable {
public:
	static ScriptDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new ScriptDataProvider();
		return singleton;
	}
	void loadData();

	string getQuestScript(int16_t questId, int8_t state);
	string getScript(int32_t objectId, ScriptTypes::ScriptTypes type);
	bool hasQuestScript(int16_t questId, int8_t state);
	bool hasScript(int32_t objectId, ScriptTypes::ScriptTypes type);
	int32_t getObjectIdFromScript(const string &script) { return m_scripts.find(script) == m_scripts.end() ? 0 : m_scripts[script]; }
private:
	ScriptDataProvider() {}
	static ScriptDataProvider *singleton;

	unordered_map<int32_t, string> & resolve(ScriptTypes::ScriptTypes type);
	string resolvePath(ScriptTypes::ScriptTypes type);

	unordered_map<int32_t, string> m_npcScripts;
	unordered_map<int32_t, string> m_reactorScripts;
	unordered_map<int32_t, string> m_mapEntryScripts;
	unordered_map<int32_t, string> m_firstMapEntryScripts;
	unordered_map<int32_t, string> m_itemScripts;
	unordered_map<int16_t, unordered_map<int8_t, string>> m_questScripts;
	unordered_map<string, int32_t> m_scripts;
};