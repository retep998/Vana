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
#include "Quest.hpp"

auto Quest::addItemRequest(int32_t itemId, int16_t quantity) -> void {
	m_itemRequests[itemId] = quantity;
}

auto Quest::addMobRequest(int32_t mobId, int16_t quantity) -> void {
	m_mobRequests[mobId] = quantity;
}

auto Quest::addQuestRequest(int16_t questId, int8_t state) -> void {
	m_questRequests[questId] = state;
}

auto Quest::addValidJob(int16_t jobId) -> void {
	m_jobRequests.push_back(jobId);
}

auto Quest::addReward(bool start, const QuestRewardInfo &info, int16_t job) -> void {
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

auto Quest::mobRequestFunc(function_t<bool (int32_t, int16_t)> func) const -> void {
	for (const auto &kvp : m_mobRequests) {
		if (func(kvp.first, kvp.second)) {
			break;
		}
	}
}

auto Quest::itemRequestFunc(function_t<bool (int32_t, int16_t)> func) const -> void {
	for (const auto &kvp : m_itemRequests) {
		if (func(kvp.first, kvp.second)) {
			break;
		}
	}
}

auto Quest::questRequestFunc(function_t<bool (int16_t, int8_t)> func) const -> void {
	for (const auto &kvp : m_questRequests) {
		if (func(kvp.first, kvp.second)) {
			break;
		}
	}
}

auto Quest::rewardsFunc(bool start, function_t<bool (const QuestRewardInfo &)> func) -> bool {
	return rewardsFunc(start, -1, func);
}

auto Quest::rewardsFunc(bool start, int16_t job, function_t<bool (const QuestRewardInfo &)> func) -> bool {
	bool broken = false;
	QuestRewardsInfo *rewMap = nullptr;
	if (start) {
		rewMap = &m_startRewards;
	}
	else {
		rewMap = &m_endRewards;
	}
	for (const auto &reward : rewMap->rewards) {
		if (func(reward)) {
			broken = true;
			break;
		}
	}
	if (!broken && job != -1) {
		auto kvp = rewMap->jobRewards.find(job);
		if (kvp != std::end(rewMap->jobRewards)) {
			const auto &rewards = kvp->second;
			for (const auto &reward : rewards) {
				if (func(reward)) {
					broken = true;
					break;
				}
			}
		}
	}
	return !broken;
}