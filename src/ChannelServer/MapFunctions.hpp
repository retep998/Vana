/*
Copyright (C) 2008-2016 Vana Development Team

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

		namespace MapFunctions {
			auto eventInstruction(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto instruction(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto timer(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto killMob(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto getMobHp(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto listMobs(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto listPortals(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto listReactors(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto listPlayers(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto listNpcs(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto mapDimensions(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto zakum(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto horntail(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto music(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto summon(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto clearDrops(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto killAllMobs(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
		}
	}
}