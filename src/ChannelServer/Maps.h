/*
Copyright (C) 2008 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
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
	char script[30];
};
typedef vector<PortalInfo> PortalsInfo;

struct MapInfo {
	int rm;
	int forcedReturn;
	double spawnrate;
	bool clock;
	int shipInterval;
	PortalsInfo Portals;
	vector<Player*> Players;
};

namespace Maps {
	extern MapTimer* timer;
	extern hash_map <int, MapInfo> info;
	void startTimer();
	void addMap(int id, MapInfo map);
	void addPlayer(Player *player);
	void moveMap(Player *player, unsigned char* packet);
	void moveMapS(Player *player, unsigned char* packet);
	void removePlayer(Player *player);
	void changeMap(Player *player, int mapid, int pos);
	void showClock(Player *player);
	void mapTimer(int mapid);
	void newMap(Player *player, int mapid);
	void changeMusic(int mapid, std::string musicname);
};

#endif
