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
#ifndef LEVELS_H
#define LEVELS_H

#include "Types.h"

class Player;
class PacketReader;

namespace Levels {
	extern uint32_t exps[200];
	void giveEXP(Player *player, uint32_t exp, bool inChat = false, bool white = true);
	void addStat(Player *player, PacketReader &packet);
	void addStatMulti(Player *player, PacketReader &packet);
	void addStat(Player *player, int32_t type, int16_t mod = 1, bool isreset = false);
};

#endif
