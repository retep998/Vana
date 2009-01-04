/*
Copyright (C) 2008-2009 Vana Development Team

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

#include "Types.h"

class Player;
class Drop;
class Pet;
struct Pos;

namespace DropsPacket {
	void showDrop(Player *player, Drop *drop, int8_t type, bool newdrop, Pos origin);
	void takeNote(Player *player, int32_t id, bool ismesos, int16_t amount);
	void takeDrop(Player *player, Drop *drop, int8_t pet_index = -1);
	void dontTake(Player *player);
	void removeDrop(Drop *drop);
	void explodeDrop(Drop *drop);
};

#endif
