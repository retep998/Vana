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

#include "common/quest.hpp"
#include "common/types.hpp"
#include <unordered_map>

namespace vana {
	class quest_data_provider {
	public:
		auto load_data() -> void;

		auto is_quest(game_quest_id quest_id) const -> bool;
		auto get_info(game_quest_id quest_id) const -> const quest &;
	private:
		auto load_quest_data() -> void;
		auto load_requests() -> void;
		auto load_required_jobs() -> void;
		auto load_rewards() -> void;

		hash_map<game_quest_id, quest> m_quests;
	};
}