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

#include "QuestRequestInfo.hpp"
#include "QuestRewardInfo.hpp"
#include "QuestStateData.hpp"
#include "Types.hpp"
#include <functional>
#include <iterator>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace Vana {
	class Quest {
	public:
		auto getNextQuest() const -> quest_id_t;
		auto getQuestId() const -> quest_id_t;
		auto forEachRequest(bool start, function_t<IterationResult (const QuestRequestInfo &)> func) const -> CompletionResult;
		auto forEachReward(bool start, job_id_t job, function_t<IterationResult (const QuestRewardInfo &)> func) const -> CompletionResult;
	private:
		friend class QuestDataProvider;

		auto setNextQuest(quest_id_t questId) -> void;
		auto setQuestId(quest_id_t questId) -> void;
		auto addReward(bool start, const QuestRewardInfo &info, job_id_t job = -1) -> void;
		auto addRequest(bool start, const QuestRequestInfo &info) -> void;

		QuestStateData m_beginState;
		QuestStateData m_endState;
		quest_id_t m_nextQuest = 0;
		quest_id_t m_id = 0;
	};
}