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
#pragma once

#include "common/quest_request_info.hpp"
#include "common/quest_reward_info.hpp"
#include "common/quest_state_data.hpp"
#include "common/types.hpp"
#include <functional>
#include <iterator>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	class quest {
	public:
		auto get_next_quest() const -> game_quest_id;
		auto get_quest_id() const -> game_quest_id;
		auto for_each_request(bool start, function<iteration_result (const quest_request_info &)> func) const -> completion_result;
		auto for_each_reward(bool start, game_job_id job, function<iteration_result (const quest_reward_info &)> func) const -> completion_result;
	private:
		friend class quest_data_provider;

		auto set_next_quest(game_quest_id quest_id) -> void;
		auto set_quest_id(game_quest_id quest_id) -> void;
		auto add_reward(bool start, const quest_reward_info &info, game_job_id job = -1) -> void;
		auto add_request(bool start, const quest_request_info &info) -> void;

		quest_state_data m_begin_state;
		quest_state_data m_end_state;
		game_quest_id m_next_quest = 0;
		game_quest_id m_id = 0;
	};
}