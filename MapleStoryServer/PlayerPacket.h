#ifndef PLAYERPACK_H
#define PLAYERPACK_H

class Player;

class PlayerPacket {
public:
	static void connectData(Player* player);
	static void newHP(Player* player, short hp);
	static void showKeys(Player* player, int keys[90]);
	static void newEXP(Player* player, int exp);
	static void setSP(Player* player);
	static void setJob(Player* player);
	static void newMP(Player* player, short mp, bool is);
	static void newHair(Player* player);
	static void newEyes(Player* player);
	static void newSkin(Player* player);
};

#endif