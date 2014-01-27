/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "ChatHandlerFunctions.hpp"

class Player;

namespace ManagementFunctions {
	auto map(Player *player, const string_t &args) -> bool;
	auto warp(Player *player, const string_t &args) -> bool;
	auto changeChannel(Player *player, const string_t &args) -> bool;
	auto lag(Player *player, const string_t &args) -> bool;
	auto header(Player *player, const string_t &args) -> bool;
	auto shutdown(Player *player, const string_t &args) -> bool;
	auto kick(Player *player, const string_t &args) -> bool;
	auto relog(Player *player, const string_t &args) -> bool;
	auto calculateRanks(Player *player, const string_t &args) -> bool;
	auto item(Player *player, const string_t &args) -> bool;
	auto storage(Player *player, const string_t &args) -> bool;
	auto shop(Player *player, const string_t &args) -> bool;
	auto reload(Player *player, const string_t &args) -> bool;
	auto npc(Player *player, const string_t &args) -> bool;
	auto addNpc(Player *player, const string_t &args) -> bool;
	auto killNpc(Player *player, const string_t &args) -> bool;
	auto kill(Player *player, const string_t &args) -> bool;
	auto ban(Player *player, const string_t &args) -> bool;
	auto tempBan(Player *player, const string_t &args) -> bool;
	auto ipBan(Player *player, const string_t &args) -> bool;
	auto unban(Player *player, const string_t &args) -> bool;
	auto rehash(Player *player, const string_t &args) -> bool;
	auto rates(Player *player, const string_t &args) -> bool;
}