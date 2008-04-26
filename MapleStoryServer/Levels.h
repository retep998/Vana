#ifndef LEVELS_H
#define LEVELS_H

class Player;

class Levels {
public:
	static int exps[200];
	static void giveEXP(Player* player, int exp, char type=0);
	static void addStat(Player* player, unsigned char* packet);
	static void setLevel(Player* player, int level);
	static void setJob(Player* player, int job);
};

#endif