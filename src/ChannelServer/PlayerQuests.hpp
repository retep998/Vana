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

#include "Common/FileTime.hpp"
#include "Common/Quest.hpp"
#include "Common/QuestDataProvider.hpp"
#include "Common/Types.hpp"
#include "ChannelServer/Quests.hpp"
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	class packet_builder;

	namespace channel_server {
		class player;

		struct active_quest {
			auto get_quest_data() const -> string {
				if (kills.size() == 0) {
					return data;
				}

				out_stream info;
				for (const auto &kvp : kills) {
					info << std::setw(3) << std::setfill('0') << kvp.second;
				}
				return info.str();
			}

			game_quest_id id = 0;
			bool done = false;
			string data;
			ord_map<game_mob_id, uint16_t> kills;
		};

		// TODO FIXME accuracy
		// Potentially refactor quest drop display to the MAP instead of the drops, because that's how global does it
		enum class allow_quest_item_result {
			allow,
			disallow,
		};

		class player_quests {
			NONCOPYABLE(player_quests);
			NO_DEFAULT_CONSTRUCTOR(player_quests);
		public:
			player_quests(player *player);

			auto load() -> void;
			auto save() -> void;
			auto connect_packet(packet_builder &builder) -> void;

			auto item_drop_allowed(game_item_id item_id, game_quest_id quest_id) -> allow_quest_item_result;
			auto add_quest(game_quest_id quest_id, game_npc_id npc_id) -> void;
			auto update_quest_mob(game_mob_id mob_id) -> void;
			auto check_done(active_quest &quest) -> void;
			auto finish_quest(game_quest_id quest_id, game_npc_id npc_id) -> void;
			auto remove_quest(game_quest_id quest_id) -> void;
			auto is_quest_active(game_quest_id quest_id) -> bool;
			auto is_quest_complete(game_quest_id quest_id) -> bool;
			auto set_quest_data(game_quest_id id, const string &data) -> void;
			auto get_quest_data(game_quest_id id) -> string;
		private:
			auto give_rewards(game_quest_id quest_id, bool start) -> result;

			player *m_player = nullptr;
			hash_map<game_mob_id, vector<game_quest_id>> m_mob_to_quest_mapping;
			ord_map<game_quest_id, active_quest> m_quests;
			ord_map<game_quest_id, file_time> m_completed;
		};
	}
}