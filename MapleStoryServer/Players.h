#ifndef PLAYERS_H
#define PLAYERS_H

#include <hash_map>

using namespace std;
using namespace stdext;

class Player;

struct Pos;

namespace Players {
	extern hash_map <int, Player*> players;
	extern hash_map <char*, Player*> names;
	void addPlayer(Player* player);
	void deletePlayer(Player* player);
	void handleMoving(Player* player, unsigned char* packet, int size);
	void damagePlayer(Player* player, unsigned char* packet);
	void faceExperiment(Player* player, unsigned char* packet);
	void chatHandler(Player* player, unsigned char* packet);
	void healPlayer(Player* player, unsigned char* packet);
	void getPlayerInfo(Player* player, unsigned char* packet);
	void commandHandler(Player* player, unsigned char* packet);
};

#endif