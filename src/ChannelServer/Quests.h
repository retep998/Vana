/*
Copyright (C) 2008-2009 Vana Development Team

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
#include <boost/tr1/unordered_map.hpp>

using std::vector;
using std::tr1::unordered_map;

class Player;
class PacketReader;

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
	int8_t gender;
	int8_t job;
	int8_t prop;
};
typedef vector<QuestRewardInfo> QuestRewardsInfo;

struct QuestInfo {
	QuestRequestsInfo requests;
	QuestRewardsInfo rewards;
	int16_t nextquest;
};

struct QuestMob {
	QuestMob() : count(0) { }
	int32_t id;
	int16_t count;
	int16_t maxcount;
};

struct Quest {
	Quest() : done(false) { }
	int16_t id;
	vector<QuestMob> mobs;
	bool done;
};

struct QuestComp {
	QuestComp(int16_t id, int64_t time) : id(id), time(time) { }
	int16_t id;
	int64_t time;
};

namespace Quests {
	extern unordered_map<int32_t, QuestInfo> quests;
	void addRequest(int32_t id, QuestRequestsInfo request);
	void addReward(int32_t id, QuestRewardsInfo raws);
	void setNextQuest(int16_t id, int16_t questid);
	void getQuest(Player *player, PacketReader &packet);
	bool giveItem(Player *player, int32_t itemid, int16_t amount);
	bool giveMesos(Player *player, int32_t amount);
};

#endif
