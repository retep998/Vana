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

#include <vector>

using std::vector;

class Player;
struct Quest;
struct QuestComp;

class PlayerQuests {
public:
	PlayerQuests(Player *player) : player(player) { }
	void addQuest(int questid, int npcid);
	void updateQuestMob(int mobid);
	void checkDone(Quest &quest);
	void finishQuest(short questid, int npcid);
	bool isQuestActive(short questid);
private:
	Player *player;
	vector<Quest> quests;
	vector<QuestComp> questscomp;
};

#endif