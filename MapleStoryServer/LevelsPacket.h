#ifndef LEVELPACK_H
#define LEVELPACK_H

#include <vector>
using namespace std;

short getShort(unsigned char* buf);
int getInt(unsigned char* buf);
void getString(unsigned char* buf, int len, char* out);

class Player;

class LevelsPacket {
public:
	static void showEXP(Player* player, int exp, char type);
	static void levelUP(Player* player, vector <Player*> players);
	static void jobChange(Player* player, vector <Player*> players);
	static void statOK(Player* player);
	static void changeStat(Player* player, int nam, short val);
};

#endif