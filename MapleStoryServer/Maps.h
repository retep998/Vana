#ifndef MAPS_H
#define MAPS_H

#include <hash_map>
#include <vector>

using namespace std;
using namespace stdext;

class Player;
class MapTimer;

struct PortalInfo {
	int id;
	char from[20];
	int toid;
	char to[20];
	int type;
	short x;
	short y;
};
typedef vector<PortalInfo> PortalsInfo;

struct MapInfo {
	int rm;
	PortalsInfo Portals;
	vector<Player*> Players;
};

namespace Maps {
	extern MapTimer* timer;
	extern hash_map <int, MapInfo> info;
	void startTimer();
	void addMap(int id, MapInfo map);
	void addPlayer(Player* player);
	void moveMap(Player* player, unsigned char* packet);
	void removePlayer(Player* player);
	void changeMap(Player* player, int mapid, int pos);
	void mapTimer(int mapid);
	void newMap(Player* player);
};

#endif