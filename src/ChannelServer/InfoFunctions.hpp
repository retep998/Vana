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

namespace vana {
	namespace channel_server {
		class player;

		namespace info_functions {
			auto help(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto lookup(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto pos(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto online(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto variable(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto quest_data(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto quest_kills(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto gm_level(ref_ptr<player> player, const game_chat &args) -> chat_result;
		}
	}
}