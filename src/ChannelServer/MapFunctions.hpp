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

namespace MapFunctions {
	auto eventInstruction(Player *player, const string_t &args) -> bool;
	auto instruction(Player *player, const string_t &args) -> bool;
	auto timer(Player *player, const string_t &args) -> bool;
	auto killMob(Player *player, const string_t &args) -> bool;
	auto getMobHp(Player *player, const string_t &args) -> bool;
	auto listMobs(Player *player, const string_t &args) -> bool;
	auto listPortals(Player *player, const string_t &args) -> bool;
	auto listNpcs(Player *player, const string_t &args) -> bool;
	auto zakum(Player *player, const string_t &args) -> bool;
	auto horntail(Player *player, const string_t &args) -> bool;
	auto music(Player *player, const string_t &args) -> bool;
	auto summon(Player *player, const string_t &args) -> bool;
	auto clearDrops(Player *player, const string_t &args) -> bool;
	auto killAllMobs(Player *player, const string_t &args) -> bool;
}