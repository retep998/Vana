#ifndef QUESTSPACK_H
#define QUESTSPACK_H

#include <vector>

using namespace std;

class Player;

struct Quest;

class QuestsPacket {
public:
	static void acceptQuest(Player* player, short questid, int npcid);
	static void updateQuest(Player* player, Quest quest);
	static void doneQuest(Player* player, int questid);
	static void questFinish(Player* player, vector <Player*> players, short questid, int npcid, short nextquest, __int64 time);
	static void giveItem(Player* player, int itemid, int amount);
	static void giveMesos(Player* player, int amount);
};

#endif