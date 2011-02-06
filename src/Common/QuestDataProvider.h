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

#include "Pos.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::tr1::unordered_map;
using std::vector;

struct QuestRewardInfo {
	QuestRewardInfo() :
		ismesos(false),
		isitem(false),
		isexp(false),
		isfame(false),
		isskill(false),
		isbuff(false),
		masterlevelonly(false)
		{ }

	bool ismesos;
	bool isitem;
	bool isexp;
	bool isfame;
	bool isskill;
	bool isbuff;
	bool masterlevelonly;
	int8_t gender;
	int16_t count;
	int16_t masterlevel;
	int32_t prop;
	int32_t id;
};

typedef vector<QuestRewardInfo> Rewards;

struct QuestRewardsInfo {
	Rewards rewards;
	unordered_map<int16_t, Rewards> jobrewards;
};

typedef vector<int16_t> JobRequests;
typedef map<int32_t, int16_t, std::less<int32_t> > MobRequests;
typedef unordered_map<int32_t, int16_t> ItemRequests;
typedef unordered_map<int16_t, int8_t> QuestRequests;

class Quest {
public:
	Quest() : nextquest(0) { }
	void addReward(bool start, const QuestRewardInfo &info, int16_t job = -1);
	void addValidJob(int16_t jobid);
	void addMobRequest(int32_t mobid, int16_t quantity);
	void addItemRequest(int32_t itemid, int16_t quantity);
	void addQuestRequest(int16_t questid, int8_t state);
	void setNextQuest(int16_t questid) { nextquest = questid; }
	void setQuestId(int16_t q) { id = q; }

	bool hasRequests() { return (hasMobRequests() || hasItemRequests() || hasQuestRequests()); }
	bool hasMobRequests() { return (mobrequests.size() > 0); }
	bool hasItemRequests() { return (itemrequests.size() > 0); }
	bool hasQuestRequests() { return (questrequests.size() > 0); }
	bool hasStartRewards() { return (startrewards.rewards.size() > 0 || startrewards.jobrewards.size() > 0); }
	bool hasEndRewards() { return (endrewards.rewards.size() > 0 || endrewards.jobrewards.size() > 0); }
	bool hasRewards() { return (hasStartRewards() || hasEndRewards()); }
	bool hasStartJobRewards(int16_t job) { return (startrewards.jobrewards.find(job) != startrewards.jobrewards.end()); }
	bool hasEndJobRewards(int16_t job) { return (endrewards.jobrewards.find(job) != endrewards.jobrewards.end()); }
	int16_t getNextQuest() const { return nextquest; }
	int16_t getQuestId() const { return id; }
	int16_t getMobRequestQuantity(int32_t mobid) { return (mobrequests.find(mobid) != mobrequests.end() ? mobrequests[mobid] : 0); }
	int16_t getItemRequestQuantity(int32_t itemid) { return (itemrequests.find(itemid) != itemrequests.end() ? itemrequests[itemid] : 0); }
	MobRequests::iterator getMobBegin() { return mobrequests.begin(); }
	MobRequests::iterator getMobEnd() { return mobrequests.end(); }
	ItemRequests::iterator getItemBegin() { return itemrequests.begin(); }
	ItemRequests::iterator getItemEnd() { return itemrequests.end(); }
	QuestRequests::iterator getQuestBegin() { return questrequests.begin(); }
	QuestRequests::iterator getQuestEnd() { return questrequests.end(); }
	Rewards::iterator getStartRewardsBegin() { return startrewards.rewards.begin(); }
	Rewards::iterator getStartRewardsEnd() { return startrewards.rewards.end(); }
	Rewards::iterator getStartJobRewardsBegin(int16_t job) { return startrewards.jobrewards[job].begin(); }
	Rewards::iterator getStartJobRewardsEnd(int16_t job) { return startrewards.jobrewards[job].end(); }
	Rewards::iterator getEndRewardsBegin() { return endrewards.rewards.begin(); }
	Rewards::iterator getEndRewardsEnd() { return endrewards.rewards.end(); }
	Rewards::iterator getEndJobRewardsBegin(int16_t job) { return endrewards.jobrewards[job].begin(); }
	Rewards::iterator getEndJobRewardsEnd(int16_t job) { return endrewards.jobrewards[job].end(); }
private:
	MobRequests mobrequests;
	ItemRequests itemrequests;
	JobRequests jobrequests;
	QuestRequests questrequests;
	QuestRewardsInfo startrewards;
	QuestRewardsInfo endrewards;
	int16_t nextquest;
	int16_t id;
};

class QuestDataProvider : boost::noncopyable {
public:
	static QuestDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new QuestDataProvider();
		return singleton;
	}
	void loadData();

	bool isQuest(int16_t questid) { return (quests.find(questid) != quests.end()); }
	int16_t getItemRequest(int16_t questid, int32_t itemid);
	Quest * getInfo(int16_t questid) { return &quests[questid]; }
private:
	QuestDataProvider() {}
	static QuestDataProvider *singleton;

	void loadQuestData();
	void loadRequests();
	void loadRequiredJobs();
	void loadRewards();

	unordered_map<int16_t, Quest> quests;
};
