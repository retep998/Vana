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

#include "Map.h"
#include <hash_map>
#include <string>

using std::string;
using stdext::hash_map;

class Player;
class ReadPacket;

namespace Maps {
	extern hash_map<int, Map *> maps;
	void startTimer();
	void addMap(int id);
	void moveMap(Player *player, ReadPacket *packet);
	void moveMapS(Player *player, ReadPacket *packet);
	void changeMap(Player *player, int mapid, int portalid);
	void showClock(Player *player);
	void mapTimer(int mapid);
	void newMap(Player *player, int mapid);
	void changeMusic(int mapid, const string &musicname);
};

#endif
