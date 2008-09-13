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
#ifndef QUESTS_H
#define QUESTS_H

#include "Types.h"
#include <vector>
#include <unordered_map>

using std::vector;
using std::tr1::unordered_map;

class Player;
class ReadPacket;

struct QuestRequestInfo {
	bool ismob;
	bool isitem;
	bool isquest;
	int32_t id;
	int16_t count;
};

typedef vector<QuestRequestInfo> QuestRequestsInfo;


struct QuestRewardInfo {
	bool start;
	bool ismesos;
	bool isitem;
	bool isexp;
	bool isfame;
	bool isskill;
	int32_t id;
	int16_t count;
	char gender;
	char job;
	char prop;
};

typedef vector<QuestRewardInfo> QuestRewardsInfo;

struct QuestInfo {
	QuestRequestsInfo requests;
	QuestRewardsInfo rewards;
	int16_t nextquest;
};

struct QuestComp {
	int32_t id;
	int64_t time;
};

struct QuestMob {
	int32_t id;
	int32_t count;
};

struct Quest {
	int16_t id;
	bool done;
	vector<QuestMob> mobs;
};

namespace Quests {
	extern unordered_map<int32_t, QuestInfo> quests;
	void addRequest(int32_t id, QuestRequestsInfo request);
	void addReward(int32_t id, QuestRewardsInfo raws);
	void setNextQuest(int16_t id, int16_t questid);
	void getQuest(Player *player, ReadPacket *packet);
	bool giveItem(Player *player, int32_t itemid, int16_t amount);
	bool giveMesos(Player *player, int32_t amount);
};

#endif
