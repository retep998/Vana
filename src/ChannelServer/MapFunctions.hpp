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
	auto eventInstruction(Player *player, const chat_t &args) -> ChatResult;
	auto instruction(Player *player, const chat_t &args) -> ChatResult;
	auto timer(Player *player, const chat_t &args) -> ChatResult;
	auto killMob(Player *player, const chat_t &args) -> ChatResult;
	auto getMobHp(Player *player, const chat_t &args) -> ChatResult;
	auto listMobs(Player *player, const chat_t &args) -> ChatResult;
	auto listPortals(Player *player, const chat_t &args) -> ChatResult;
	auto listReactors(Player *player, const chat_t &args) -> ChatResult;
	auto listNpcs(Player *player, const chat_t &args) -> ChatResult;
	auto zakum(Player *player, const chat_t &args) -> ChatResult;
	auto horntail(Player *player, const chat_t &args) -> ChatResult;
	auto music(Player *player, const chat_t &args) -> ChatResult;
	auto summon(Player *player, const chat_t &args) -> ChatResult;
	auto clearDrops(Player *player, const chat_t &args) -> ChatResult;
	auto killAllMobs(Player *player, const chat_t &args) -> ChatResult;
}