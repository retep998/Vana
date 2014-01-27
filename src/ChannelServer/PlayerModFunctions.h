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

#include "ChatHandlerFunctions.h"

class Player;

namespace PlayerModFunctions {
	auto disconnect(Player *player, const string_t &args) -> bool;
	auto save(Player *player, const string_t &args) -> bool;
	auto modMesos(Player *player, const string_t &args) -> bool;
	auto heal(Player *player, const string_t &args) -> bool;
	auto modStr(Player *player, const string_t &args) -> bool;
	auto modDex(Player *player, const string_t &args) -> bool;
	auto modInt(Player *player, const string_t &args) -> bool;
	auto modLuk(Player *player, const string_t &args) -> bool;
	auto maxStats(Player *player, const string_t &args) -> bool;
	auto hp(Player *player, const string_t &args) -> bool;
	auto mp(Player *player, const string_t &args) -> bool;
	auto sp(Player *player, const string_t &args) -> bool;
	auto ap(Player *player, const string_t &args) -> bool;
	auto fame(Player *player, const string_t &args) -> bool;
	auto level(Player *player, const string_t &args) -> bool;
	auto job(Player *player, const string_t &args) -> bool;
	auto addSp(Player *player, const string_t &args) -> bool;
	auto maxSp(Player *player, const string_t &args) -> bool;
}