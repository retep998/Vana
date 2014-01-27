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

#include "Types.h"
#include <functional>
#include <iterator>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

struct QuestRewardInfo {
	bool isMesos = false;
	bool isItem = false;
	bool isExp = false;
	bool isFame = false;
	bool isSkill = false;
	bool isBuff = false;
	bool masterLevelOnly = false;
	int8_t gender = 0;
	int16_t count = 0;
	int16_t masterLevel = 0;
	int32_t prop = 0;
	int32_t id = 0;
};

struct QuestRewardsInfo {
	vector_t<QuestRewardInfo> rewards;
	hash_map_t<int16_t, vector_t<QuestRewardInfo>> jobRewards;
};

class Quest {
public:
	auto addReward(bool start, const QuestRewardInfo &info, int16_t job = -1) -> void;
	auto addValidJob(int16_t jobId) -> void;
	auto addMobRequest(int32_t mobId, int16_t quantity) -> void;
	auto addItemRequest(int32_t itemId, int16_t quantity) -> void;
	auto addQuestRequest(int16_t questId, int8_t state) -> void;
	auto setNextQuest(int16_t questId) -> void { m_nextQuest = questId; }
	auto setQuestId(int16_t questId) -> void { m_id = questId; }

	auto hasRequests() const -> bool { return hasMobRequests() || hasItemRequests() || hasQuestRequests(); }
	auto hasMobRequests() const -> bool { return m_mobRequests.size() > 0; }
	auto hasItemRequests() const -> bool { return m_itemRequests.size() > 0; }
	auto hasQuestRequests() const -> bool { return m_questRequests.size() > 0; }
	auto hasStartRewards() const -> bool { return m_startRewards.rewards.size() > 0 || m_startRewards.jobRewards.size() > 0; }
	auto hasEndRewards() const -> bool { return m_endRewards.rewards.size() > 0 || m_endRewards.jobRewards.size() > 0; }
	auto hasRewards() const -> bool { return hasStartRewards() || hasEndRewards(); }
	auto getNextQuest() const -> int16_t { return m_nextQuest; }
	auto getQuestId() const -> int16_t { return m_id; }
	auto getMobRequestQuantity(int32_t mobId) -> int16_t { return m_mobRequests.find(mobId) != std::end(m_mobRequests) ? m_mobRequests[mobId] : 0; }
	auto getItemRequestQuantity(int32_t itemId) -> int16_t { return m_itemRequests.find(itemId) != std::end(m_itemRequests) ? m_itemRequests[itemId] : 0; }
	auto mobRequestFunc(function_t<bool (int32_t, int16_t)> func) const -> void;
	auto itemRequestFunc(function_t<bool (int32_t, int16_t)> func) const -> void;
	auto questRequestFunc(function_t<bool (int16_t, int8_t)> func) const -> void;
	auto rewardsFunc(bool start, function_t<bool (const QuestRewardInfo &)> func) -> bool;
	auto rewardsFunc(bool start, int16_t job, function_t<bool (const QuestRewardInfo &)> func) -> bool;
private:
	QuestRewardsInfo m_startRewards;
	QuestRewardsInfo m_endRewards;
	ord_map_t<int32_t, int16_t> m_mobRequests;
	hash_map_t<int32_t, int16_t> m_itemRequests;
	vector_t<int16_t> m_jobRequests;
	hash_map_t<int16_t, int8_t> m_questRequests;
	int16_t m_nextQuest = 0;
	int16_t m_id = 0;
};