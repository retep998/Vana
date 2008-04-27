#ifndef DROPPACK_H
#define DROPPACK_H

#include <vector>

using namespace std;

class Player;
class Drop;
struct Dropped;

namespace DropsPacket {
	void drop(vector <Player*> players, Drop* drop, Dropped dropper);
	void dropForPlayer(Player* player, Drop* drop, Dropped dropper);
	void showDrop(Player* player, Drop* drop);
	void takeNote(Player* player, int id, bool ismesos, short amount);
	void takeDrop(Player* player, vector <Player*> players, Drop* drop);
	void dontTake(Player* player);
	void removeDrop(vector <Player*> players, Drop* drop);
	void explodeDrop(vector <Player*> players, Drop* drop);
};

#endif