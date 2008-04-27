#ifndef LEVELS_H
#define LEVELS_H

class Player;

namespace Levels {
	extern int exps[200];
	void giveEXP(Player* player, int exp, char type=0);
	void addStat(Player* player, unsigned char* packet);
	void setLevel(Player* player, int level);
	void setJob(Player* player, int job);
};

#endif