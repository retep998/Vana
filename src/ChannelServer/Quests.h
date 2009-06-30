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
#include <boost/tr1/unordered_map.hpp>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::tr1::unordered_map;
using std::vector;

class Player;
class PacketReader;

namespace QuestRequestTypes {
	const int32_t Count = 3;
	const int32_t Mob = 0x01;
	const int32_t Item = 0x02;
	const int32_t Quest = 0x04;
};

typedef unordered_map<int32_t, int16_t> QuestRequest;
typedef unordered_map<int32_t, QuestRequest> QuestRequestInfo;

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
	bool hasRequests() { return (hasRequests(QuestRequestTypes::Mob) || hasRequests(QuestRequestTypes::Item) || hasRequests(QuestRequestTypes::Quest)); }
	bool hasRequests(int32_t type) { return (requests.find(type) != requests.end()); }
	QuestRequest getRequest(int32_t type) { return requests[type]; }

	QuestRequestInfo requests;
	QuestRewardsInfo rewards;
	int16_t nextquest;
};

struct QuestMob {
	QuestMob() : count(0) { }
	int32_t id;
	int16_t count;
	int16_t maxcount;
};

struct ActiveQuest {
	ActiveQuest() : done(false) { }

	string getQuestData() const {
		size_t s = mobs.size();
		if (s == 0)
			return data;

		std::ostringstream info;
		for (size_t i = 0; i < s; i++) {
			info << std::setw(3) << std::setfill('0') << mobs[(s - i - 1)].count;
		}
		return info.str();
	}

	int16_t id;
	bool done;
	string data;
	vector<QuestMob> mobs;
};

namespace Quests {
	extern unordered_map<int32_t, QuestInfo> quests;
	void addRequest(int32_t id, int32_t type, unordered_map<int32_t, int16_t> &request);
	void addReward(int32_t id, QuestRewardsInfo &raws);
	void setNextQuest(int16_t id, int16_t questid);
	void getQuest(Player *player, PacketReader &packet);
	bool giveItem(Player *player, int32_t itemid, int16_t amount);
	bool giveMesos(Player *player, int32_t amount);
};

#endif
