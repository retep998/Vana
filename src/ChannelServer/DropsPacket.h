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
#ifndef DROPPACK_H
#define DROPPACK_H

#include <vector>

using namespace std;

class Player;
class Drop;
struct Dropped;
struct Pos;

namespace DropsPacket {
	void drop(Drop *drop, Pos origin);
	void dropForPlayer(Player *player, Drop *drop, Pos origin);
	void showDrop(Player *player, Drop *drop);
	void takeNote(Player *player, int id, bool ismesos, short amount);
	void takeDrop(Player *player, Drop *drop);
	void dontTake(Player *player);
	void removeDrop(Drop *drop);
	void explodeDrop(Drop *drop);
};

#endif
