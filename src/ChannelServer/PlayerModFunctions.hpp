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

		namespace PlayerModFunctions {
			auto disconnect(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto save(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto modMesos(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto heal(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto modStr(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto modDex(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto modInt(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto modLuk(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto maxStats(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto hp(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto mp(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto sp(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto ap(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto fame(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto level(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto job(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto addSp(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
			auto maxSp(ref_ptr_t<Player> player, const chat_t &args) -> ChatResult;
		}
	}
}