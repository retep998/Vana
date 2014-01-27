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
#pragma once

#include "Types.hpp"
#include <string>
#include <unordered_map>

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

class ScriptDataProvider {
	SINGLETON(ScriptDataProvider);
public:
	auto loadData() -> void;

	auto getQuestScript(int16_t questId, int8_t state) -> string_t;
	auto getScript(int32_t objectId, ScriptTypes::ScriptTypes type) -> string_t;
	auto hasQuestScript(int16_t questId, int8_t state) -> bool;
	auto hasScript(int32_t objectId, ScriptTypes::ScriptTypes type) -> bool;
	auto getObjectIdFromScript(const string_t &script) -> int32_t { return m_scripts.find(script) == std::end(m_scripts) ? 0 : m_scripts[script]; }
private:
	auto resolve(ScriptTypes::ScriptTypes type) -> hash_map_t<int32_t, string_t> &;
	auto resolvePath(ScriptTypes::ScriptTypes type) -> string_t;

	hash_map_t<int32_t, string_t> m_npcScripts;
	hash_map_t<int32_t, string_t> m_reactorScripts;
	hash_map_t<int32_t, string_t> m_mapEntryScripts;
	hash_map_t<int32_t, string_t> m_firstMapEntryScripts;
	hash_map_t<int32_t, string_t> m_itemScripts;
	hash_map_t<int16_t, hash_map_t<int8_t, string_t>> m_questScripts;
	hash_map_t<string_t, int32_t> m_scripts;
};