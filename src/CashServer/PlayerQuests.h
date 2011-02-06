/*
Copyright (C) 2008-2011 Vana Development Team

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

#include "Types.h"
#include "QuestDataProvider.h"
#include "Quests.h"
#include <boost/tr1/unordered_map.hpp>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

using std::map;
using std::string;
using std::tr1::unordered_map;

class PacketCreator;
class Player;

struct ActiveQuest {
	ActiveQuest() : done(false) { }

	string getQuestData() const {
		if (kills.size() == 0)
			return data;

		std::ostringstream info;
		for (map<int32_t, int16_t, std::less<int32_t> >::const_iterator iter = kills.begin(); iter != kills.end(); iter++) {
			info << std::setw(3) << std::setfill('0') << iter->second;
		}
		return info.str();
	}

	int16_t id;
	bool done;
	string data;
	map<int32_t, int16_t, std::less<int32_t> > kills;
};

class PlayerQuests {
public:
	PlayerQuests(Player *player);

	void load();
	void save();
	void connectData(PacketCreator &packet);

	void addQuest(int16_t questid, int32_t npcid);
	void updateQuestMob(int32_t mobid);
	void checkDone(ActiveQuest &quest);
	void finishQuest(int16_t questid, int32_t npcid);
	void removeQuest(int16_t questid);
	bool isQuestActive(int16_t questid);
	bool isQuestComplete(int16_t questid);
	void setQuestData(int16_t id, const string &data);
	string getQuestData(int16_t id);
private:
	Player *m_player;
	unordered_map<int32_t, vector<int16_t> > m_mobtoquest;
	map<int16_t, ActiveQuest> m_quests;
	map<int16_t, int64_t> m_completed;

	bool giveRewards(int16_t questid, bool start);
	bool giveRewards(const QuestRewardInfo &info);
	bool checkRewards(int16_t questid, Rewards::iterator &begin, Rewards::iterator &end);
	void addQuest(int16_t questid);
	void addQuestMobs(int16_t questid);
};
