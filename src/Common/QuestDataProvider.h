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
#include "Types.h"
#include <unordered_map>

class QuestDataProvider {
	SINGLETON(QuestDataProvider);
public:
	auto loadData() -> void;

	auto isQuest(uint16_t questId) -> bool { return m_quests.find(questId) != std::end(m_quests); }
	auto getItemRequest(uint16_t questId, int32_t itemId) -> int16_t;
	auto getInfo(uint16_t questId) -> Quest * { return &m_quests[questId]; }
private:
	auto loadQuestData() -> void;
	auto loadRequests() -> void;
	auto loadRequiredJobs() -> void;
	auto loadRewards() -> void;

	hash_map_t<uint16_t, Quest> m_quests;
};