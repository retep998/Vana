#ifndef PLAYERS_H
#define PLAYERS_H

#include <hash_map>

using namespace std;
using namespace stdext;

class Player;

struct Pos;

class Players {
public:
	static hash_map <int, Player*> players;
	static hash_map <char*, Player*> names;
	static void addPlayer(Player* player);
	static void deletePlayer(Player* player);
	static void handleMoving(Player* player, unsigned char* packet, int size);
	static void damagePlayer(Player* player, unsigned char* packet);
	static void faceExperiment(Player* player, unsigned char* packet);
	static void chatHandler(Player* player, unsigned char* packet);
	static void healPlayer(Player* player, unsigned char* packet);
	static void getPlayerInfo(Player* player, unsigned char* packet);
	static void searchPlayer(Player* player, unsigned char* packet);
};

#endif