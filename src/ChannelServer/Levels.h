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
	void giveExp(Player *player, uint32_t exp, bool inChat = false, bool white = true);
	void addStat(Player *player, PacketReader &packet);
	void addStatMulti(Player *player, PacketReader &packet);
	void addStat(Player *player, int32_t type, int16_t mod = 1, bool isreset = false);
	int16_t randHp();
	int16_t randMp();
	int16_t levelHp(int16_t val, int16_t bonus = 0);
	int16_t levelMp(int16_t val, int16_t bonus = 0);
	int16_t apResetHp(bool isreset, bool issubtract, int16_t val, int16_t sval = 0);
	int16_t apResetMp(bool isreset, bool issubtract, int16_t val, int16_t sval = 0);
	int16_t getX(Player *player, int32_t skillid);
	int16_t getY(Player *player, int32_t skillid);
	uint32_t getExp(uint8_t level);
};

#endif
