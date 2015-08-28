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

#include "FileTime.hpp"
#include "Quest.hpp"
#include "QuestDataProvider.hpp"
#include "Quests.hpp"
#include "Types.hpp"
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Vana {
	class PacketBuilder;
	class Player;

	struct ActiveQuest {
		auto getQuestData() const -> string_t {
			if (kills.size() == 0) {
				return data;
			}

			out_stream_t info;
			for (const auto &kvp : kills) {
				info << std::setw(3) << std::setfill('0') << kvp.second;
			}
			return info.str();
		}

		quest_id_t id = 0;
		bool done = false;
		string_t data;
		ord_map_t<mob_id_t, uint16_t> kills;
	};

	// TODO FIXME accuracy
	// Potentially refactor quest drop display to the MAP instead of the drops, because that's how global does it
	enum class AllowQuestItemResult {
		Allow,
		Disallow,
	};

	class PlayerQuests {
		NONCOPYABLE(PlayerQuests);
		NO_DEFAULT_CONSTRUCTOR(PlayerQuests);
	public:
		PlayerQuests(Player *player);

		auto load() -> void;
		auto save() -> void;
		auto connectData(PacketBuilder &packet) -> void;

		auto itemDropAllowed(item_id_t itemId, quest_id_t questId) -> AllowQuestItemResult;
		auto addQuest(quest_id_t questId, npc_id_t npcId) -> void;
		auto updateQuestMob(mob_id_t mobId) -> void;
		auto checkDone(ActiveQuest &quest) -> void;
		auto finishQuest(quest_id_t questId, npc_id_t npcId) -> void;
		auto removeQuest(quest_id_t questId) -> void;
		auto isQuestActive(quest_id_t questId) -> bool;
		auto isQuestComplete(quest_id_t questId) -> bool;
		auto setQuestData(quest_id_t id, const string_t &data) -> void;
		auto getQuestData(quest_id_t id) -> string_t;
	private:
		auto giveRewards(quest_id_t questId, bool start) -> Result;

		Player *m_player = nullptr;
		hash_map_t<mob_id_t, vector_t<quest_id_t>> m_mobToQuestMapping;
		ord_map_t<quest_id_t, ActiveQuest> m_quests;
		ord_map_t<quest_id_t, FileTime> m_completed;
	};
}