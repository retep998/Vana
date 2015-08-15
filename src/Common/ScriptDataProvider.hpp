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
#pragma once

#include "Types.hpp"
#include <string>
#include <unordered_map>

class AbstractServer;

enum class ScriptTypes {
	Npc,
	Reactor,
	Quest,
	Item,
	MapEntry,
	FirstMapEntry,
	Portal,
	Instance,
};

class ScriptDataProvider {
public:
	auto loadData() -> void;

	auto getQuestScript(AbstractServer *server, quest_id_t questId, int8_t state) const -> string_t;
	auto getScript(AbstractServer *server, int32_t objectId, ScriptTypes type) const -> string_t;
	auto hasQuestScript(quest_id_t questId, int8_t state) const -> bool;
	auto hasScript(int32_t objectId, ScriptTypes type) const -> bool;
	auto buildScriptPath(ScriptTypes type, const string_t &location) const -> string_t;
private:
	auto resolve(ScriptTypes type) const -> const hash_map_t<int32_t, string_t> &;
	auto resolvePath(ScriptTypes type) const -> string_t;

	hash_map_t<npc_id_t, string_t> m_npcScripts;
	hash_map_t<reactor_id_t, string_t> m_reactorScripts;
	hash_map_t<map_id_t, string_t> m_mapEntryScripts;
	hash_map_t<map_id_t, string_t> m_firstMapEntryScripts;
	hash_map_t<item_id_t, string_t> m_itemScripts;
	hash_map_t<quest_id_t, hash_map_t<int8_t, string_t>> m_questScripts;
};