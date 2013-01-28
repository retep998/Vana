/*
Copyright (C) 2008-2013 Vana Development Team

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
#pragma once

#include "ChatHandlerFunctions.h"

class Player;

namespace MapFunctions {
	bool eventInstruction(Player *player, const string &args);
	bool instruction(Player *player, const string &args);
	bool timer(Player *player, const string &args);
	bool killMob(Player *player, const string &args);
	bool getMobHp(Player *player, const string &args);
	bool listMobs(Player *player, const string &args);
	bool zakum(Player *player, const string &args);
	bool horntail(Player *player, const string &args);
	bool music(Player *player, const string &args);
	bool summon(Player *player, const string &args);
	bool clearDrops(Player *player, const string &args);
	bool killAllMobs(Player *player, const string &args);
}