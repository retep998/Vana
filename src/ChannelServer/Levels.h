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
#ifndef LEVELS_H
#define LEVELS_H

class Player;
class ReadPacket;

namespace Levels {
	extern int exps[200];
	void giveEXP(Player *player, long exp, char type=0);
	void addStat(Player *player, ReadPacket *packet);
	void setLevel(Player *player, int level);
	void setJob(Player *player, int job);
};

#endif