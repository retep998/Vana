/*
Copyright (C) 2008-2016 Vana Development Team

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

namespace Vana {

auto Quest::addReward(bool start, const QuestRewardInfo &info, job_id_t job) -> void {
	auto &rewards = start ? m_beginState.rewards : m_endState.rewards;
	auto &rewardList = job == -1 ? rewards.universal : rewards.job[job];
	rewardList.push_back(info);
}

auto Quest::addRequest(bool start, const QuestRequestInfo &info) -> void {
	auto &requests = start ? m_beginState.requests : m_endState.requests;
	requests.universal.push_back(info);
}

auto Quest::forEachRequest(bool start, function_t<IterationResult (const QuestRequestInfo &)> func) const -> CompletionResult {
	bool broken = false;
	const auto &requests = start ? m_beginState.requests : m_endState.requests;
	for (const auto &request : requests.universal) {
		if (func(request) == IterationResult::StopIterating) {
			broken = true;
			break;
		}
	}
	return broken ? CompletionResult::Incomplete : CompletionResult::Complete;
}

auto Quest::forEachReward(bool start, job_id_t job, function_t<IterationResult (const QuestRewardInfo &)> func) const -> CompletionResult {
	bool broken = false;
	const auto &rewards = start ? m_beginState.rewards : m_endState.rewards;
	for (const auto &reward : rewards.universal) {
		if (func(reward) == IterationResult::StopIterating) {
			broken = true;
			break;
		}
	}
	if (!broken && job != -1) {
		auto kvp = rewards.job.find(job);
		if (kvp != std::end(rewards.job)) {
			for (const auto &reward : kvp->second) {
				if (func(reward) == IterationResult::StopIterating) {
					broken = true;
					break;
				}
			}
		}
	}
	return broken ? CompletionResult::Incomplete : CompletionResult::Complete;
}

auto Quest::getNextQuest() const -> quest_id_t {
	return m_nextQuest;
}

auto Quest::getQuestId() const -> quest_id_t {
	return m_id;
}

auto Quest::setNextQuest(quest_id_t questId) -> void {
	m_nextQuest = questId;
}

auto Quest::setQuestId(quest_id_t questId) -> void {
	m_id = questId;
}

}