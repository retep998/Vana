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

#include "common/types_temp.hpp"
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	class packet_builder;

	namespace channel_server {
		class instance;
		class player;

		class party {
			NONCOPYABLE(party);
			NO_DEFAULT_CONSTRUCTOR(party);
		public:
			party(game_party_id party_id);

			auto set_leader(game_player_id player_id, bool show_packet = false) -> void;
			auto set_member(game_player_id player_id, ref_ptr<player> player) -> void;
			auto set_instance(instance *inst) -> void { m_instance = inst; }
			auto is_leader(game_player_id player_id) const -> bool { return player_id == m_leader_id; }
			auto get_members_count() const -> int8_t { return static_cast<int8_t>(m_members.size()); }
			auto get_id() const -> game_party_id { return m_party_id; }
			auto get_leader_id() const -> game_player_id { return m_leader_id; }
			auto get_member(game_player_id id) -> ref_ptr<player> { return m_members.find(id) != std::end(m_members) ? m_members[id] : nullptr; }
			auto get_leader() -> ref_ptr<player> { return m_members[m_leader_id]; }
			auto get_instance() const -> instance * { return m_instance; }

			// More complicated specific functions
			auto add_member(ref_ptr<player> player, bool first = false) -> void;
			auto add_member(game_player_id id, const string &name, bool first = false) -> void;
			auto delete_member(ref_ptr<player> player, bool kicked) -> void;
			auto delete_member(game_player_id id, const string &name, bool kicked) -> void;
			auto disband() -> void;
			auto show_hp_bar(ref_ptr<player> player) -> void;
			auto receive_hp_bar(ref_ptr<player> player) -> void;
			auto silent_update() -> void;
			auto run_function(function<void(ref_ptr<player>)> func) -> void;

			auto warp_all_members(game_map_id map_id, const string &portal_name = "") -> void;
			auto is_within_level_range(game_player_level low_bound, game_player_level high_bound) -> bool;
			auto check_footholds(int8_t member_count, const vector<vector<game_foothold_id>> &footholds) -> result;
			auto verify_footholds(const vector<vector<game_foothold_id>> &footholds) -> result;
			auto get_member_count_on_map(game_map_id map_id) -> int8_t;
			auto get_member_by_index(uint8_t one_based_index) -> ref_ptr<player>;
			auto get_zero_based_index_by_member(ref_ptr<player> player) -> int8_t;
			auto get_all_player_ids() -> vector<game_player_id>;
			auto get_party_members(game_map_id map_id = -1) -> vector<ref_ptr<player>>;

			auto get_members() const -> const ord_map<game_player_id, ref_ptr<player>, std::greater<game_player_id>> & { return m_members; }
		private:
			game_player_id m_leader_id = 0;
			game_party_id m_party_id = 0;
			instance *m_instance = nullptr;
			ord_map<game_player_id, ref_ptr<player>, std::greater<game_player_id>> m_members;
		};
	}
}