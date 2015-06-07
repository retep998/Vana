/*
Copyright (C) 2008-2015 Vana Development Team

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
	auto map(Player *player, const chat_t &args) -> ChatResult;
	auto warp(Player *player, const chat_t &args) -> ChatResult;
	auto follow(Player *player, const chat_t &args) -> ChatResult;
	auto changeChannel(Player *player, const chat_t &args) -> ChatResult;
	auto lag(Player *player, const chat_t &args) -> ChatResult;
	auto header(Player *player, const chat_t &args) -> ChatResult;
	auto shutdown(Player *player, const chat_t &args) -> ChatResult;
	auto kick(Player *player, const chat_t &args) -> ChatResult;
	auto relog(Player *player, const chat_t &args) -> ChatResult;
	auto calculateRanks(Player *player, const chat_t &args) -> ChatResult;
	auto item(Player *player, const chat_t &args) -> ChatResult;
	auto storage(Player *player, const chat_t &args) -> ChatResult;
	auto shop(Player *player, const chat_t &args) -> ChatResult;
	auto reload(Player *player, const chat_t &args) -> ChatResult;
	auto npc(Player *player, const chat_t &args) -> ChatResult;
	auto addNpc(Player *player, const chat_t &args) -> ChatResult;
	auto killNpc(Player *player, const chat_t &args) -> ChatResult;
	auto kill(Player *player, const chat_t &args) -> ChatResult;
	auto ban(Player *player, const chat_t &args) -> ChatResult;
	auto tempBan(Player *player, const chat_t &args) -> ChatResult;
	auto ipBan(Player *player, const chat_t &args) -> ChatResult;
	auto unban(Player *player, const chat_t &args) -> ChatResult;
	auto rehash(Player *player, const chat_t &args) -> ChatResult;
	auto rates(Player *player, const chat_t &args) -> ChatResult;
}