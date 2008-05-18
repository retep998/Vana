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
#ifndef MAPPACK_H
#define MAPPACK_H

class Player;
class Packet;

namespace MapPacket {
	Packet playerPacket(Player* player);
	void showPlayer(Player* player, vector <Player*> players);
	void showPlayers(Player* player, vector <Player*> players);
	void removePlayer(Player* player, vector <Player*> players);
	void changeMap(Player* player);
	void showTime(Player* player, unsigned char hour, unsigned char min, unsigned char sec);
	void makeApple(Player* player);
	void changeMusic(vector <Player*> players, char *musicname); // Change map music
};

#endif
