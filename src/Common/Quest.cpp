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
#include "quest.hpp"

namespace vana {

auto quest::add_reward(bool start, const data::type::quest_reward_info &info, game_job_id job) -> void {
	auto &rewards = start ? m_begin_state.rewards : m_end_state.rewards;
	auto &reward_list = job == -1 ? rewards.universal : rewards.job[job];
	reward_list.push_back(info);
}

auto quest::add_request(bool start, const data::type::quest_request_info &info) -> void {
	auto &requests = start ? m_begin_state.requests : m_end_state.requests;
	requests.universal.push_back(info);
}

auto quest::for_each_request(bool start, function<iteration_result (const data::type::quest_request_info &)> func) const -> completion_result {
	bool broken = false;
	const auto &requests = start ? m_begin_state.requests : m_end_state.requests;
	for (const auto &request : requests.universal) {
		if (func(request) == iteration_result::stop_iterating) {
			broken = true;
			break;
		}
	}
	return broken ? completion_result::incomplete : completion_result::complete;
}

auto quest::for_each_reward(bool start, game_job_id job, function<iteration_result (const data::type::quest_reward_info &)> func) const -> completion_result {
	bool broken = false;
	const auto &rewards = start ? m_begin_state.rewards : m_end_state.rewards;
	for (const auto &reward : rewards.universal) {
		if (func(reward) == iteration_result::stop_iterating) {
			broken = true;
			break;
		}
	}
	if (!broken && job != -1) {
		auto kvp = rewards.job.find(job);
		if (kvp != std::end(rewards.job)) {
			for (const auto &reward : kvp->second) {
				if (func(reward) == iteration_result::stop_iterating) {
					broken = true;
					break;
				}
			}
		}
	}
	return broken ? completion_result::incomplete : completion_result::complete;
}

auto quest::get_next_quest() const -> game_quest_id {
	return m_next_quest;
}

auto quest::get_quest_id() const -> game_quest_id {
	return m_id;
}

auto quest::set_next_quest(game_quest_id quest_id) -> void {
	m_next_quest = quest_id;
}

auto quest::set_quest_id(game_quest_id quest_id) -> void {
	m_id = quest_id;
}

}