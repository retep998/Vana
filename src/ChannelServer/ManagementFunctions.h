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

namespace ManagementFunctions {
	bool map(Player *player, const string &args);
	bool changeChannel(Player *player, const string &args);
	bool lag(Player *player, const string &args);
	bool header(Player *player, const string &args);
	bool shutdown(Player *player, const string &args);
	bool kick(Player *player, const string &args);
	bool relog(Player *player, const string &args);
	bool calculateRanks(Player *player, const string &args);
	bool item(Player *player, const string &args);
	bool storage(Player *player, const string &args);
	bool shop(Player *player, const string &args);
	bool reload(Player *player, const string &args);
	bool npc(Player *player, const string &args);
	bool addNpc(Player *player, const string &args);
	bool killNpc(Player *player, const string &args);
	bool kill(Player *player, const string &args);
	bool ban(Player *player, const string &args);
	bool tempBan(Player *player, const string &args);
	bool ipBan(Player *player, const string &args);
	bool unban(Player *player, const string &args);
	bool packet(Player *player, const string &args);
}