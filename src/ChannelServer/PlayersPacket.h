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
#ifndef PLAYERSPACK_H
#define PLAYERSPACK_H

#include <vector>

using std::vector;

class Player;
class Packet;

namespace PlayersPacket {
	void showMoving(Player* player, vector <Player*> players, unsigned char* packet, int size);
	void faceExperiment(Player* player, vector <Player*> players, int face);
	void showChat(Player* player, vector <Player*> players, char* msg);
	void damagePlayer(Player* player, vector <Player*> players, int dmg, int mob);
	void showMessage(char* msg, char type);
	void showInfo(Player* player, Player* getinfo);
	void findPlayer(Player* player, char* name, int map, unsigned char is = 0);
	void whisperPlayer(Player* player, Player* target, char* message);
	void sendToPlayers(unsigned char *data, int len);
};

#endif