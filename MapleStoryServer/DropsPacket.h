#ifndef DROPPACK_H
#define DROPPACK_H

#include <vector>

using namespace std;

class Player;
class Drop;
struct Dropped;

short getShort(unsigned char* buf);
int getInt(unsigned char* buf);
void getString(unsigned char* buf, int len, char* out);

class DropsPacket {
public:
	static void drop(vector <Player*> players, Drop* drop, Dropped dropper);
	static void dropForPlayer(Player* player, Drop* drop, Dropped dropper);
	static void showDrop(Player* player, Drop* drop);
	static void takeNote(Player* player, int id, bool ismesos, short amount);
	static void takeDrop(Player* player, vector <Player*> players, Drop* drop);
	static void dontTake(Player* player);
	static void removeDrop(vector <Player*> players, Drop* drop);
	static void explodeDrop(vector <Player*> players, Drop* drop);
};

#endif