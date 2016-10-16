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
#include "party.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/instance.hpp"
#include "channel_server/map_packet.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/party_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/player_skills.hpp"
#include "channel_server/world_server_packet.hpp"

namespace vana {
namespace channel_server {

party::party(game_party_id party_id) :
	m_party_id{party_id}
{
}

auto party::set_leader(game_player_id player_id, bool show_packet) -> void {
	m_leader_id = player_id;
	if (show_packet) {
		run_function([this, player_id](ref_ptr<player> player) {
			player->send(packets::party::set_leader(this, player_id));
		});
	}
}

namespace functors {
	struct join_party_update {
		auto operator()(ref_ptr<player> target) -> void {
			target->send(packets::party::join_party(target->get_map_id(), party, player));
		}
		party *party;
		string player;
	};
}

auto party::add_member(ref_ptr<player> player_value, bool first) -> void {
	m_members[player_value->get_id()] = player_value;
	player_value->set_party(this);
	show_hp_bar(player_value);
	receive_hp_bar(player_value);

	if (!first) {
		// This must be executed first in case the player has an open door already
		// The town position will need to change upon joining
		player_value->get_skills()->on_join_party(this, player_value);

		run_function([&](ref_ptr<player> party_member) {
			if (party_member != player_value) {
				party_member->get_skills()->on_join_party(this, player_value);
			}
		});

		functors::join_party_update func = {this, player_value->get_name()};
		run_function(func);
	}
}

auto party::add_member(game_player_id id, const string &name, bool first) -> void {
	m_members[id] = nullptr;
	if (!first) {
		functors::join_party_update func = {this, name};
		run_function(func);
	}
}

auto party::set_member(game_player_id player_id, ref_ptr<player> player) -> void {
	m_members[player_id] = player;
}

namespace functors {
	struct leave_party_update {
		auto operator()(ref_ptr<player> target) -> void {
			target->send(packets::party::leave_party(target->get_map_id(), party, player_id, player, kicked));
		}
		party *party;
		game_player_id player_id;
		string player;
		bool kicked;
	};
}

auto party::delete_member(ref_ptr<player> player_value, bool kicked) -> void {
	player_value->get_skills()->on_leave_party(this, player_value, kicked);

	m_members.erase(player_value->get_id());
	player_value->set_party(nullptr);
	if (instance *inst = get_instance()) {
		inst->remove_party_member(get_id(), player_value->get_id());
	}

	run_function([&](ref_ptr<player> party_member) {
		if (party_member != player_value) {
			party_member->get_skills()->on_leave_party(this, player_value, kicked);
		}
	});

	functors::leave_party_update func = {this, player_value->get_id(), player_value->get_name(), kicked};
	func(player_value);
	run_function(func);
}

auto party::delete_member(game_player_id id, const string &name, bool kicked) -> void {
	if (instance *inst = get_instance()) {
		inst->remove_party_member(get_id(), id);
	}
	m_members.erase(id);

	functors::leave_party_update func = {this, id, name, kicked};
	run_function(func);
}

auto party::disband() -> void {
	if (instance *inst = get_instance()) {
		inst->party_disband(get_id());
		set_instance(nullptr);
	}

	run_function([&](ref_ptr<player> party_member) {
		party_member->get_skills()->on_party_disband(this);
	});

	auto temp = m_members;
	for (const auto &kvp : temp) {
		if (auto player = kvp.second) {
			player->set_party(nullptr);
			player->send(packets::party::disband_party(this));
		}
		m_members.erase(kvp.first);
	}
}

auto party::silent_update() -> void {
	run_function([this](ref_ptr<player> player) {
		player->send(packets::party::silent_update(player->get_map_id(), this));
	});
}

auto party::get_member_by_index(uint8_t one_based_index) -> ref_ptr<player> {
	ref_ptr<player> member = nullptr;
	if (one_based_index <= m_members.size()) {
		uint8_t f = 0;
		for (const auto &kvp : m_members) {
			f++;
			if (f == one_based_index) {
				member = kvp.second;
				break;
			}
		}
	}
	return member;
}

auto party::get_zero_based_index_by_member(ref_ptr<player> player) -> int8_t {
	int8_t index = 0;
	for (const auto &kvp : m_members) {
		if (kvp.second == player) {
			return index;
		}
		index++;
	}
	return -1;
}

auto party::run_function(function<void(ref_ptr<player>)> func) -> void {
	for (const auto &kvp : m_members) {
		if (auto player = kvp.second) {
			func(player);
		}
	}
}

auto party::get_all_player_ids() -> vector<game_player_id> {
	vector<game_player_id> player_ids;
	for (const auto &kvp : m_members) {
		player_ids.push_back(kvp.first);
	}
	return player_ids;
}

auto party::get_party_members(game_map_id map_id) -> vector<ref_ptr<player>> {
	vector<ref_ptr<player>> players;
	run_function([&players, &map_id](ref_ptr<player> player) {
		if (map_id == -1 || player->get_map_id() == map_id) {
			players.push_back(player);
		}
	});
	return players;
}

auto party::show_hp_bar(ref_ptr<player> player_value) -> void {
	run_function([&player_value](ref_ptr<player> test_player) {
		if (test_player != player_value && test_player->get_map_id() == player_value->get_map_id()) {
			test_player->send(packets::player::show_hp_bar(player_value->get_id(), player_value->get_stats()->get_hp(), player_value->get_stats()->get_max_hp()));
		}
	});
}

auto party::receive_hp_bar(ref_ptr<player> player_value) -> void {
	run_function([&player_value](ref_ptr<player> test_player) {
		if (test_player != player_value && test_player->get_map_id() == player_value->get_map_id()) {
			player_value->send(packets::player::show_hp_bar(test_player->get_id(), test_player->get_stats()->get_hp(), test_player->get_stats()->get_max_hp()));
		}
	});
}

auto party::get_member_count_on_map(game_map_id map_id) -> int8_t {
	int8_t count = 0;
	for (const auto &kvp : m_members) {
		if (auto test = kvp.second) {
			if (test->get_map_id() == map_id) {
				count++;
			}
		}
	}
	return count;
}

auto party::is_within_level_range(game_player_level low_bound, game_player_level high_bound) -> bool {
	bool ret = true;
	for (const auto &kvp : m_members) {
		if (auto test = kvp.second) {
			if (test->get_stats()->get_level() < low_bound || test->get_stats()->get_level() > high_bound) {
				ret = false;
				break;
			}
		}
	}
	return ret;
}

auto party::warp_all_members(game_map_id map_id, const string &portal_name) -> void {
	if (map *destination = maps::get_map(map_id)) {
		const data::type::portal_info * const destination_portal = destination->query_portal_name(portal_name);
		run_function([&](ref_ptr<player> test) {
			test->set_map(map_id, destination_portal);
		});
	}
}

auto party::check_footholds(int8_t member_count, const vector<vector<game_foothold_id>> &footholds) -> result {
	// Determines if the players are properly arranged (i.e. 5 people on 5 barrels in Kerning PQ)
	result winner = result::success;
	int8_t members_on_footholds = 0;
	hash_set<size_t> foothold_groups_used;

	for (size_t group = 0; group < footholds.size(); group++) {
		const auto &group_footholds = footholds[group];
		for (const auto &kvp : m_members) {
			if (auto test = kvp.second) {
				for (const auto &foothold : group_footholds) {
					if (test->get_foothold() == foothold) {
						if (foothold_groups_used.find(group) != std::end(foothold_groups_used)) {
							winner = result::failure;
						}
						else {
							foothold_groups_used.insert(group);
							members_on_footholds++;
						}
						break;
					}
				}
			}
			if (winner == result::failure) {
				break;
			}
		}
		if (winner == result::failure) {
			break;
		}
	}
	if (winner == result::success && members_on_footholds != member_count) {
		// Not all the foothold groups were indexed
		winner = result::failure;
	}
	return winner;
}

auto party::verify_footholds(const vector<vector<game_foothold_id>> &footholds) -> result {
	// Determines if the players match your selected footholds
	result winner = result::success;
	hash_set<size_t> foothold_groups_used;

	for (size_t group = 0; group < footholds.size(); group++) {
		const auto &group_footholds = footholds[group];
		for (const auto &kvp : m_members) {
			if (auto test = kvp.second) {
				for (const auto &foothold : group_footholds) {
					if (test->get_foothold() == foothold) {
						if (foothold_groups_used.find(group) != std::end(foothold_groups_used)) {
							winner = result::failure;
						}
						else {
							foothold_groups_used.insert(group);
						}
						break;
					}
				}
				if (winner == result::failure) {
					break;
				}
			}
		}
		if (winner == result::failure) {
			break;
		}
	}
	if (winner == result::success) {
		winner = foothold_groups_used.size() == footholds.size() ? result::success : result::failure;
	}
	return winner;
}

}
}