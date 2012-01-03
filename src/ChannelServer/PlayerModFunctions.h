/*
Copyright (C) 2008-2012 Vana Development Team

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

namespace PlayerModFunctions {
	bool disconnect(Player *player, const string &args);
	bool save(Player *player, const string &args);
	bool modMesos(Player *player, const string &args);
	bool heal(Player *player, const string &args);
	bool modStr(Player *player, const string &args);
	bool modDex(Player *player, const string &args);
	bool modInt(Player *player, const string &args);
	bool modLuk(Player *player, const string &args);
	bool maxStats(Player *player, const string &args);
	bool hp(Player *player, const string &args);
	bool mp(Player *player, const string &args);
	bool sp(Player *player, const string &args);
	bool ap(Player *player, const string &args);
	bool fame(Player *player, const string &args);
	bool level(Player *player, const string &args);
	bool job(Player *player, const string &args);
	bool addSp(Player *player, const string &args);
}