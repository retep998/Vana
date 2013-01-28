/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "Quest.h"

Quest::Quest() :
	m_nextQuest(0)
{
}

void Quest::addItemRequest(int32_t itemId, int16_t quantity) {
	m_itemRequests[itemId] = quantity;
}

void Quest::addMobRequest(int32_t mobId, int16_t quantity) {
	m_mobRequests[mobId] = quantity;
}

void Quest::addQuestRequest(int16_t questId, int8_t state) {
	m_questRequests[questId] = state;
}

void Quest::addValidJob(int16_t jobId) {
	m_jobRequests.push_back(jobId);
}

void Quest::addReward(bool start, const QuestRewardInfo &info, int16_t job) {
	QuestRewardsInfo *rewMap = nullptr;
	if (start) {
		rewMap = &m_startRewards;
	}
	else {
		rewMap = &m_endRewards;
	}
	if (job == -1) {
		rewMap->rewards.push_back(info);
	}
	else {
		rewMap->jobRewards[job].push_back(info);
	}
}

void Quest::mobRequestFunc(function<bool (int32_t, int16_t)> func) const {
	for (MobRequests::const_iterator iter = m_mobRequests.begin(); iter != m_mobRequests.end(); ++iter) {
		if (func(iter->first, iter->second)) {
			break;
		}
	}
}

void Quest::itemRequestFunc(function<bool (int32_t, int16_t)> func) const {
	for (ItemRequests::const_iterator iter = m_itemRequests.begin(); iter != m_itemRequests.end(); ++iter) {
		if (func(iter->first, iter->second)) {
			break;
		}
	}
}

void Quest::questRequestFunc(function<bool (int16_t, int8_t)> func) const {
	for (QuestRequests::const_iterator iter = m_questRequests.begin(); iter != m_questRequests.end(); ++iter) {
		if (func(iter->first, iter->second)) {
			break;
		}
	}
}

bool Quest::rewardsFunc(bool start, function<bool (const QuestRewardInfo &)> func) {
	return rewardsFunc(start, -1, func);
}

bool Quest::rewardsFunc(bool start, int16_t job, function<bool (const QuestRewardInfo &)> func) {
	bool broken = false;
	QuestRewardsInfo *rewMap = nullptr;
	if (start) {
		rewMap = &m_startRewards;
	}
	else {
		rewMap = &m_endRewards;
	}
	for (Rewards::const_iterator iter = rewMap->rewards.begin(); iter != rewMap->rewards.end(); ++iter) {
		if (func(*iter)) {
			broken = true;
			break;
		}
	}
	if (!broken && job != -1) {
		if (rewMap->jobRewards.find(job) != rewMap->jobRewards.end()) {
			Rewards &rew = rewMap->jobRewards[job];
			for (Rewards::const_iterator iter = rew.begin(); iter != rew.end(); ++iter) {
				if (func(*iter)) {
					broken = true;
					break;
				}
			}
		}
	}
	return !broken;
}