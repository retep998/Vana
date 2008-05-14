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
#include "Players.h"

Players * Players::singleton = 0;

void Players::registerPlayer(int id, char *name, int channel) {
	if (players.find(id) == players.end()) {
		Player *player = new Player();
		player->id = id;
		strcpy_s(player->name, name);
		player->channel = channel;
		players[id] = player;
	}
	else {
		players[id]->channel = channel;
	}
}

void Players::remove(int id, int channel) {
	if (channel == -1 || players[id]->channel == channel) {
		players.erase(id);
	}
}

Player * Players::getPlayerFromName(char *name) {
	Player *player;
	bool found;
	for (hash_map <int, Player *>::iterator iter = players.begin(); iter != players.end(); iter++) {
		if (_stricmp(iter->second->name, name) == 0) {
			player = iter->second;
			found = true;
			break;
		}
	}
	if (!found) {
		player = new Player;
		player->channel = -1;
	}
	return player;
}

int Players::size() {
	return players.size();
}