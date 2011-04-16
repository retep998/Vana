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
#pragma once

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/functional.hpp>
#include <iterator>
#include <map>
#include <string>
#include <vector>

using std::map;
using std::tr1::unordered_map;
using std::tr1::function;
using std::vector;

struct QuestRewardInfo {
	QuestRewardInfo() :
		isMesos(false),
		isItem(false),
		isExp(false),
		isFame(false),
		isSkill(false),
		isBuff(false),
		masterLevelOnly(false)
		{ }

	bool isMesos;
	bool isItem;
	bool isExp;
	bool isFame;
	bool isSkill;
	bool isBuff;
	bool masterLevelOnly;
	int8_t gender;
	int16_t count;
	int16_t masterLevel;
	int32_t prop;
	int32_t id;
};

typedef vector<QuestRewardInfo> Rewards;

struct QuestRewardsInfo {
	Rewards rewards;
	unordered_map<int16_t, Rewards> jobrewards;
};

typedef vector<int16_t> JobRequests;
typedef map<int32_t, int16_t, std::less<int32_t>> MobRequests;
typedef unordered_map<int32_t, int16_t> ItemRequests;
typedef unordered_map<int16_t, int8_t> QuestRequests;

class Quest {
public:
	Quest();
	void addReward(bool start, const QuestRewardInfo &info, int16_t job = -1);
	void addValidJob(int16_t jobId);
	void addMobRequest(int32_t mobId, int16_t quantity);
	void addItemRequest(int32_t itemId, int16_t quantity);
	void addQuestRequest(int16_t questId, int8_t state);
	void setNextQuest(int16_t questId) { m_nextQuest = questId; }
	void setQuestId(int16_t questId) { m_id = questId; }

	bool hasRequests() const { return (hasMobRequests() || hasItemRequests() || hasQuestRequests()); }
	bool hasMobRequests() const { return (m_mobRequests.size() > 0); }
	bool hasItemRequests() const { return (m_itemRequests.size() > 0); }
	bool hasQuestRequests() const { return (m_questRequests.size() > 0); }
	bool hasStartRewards() { return (m_startRewards.rewards.size() > 0 || m_startRewards.jobrewards.size() > 0); }
	bool hasEndRewards() { return (m_endRewards.rewards.size() > 0 || m_endRewards.jobrewards.size() > 0); }
	bool hasRewards() { return (hasStartRewards() || hasEndRewards()); }
	int16_t getNextQuest() const { return m_nextQuest; }
	int16_t getQuestId() const { return m_id; }
	int16_t getMobRequestQuantity(int32_t mobId) { return (m_mobRequests.find(mobId) != m_mobRequests.end() ? m_mobRequests[mobId] : 0); }
	int16_t getItemRequestQuantity(int32_t itemId) { return (m_itemRequests.find(itemId) != m_itemRequests.end() ? m_itemRequests[itemId] : 0); }
	void mobRequestFunc(function<bool (int32_t, int16_t)> func) const;
	void itemRequestFunc(function<bool (int32_t, int16_t)> func) const;
	void questRequestFunc(function<bool (int16_t, int8_t)> func) const;
	bool rewardsFunc(function<bool (const QuestRewardInfo &)> func, bool start, int16_t job = -1);
private:
	MobRequests m_mobRequests;
	ItemRequests m_itemRequests;
	JobRequests m_jobRequests;
	QuestRequests m_questRequests;
	QuestRewardsInfo m_startRewards;
	QuestRewardsInfo m_endRewards;
	int16_t m_nextQuest;
	int16_t m_id;
};