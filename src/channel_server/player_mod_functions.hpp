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

		namespace player_mod_functions {
			auto disconnect(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto save(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto mod_mesos(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto heal(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto mod_str(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto mod_dex(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto mod_int(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto mod_luk(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto max_stats(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto hp(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto mp(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto sp(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto ap(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto fame(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto level(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto job(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto add_sp(ref_ptr<player> player, const game_chat &args) -> chat_result;
			auto max_sp(ref_ptr<player> player, const game_chat &args) -> chat_result;
		}
	}
}