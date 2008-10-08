/*
Copyright (C) 2008 Vana Development Team

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
#ifndef PLAYERQUESTS_H
#define PLAYERQUESTS_H

#include "Types.h"
#include <unordered_map>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

class Player;
struct Quest;
struct QuestComp;

class PlayerQuests {
public:
	PlayerQuests(Player *player) : player(player) { }
	void addQuest(int16_t questid, int32_t npcid);
	void updateQuestMob(int32_t mobid);
	void checkDone(Quest &quest);
	void finishQuest(int16_t questid, int32_t npcid);
	bool isQuestActive(int16_t questid);
private:
	Player *player;
	unordered_map<int32_t, Quest> quests;
	vector<QuestComp> completed;
};

#endif