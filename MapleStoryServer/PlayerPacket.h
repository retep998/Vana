#ifndef PLAYERPACK_H
#define PLAYERPACK_H

class Player;

namespace PlayerPacket {
	void connectData(Player* player);
	void newHP(Player* player, short hp);
	void showKeys(Player* player, int keys[90]);
	void newEXP(Player* player, int exp);
	void setSP(Player* player);
	void setJob(Player* player);
	void newMP(Player* player, short mp, bool is);
	void newHair(Player* player);
	void newEyes(Player* player);
	void newSkin(Player* player);
};

#endif