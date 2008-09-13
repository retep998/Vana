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

#include "Types.h"
#include <vector>

using std::vector;

class Player;

namespace LevelsPacket {
	void showEXP(Player *player, int32_t exp, int8_t type);
	void levelUP(Player *player);
	void jobChange(Player *player);
	void statOK(Player *player);
};

#endif
