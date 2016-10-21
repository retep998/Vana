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

#include "channel_server/chat_handler_functions.hpp"

namespace vana {
	namespace channel_server {
		class player;

		namespace management_functions {
			auto user_warp(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto warp(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto follow(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto change_channel(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto lag(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto header(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto shutdown(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto kick(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto relog(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto calculate_ranks(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto item(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto storage(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto shop(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto reload(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto run_npc(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto add_npc(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto kill_npc(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto kill(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto ban(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto temp_ban(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto ip_ban(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto unban(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto rehash(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto rates(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto packet(ref_ptr<player> player, const game_chat &args) -> chat_result;
		}
	}
}