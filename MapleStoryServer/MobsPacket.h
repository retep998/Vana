#ifndef MOBSPACK_H
#define MOBSPACK_H

class Player;
class Mob;

namespace MobsPacket {
	void controlMob(Player* player, Mob* mob);
	void endControlMob(Player* player, Mob* mob);
	void spawnMob(Player* player, Mob* mob, vector <Player*> players, bool isspawn);
	void showMob(Player* player, Mob* mob);
	void moveMob(Player* player, Mob* mob ,vector <Player*> players, unsigned char* pack, int pla);
	void damageMob(Player* player, vector <Player*> players, unsigned char* pack);
	void showHP(Player* player, int mobid, char per);
	void dieMob(Player* player, vector<Player*> players, Mob* mob, int mobid);
	void damageMobSkill(Player* player, vector <Player*> players, unsigned char* pack);
	void damageMobS(Player* player, vector <Player*> players, unsigned char* pack, int itemid);
};

#endif