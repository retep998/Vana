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
#ifndef PLAYERS_H
#define PLAYERS_H

#include <hash_map>

using stdext::hash_map;

struct Player {
	int id;
	char name[15];
	int channel;
};

class Players {
public:
	static Players * Instance() {
		if (singleton == 0)
			singleton = new Players;
		return singleton;
	}
	void registerPlayer(int id, char *name, int channel);
	void remove(int id, int channel = -1);
	Player * getPlayerFromName(char *name);
	Player * getPlayer(int id) { return players[id]; }
	int size();
private:
	Players() {};
	Players(const Players&);
	Players& operator=(const Players&);
	static Players *singleton;

	hash_map <int, Player *> players;
};

#endif