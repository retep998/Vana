/*
Copyright (C) 2008-2012 Vana Development Team

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

#include "Quest.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>

using std::tr1::unordered_map;

class QuestDataProvider : boost::noncopyable {
public:
	static QuestDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new QuestDataProvider();
		return singleton;
	}
	void loadData();

	bool isQuest(int16_t questId) { return (m_quests.find(questId) != m_quests.end()); }
	int16_t getItemRequest(int16_t questId, int32_t itemId);
	Quest * getInfo(int16_t questId) { return &m_quests[questId]; }
private:
	QuestDataProvider() {}
	static QuestDataProvider *singleton;

	void loadQuestData();
	void loadRequests();
	void loadRequiredJobs();
	void loadRewards();

	unordered_map<int16_t, Quest> m_quests;
};