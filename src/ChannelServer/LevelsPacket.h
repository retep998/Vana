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
#ifndef LEVELPACK_H
#define LEVELPACK_H

#include <vector>
using namespace std;

class Player;

namespace LevelsPacket {
	void showEXP(Player* player, int exp, char type);
	void levelUP(Player* player, vector <Player*> players);
	void jobChange(Player* player, vector <Player*> players);
	void statOK(Player* player);
	void changeStat(Player* player, int nam, short val);
};

#endif