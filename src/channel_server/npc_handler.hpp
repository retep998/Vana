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

#include "common/types.hpp"

namespace vana {
	class packet_reader;

	namespace channel_server {
		class player;

		namespace npc_handler {
			auto handle_npc(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_npc_in(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_npc_animation(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_quest_npc(ref_ptr<player> player, game_npc_id npc_id, bool start, game_quest_id quest_id = 0) -> void;
			auto use_shop(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_storage(ref_ptr<player> player, packet_reader &reader) -> void;
			auto show_shop(ref_ptr<player> player, game_shop_id shop_id) -> result;
			auto show_storage(ref_ptr<player> player, game_npc_id npc_id) -> result;
			auto show_guild_rank(ref_ptr<player> player, game_npc_id npc_id) -> result;
		}
	}
}