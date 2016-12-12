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

namespace PlayerModFunctions {
	auto disconnect(Player *player, const chat_t &args) -> ChatResult;
	auto save(Player *player, const chat_t &args) -> ChatResult;
	auto modMesos(Player *player, const chat_t &args) -> ChatResult;
	auto heal(Player *player, const chat_t &args) -> ChatResult;
	auto modStr(Player *player, const chat_t &args) -> ChatResult;
	auto modDex(Player *player, const chat_t &args) -> ChatResult;
	auto modInt(Player *player, const chat_t &args) -> ChatResult;
	auto modLuk(Player *player, const chat_t &args) -> ChatResult;
	auto maxStats(Player *player, const chat_t &args) -> ChatResult;
	auto hp(Player *player, const chat_t &args) -> ChatResult;
	auto mp(Player *player, const chat_t &args) -> ChatResult;
	auto sp(Player *player, const chat_t &args) -> ChatResult;
	auto ap(Player *player, const chat_t &args) -> ChatResult;
	auto fame(Player *player, const chat_t &args) -> ChatResult;
	auto level(Player *player, const chat_t &args) -> ChatResult;
	auto job(Player *player, const chat_t &args) -> ChatResult;
	auto addSp(Player *player, const chat_t &args) -> ChatResult;
	auto maxSp(Player *player, const chat_t &args) -> ChatResult;
}