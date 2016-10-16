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
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	class packet_builder;
	class soci::row;
	namespace io {
		class database;
	}

	namespace channel_server {
		class player;

		struct buddy {
			uint8_t opposite_status = 0;
			game_player_id char_id = 0;
			string name;
			string group_name;
		};

		struct buddy_invite {
			bool send = true;
			game_player_id id = 0;
			string name;
		};

		class player_buddy_list {
			NONCOPYABLE(player_buddy_list);
			NO_DEFAULT_CONSTRUCTOR(player_buddy_list);
		public:
			player_buddy_list(ref_ptr<player> player);

			auto add_buddy(const string &name, const string &group, bool invite = true) -> uint8_t;
			auto remove_buddy(game_player_id char_id) -> void;

			auto get_buddy(game_player_id char_id) -> ref_ptr<buddy> { return m_buddies[char_id]; }
			auto list_size() const -> uint8_t { return static_cast<uint8_t>(m_buddies.size()); }
			auto get_buddy_ids() -> vector<game_player_id>;
			auto add_buddy_invite(const buddy_invite &invite) -> void { m_pending_buddies.push_back(invite); }

			auto add_buddies(packet_builder &builder) -> void;
			auto check_for_pending_buddy() -> void;
			auto buddy_accepted(game_player_id buddy_id) -> void;
			auto remove_pending_buddy(game_player_id id, bool accepted) -> void;
		private:
			auto add_buddy(vana::io::database &db, const soci::row &row) -> void;
			auto load() -> void;

			bool m_sent_request = false;
			view_ptr<player> m_player;
			queue<buddy_invite> m_pending_buddies;
			hash_map<game_player_id, ref_ptr<buddy>> m_buddies;
		};
	}
}