#ifndef LEVELPACK_H
#define LEVELPACK_H

#include <vector>
using namespace std;

class Player;

namespace LevelsPacket {
	void showEXP(Player* player, int exp, char type);
	void levelUP(Player* player, vector <Player*> players);
	void jobChange(Player* player, vector <Player*> players);
	void statOK(Player* player);
	void changeStat(Player* player, int nam, short val);
};

#endif