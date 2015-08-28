/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Quest.hpp"
#include "Types.hpp"
#include <unordered_map>

namespace Vana {
	class QuestDataProvider {
	public:
		auto loadData() -> void;

		auto isQuest(quest_id_t questId) const -> bool;
		auto getInfo(quest_id_t questId) const -> const Quest &;
	private:
		auto loadQuestData() -> void;
		auto loadRequests() -> void;
		auto loadRequiredJobs() -> void;
		auto loadRewards() -> void;

		hash_map_t<quest_id_t, Quest> m_quests;
	};
}