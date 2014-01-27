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

#include "Quest.h"
#include "QuestDataProvider.h"
#include "Quests.h"
#include "Types.h"
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class PacketCreator;
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

	int16_t id = 0;
	bool done = false;
	string_t data;
	ord_map_t<int32_t, int16_t> kills;
};

class PlayerQuests {
	NONCOPYABLE(PlayerQuests);
	NO_DEFAULT_CONSTRUCTOR(PlayerQuests);
public:
	PlayerQuests(Player *player);

	auto load() -> void;
	auto save() -> void;
	auto connectData(PacketCreator &packet) -> void;

	auto addQuest(int16_t questId, int32_t npcId) -> void;
	auto updateQuestMob(int32_t mobId) -> void;
	auto checkDone(ActiveQuest &quest) -> void;
	auto finishQuest(int16_t questId, int32_t npcId) -> void;
	auto removeQuest(int16_t questId) -> void;
	auto isQuestActive(int16_t questId) -> bool;
	auto isQuestComplete(int16_t questId) -> bool;
	auto setQuestData(int16_t id, const string_t &data) -> void;
	auto getQuestData(int16_t id) -> string_t;
private:
	auto giveRewards(int16_t questId, bool start) -> bool;
	auto addQuest(int16_t questId) -> void;
	auto addQuestMobs(int16_t questId) -> void;

	Player *m_player = nullptr;
	hash_map_t<int32_t, vector_t<int16_t>> m_mobToQuestMapping;
	ord_map_t<int16_t, ActiveQuest> m_quests;
	ord_map_t<int16_t, int64_t> m_completed;
};