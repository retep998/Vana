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

#include "noncopyable.hpp"
#include "Quest.h"
#include "QuestDataProvider.h"
#include "Quests.h"
#include "Types.h"
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>

using std::map;
using std::string;
using std::unordered_map;

class PacketCreator;
class Player;

struct ActiveQuest {
	ActiveQuest() : done(false) { }

	string getQuestData() const {
		if (kills.size() == 0)
			return data;

		std::ostringstream info;
		for (map<int32_t, int16_t, std::less<int32_t>>::const_iterator iter = kills.begin(); iter != kills.end(); ++iter) {
			info << std::setw(3) << std::setfill('0') << iter->second;
		}
		return info.str();
	}

	uint16_t id;
	bool done;
	string data;
	map<int32_t, int16_t, std::less<int32_t>> kills;
};

class PlayerQuests : boost::noncopyable {
public:
	PlayerQuests(Player *player);

	void load();
	void save();
	void connectData(PacketCreator &packet);

	void addQuest(uint16_t questId, int32_t npcId);
	void updateQuestMob(int32_t mobId);
	void checkDone(ActiveQuest &quest);
	void finishQuest(uint16_t questId, int32_t npcId);
	void removeQuest(uint16_t questId);
	bool isQuestActive(uint16_t questId);
	bool isQuestComplete(uint16_t questId);
	void setQuestData(uint16_t id, const string &data);
	string getQuestData(uint16_t id);
private:
	Player *m_player;
	unordered_map<int32_t, vector<int16_t>> m_mobToQuestMapping;
	map<uint16_t, ActiveQuest> m_quests;
	map<uint16_t, int64_t> m_completed;

	bool giveRewards(uint16_t questId, bool start);
	void addQuest(uint16_t questId);
	void addQuestMobs(uint16_t questId);
};