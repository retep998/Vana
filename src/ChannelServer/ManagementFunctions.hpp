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

#include "ChannelServer/ChatHandlerFunctions.hpp"

namespace Vana {
	namespace ChannelServer {
		class Player;

		namespace ManagementFunctions {
			auto map(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto warp(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto follow(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto changeChannel(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto lag(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto header(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto shutdown(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto kick(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto relog(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto calculateRanks(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto item(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto storage(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto shop(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto reload(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto npc(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto addNpc(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto killNpc(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto kill(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto ban(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto tempBan(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto ipBan(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto unban(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto rehash(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto rates(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
		}
	}
}