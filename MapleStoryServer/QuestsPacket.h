#ifndef QUESTSPACK_H
#define QUESTSPACK_H

#include <vector>

using namespace std;

class Player;

struct Quest;

namespace QuestsPacket {
	void acceptQuest(Player* player, short questid, int npcid);
	void updateQuest(Player* player, Quest quest);
	void doneQuest(Player* player, int questid);
	void questFinish(Player* player, vector <Player*> players, short questid, int npcid, short nextquest, __int64 time);
	void giveItem(Player* player, int itemid, int amount);
	void giveMesos(Player* player, int amount);
};

#endif