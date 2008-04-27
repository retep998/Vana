#ifndef MOBSPACK_H
#define MOBSPACK_H

class Player;
class Mob;

class MobsPacket {
public:
	static void controlMob(Player* player, Mob* mob);
	static void endControlMob(Player* player, Mob* mob);
	static void spawnMob(Player* player, Mob* mob, vector <Player*> players, bool isspawn);
	static void showMob(Player* player, Mob* mob);
	static void moveMob(Player* player, Mob* mob ,vector <Player*> players, unsigned char* pack, int pla);
	static void damageMob(Player* player, vector <Player*> players, unsigned char* pack);
	static void showHP(Player* player, int mobid, char per);
	static void dieMob(Player* player, vector<Player*> players, Mob* mob, int mobid);
	static void damageMobSkill(Player* player, vector <Player*> players, unsigned char* pack);
	static void damageMobS(Player* player, vector <Player*> players, unsigned char* pack, int itemid);
};

#endif