#ifndef QUESTS_H
#define QUESTS_H

#include <vector>
#include <hash_map>

using namespace std;
using namespace stdext;

class Player;

struct QuestRequestInfo {
	bool ismob;
	bool isitem;
	bool isquest;
	int id;
	short count;
};

typedef vector <QuestRequestInfo> QuestRequestsInfo;


struct QuestRewardInfo {
	bool start;
	bool ismesos;
	bool isitem;
	bool isexp;
	bool isfame;
	int id;
	short count;
	char gender;
	char job;
	char prop;
};

typedef vector <QuestRewardInfo> QuestRewardsInfo;

struct QuestInfo {
	QuestRequestsInfo requests;
	QuestRewardsInfo rewards;
	int nextquest;
};

struct QuestComp {
	int id;
	__int64 time;
};

struct QuestMob {
	int id;
	int count;
};

struct Quest {
	int id;
	bool done;
	vector <QuestMob> mobs;
};

class Quests{
public:
	static hash_map <int, QuestInfo> quests;
	static void addRequest(int id, QuestRequestsInfo request){		
		QuestInfo quest;
		if(quests.find(id) != quests.end())
			quest = quests[id];
		quest.requests = request;
		quests[id] = quest;
	}
	static void addReward(int id, QuestRewardsInfo raws){		
		QuestInfo quest;
		if(quests.find(id) != quests.end())
			quest = quests[id];
		quest.rewards = raws;
		quests[id] = quest;
	}
	static void setNextQuest(int id, int questid){		
		QuestInfo quest;
		if(quests.find(id) != quests.end())
			quest = quests[id];
		quest.nextquest = questid;
		quests[id] = quest;
	}
	static void getQuest(Player* player, unsigned char* packet);
	static void giveItem(Player* player, int itemid, int amount);
	static void giveMesos(Player* player, int amount);
};

class PlayerQuests {
public:
	void addQuest(int questid, int npcid);
	void updateQuestMob(int mobid);
	void checkDone(Quest &quest);
	void finishQuest(short questid, int npcid);
	void setPlayer(Player* player){
		this->player=player;
	}
	bool isQuestActive(short questid){
		for(unsigned int i=0; i<quests.size(); i++){
			if(quests[i].id == questid){
				return 1;
			}
		}
		return 0;
	}
private:
	Player* player;
	vector <Quest> quests;
	vector <QuestComp> questscomp;
};

#endif