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

		namespace map_functions {
			auto event_instruction(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto instruction(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto timer(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto kill_mob(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto get_mob_hp(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto list_mobs(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto list_portals(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto list_reactors(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto list_players(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto list_npcs(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto map_dimensions(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto zakum(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto horntail(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto music(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto summon(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto clear_drops(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto kill_all_mobs(ref_ptr<player> player, const game_chat &args) -> chat_result;
		}
	}
}