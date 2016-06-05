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
#include "PlayerQuests.hpp"
#include "common/algorithm_temp.hpp"
#include "common/database_temp.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/quest_data_provider.hpp"
#include "common/randomizer_temp.hpp"
#include "common/time_utilities.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/Inventory.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/QuestsPacket.hpp"
#include <array>

namespace vana {
namespace channel_server {

player_quests::player_quests(player *player) :
	m_player{player}
{
	load();
}

auto player_quests::save() -> void {
	auto &db = database::get_char_db();
	auto &sql = db.get_session();
	game_player_id char_id = m_player->get_id();
	game_quest_id quest_id = 0;

	sql.once << "DELETE FROM " << db.make_table("active_quests") << " WHERE character_id = :char",
		soci::use(char_id, "char");
	sql.once << "DELETE FROM " << db.make_table("completed_quests") << " WHERE character_id = :char",
		soci::use(char_id, "char");

	if (m_quests.size() > 0) {
		game_mob_id mob_id = 0;
		uint16_t killed = 0;
		int64_t id = 0;
		opt_string data;
		// GCC, as usual, bad with operators
		data = "";

		soci::statement st = (sql.prepare
			<< "INSERT INTO " << db.make_table("active_quests") << " (character_id, quest_id, data) "
			<< "VALUES (:char, :quest, :data)",
			soci::use(char_id, "char"),
			soci::use(quest_id, "quest"),
			soci::use(data, "data"));

		soci::statement st_mobs = (sql.prepare
			<< "INSERT INTO " << db.make_table("active_quests_mobs") << " (active_quest_id, mob_id, quantity_killed) "
			<< "VALUES (:id, :mob, :killed)",
			soci::use(id, "id"),
			soci::use(mob_id, "mob"),
			soci::use(killed, "killed"));

		for (const auto &kvp : m_quests) {
			const string &d = kvp.second.data;
			quest_id = kvp.first;
			if (d.empty()) {
				data.reset();
			}
			else {
				data = d;
			}
			st.execute(true);

			if (kvp.second.kills.size() > 0) {
				id = db.get_last_id<int64_t>();
				for (const auto &kill_pair : kvp.second.kills) {
					mob_id = kill_pair.first;
					killed = kill_pair.second;
					st_mobs.execute(true);
				}
			}
		}
	}

	if (m_completed.size() > 0) {
		int64_t time = 0;

		soci::statement st = (sql.prepare
			<< "INSERT INTO " << db.make_table("completed_quests") << " "
			<< "VALUES (:char, :quest, :time)",
			soci::use(char_id, "char"),
			soci::use(quest_id, "quest"),
			soci::use(time, "time"));

		for (const auto &kvp : m_completed) {
			quest_id = kvp.first;
			time = kvp.second.get_value();
			st.execute(true);
		}
	}
}

auto player_quests::load() -> void {
	auto &db = database::get_char_db();
	auto &sql = db.get_session();
	game_player_id char_id = m_player->get_id();
	game_quest_id previous = 0;
	game_quest_id current = 0;
	bool init = true;
	active_quest cur_quest;

	soci::rowset<> rs = (sql.prepare
		<< "SELECT a.quest_id, am.mob_id, am.quantity_killed, a.data "
		<< "FROM " << db.make_table("active_quests") << " a "
		<< "LEFT OUTER JOIN " << db.make_table("active_quests_mobs") << " am ON am.active_quest_id = a.id "
		<< "WHERE a.character_id = :char ORDER BY a.quest_id ASC",
		soci::use(char_id, "char"));

	for (const auto &row : rs) {
		current = row.get<game_quest_id>("quest_id");
		game_mob_id mob = row.get<game_mob_id>("mob_id");
		string data = row.get<string>("data");

		if (init) {
			cur_quest.id = current;
			cur_quest.data = data;
			init = false;
		}
		else if (previous != -1 && current != previous) {
			m_quests[previous] = cur_quest;
			cur_quest = active_quest{};
			cur_quest.id = current;
			cur_quest.data = data;
		}
		if (mob != 0) {
			uint16_t kills = row.get<uint16_t>("quantity_killed");
			cur_quest.kills[mob] = kills;
			m_mob_to_quest_mapping[mob].push_back(current);
		}
		previous = current;
	}
	if (!init) {
		m_quests[previous] = cur_quest;
	}

	rs = (sql.prepare << "SELECT c.quest_id, c.end_time FROM " << db.make_table("completed_quests") << " c WHERE c.character_id = :char",
		soci::use(char_id, "char"));

	for (const auto &row : rs) {
		m_completed[row.get<game_quest_id>("quest_id")] = file_time{row.get<int64_t>("end_time")};
	}
}

auto player_quests::add_quest(game_quest_id quest_id, game_npc_id npc_id) -> void {
	m_player->send(packets::quests::accept_quest_notice(quest_id));
	m_player->send(packets::quests::accept_quest(quest_id, npc_id));

	active_quest quest;
	quest.id = quest_id;
	m_quests[quest_id] = quest;

	auto &quest_info = channel_server::get_instance().get_quest_data_provider().get_info(quest_id);
	quest_info.for_each_request(false, [&](const quest_request_info &info) -> iteration_result {
		if (info.is_mob) {
			quest.kills[info.id] = 0;
			m_mob_to_quest_mapping[info.id].push_back(quest_id);
		}
		return iteration_result::continue_iterating;
	});

	give_rewards(quest_id, true);
	check_done(m_quests[quest_id]);
}

auto player_quests::update_quest_mob(game_mob_id mob_id) -> void {
	auto kvp = m_mob_to_quest_mapping.find(mob_id);
	if (kvp == std::end(m_mob_to_quest_mapping)) {
		return;
	}

	for (const auto &quest_id : kvp->second) {
		auto &quest = m_quests[quest_id];
		if (quest.done) {
			continue;
		}

		auto &quest_info = channel_server::get_instance().get_quest_data_provider().get_info(quest_id);
		bool possibly_completed = false;
		bool any_update = false;
		quest_info.for_each_request(false, [&](const quest_request_info &info) -> iteration_result {
			if (info.is_mob && info.id == mob_id && quest.kills[info.id] < info.count) {
				quest.kills[info.id]++;
				any_update = true;
				if (info.count == quest.kills[info.id]) {
					possibly_completed = true;
				}
			}
			return iteration_result::continue_iterating;
		});

		if (any_update) {
			m_player->send(packets::quests::update_quest(quest));
		}
		if (possibly_completed) {
			check_done(quest);
		}
	}
}

auto player_quests::check_done(active_quest &quest) -> void {
	auto &quest_info = channel_server::get_instance().get_quest_data_provider().get_info(quest.id);

	quest.done = completion_result::complete == quest_info.for_each_request(false, [&](const quest_request_info &info) -> iteration_result {
		if (info.is_mob) {
			if (quest.kills[info.id] < info.count) {
				return iteration_result::stop_iterating;
			}
		}
		else if (info.is_item) {
			if (m_player->get_inventory()->get_item_amount(info.id) < info.count) {
				return iteration_result::stop_iterating;
			}
		}
		return iteration_result::continue_iterating;
	});

	if (quest.done) {
		m_player->send(packets::quests::done_quest(quest.id));
	}
}

auto player_quests::finish_quest(game_quest_id quest_id, game_npc_id npc_id) -> void {
	auto &quest_info = channel_server::get_instance().get_quest_data_provider().get_info(quest_id);

	if (give_rewards(quest_id, false) == result::failure) {
		// Don't complete the quest yet
		return;
	}

	quest_info.for_each_request(false, [&](const quest_request_info &info) -> iteration_result {
		if (info.is_mob) {
			auto &mapping = m_mob_to_quest_mapping[info.id];
			if (mapping.size() == 1) {
				m_mob_to_quest_mapping.erase(info.id);
			}
			else {
				ext::remove_element(mapping, quest_id);
			}
		}
		return iteration_result::continue_iterating;
	});

	m_quests.erase(quest_id);
	file_time end_time{};
	m_completed[quest_id] = end_time;
	m_player->send(packets::quests::complete_quest_notice(quest_id, end_time));
	m_player->send(packets::quests::complete_quest(quest_id, npc_id, quest_info.get_next_quest()));
	m_player->send_map(packets::quests::complete_quest_animation(m_player->get_id()));
}

auto player_quests::item_drop_allowed(game_item_id item_id, game_quest_id quest_id) -> allow_quest_item_result {
	if (!is_quest_active(quest_id)) {
		return allow_quest_item_result::disallow;
	}
	auto &info = channel_server::get_instance().get_quest_data_provider().get_info(quest_id);
	game_slot_qty quest_amount = 0;
	info.for_each_request(false, [&quest_amount, item_id](const quest_request_info &info) -> iteration_result {
		if (info.is_item && info.id == item_id) {
			quest_amount += info.count;
		}
		return iteration_result::continue_iterating;
	});

	if (m_player->get_inventory()->get_item_amount(item_id) >= quest_amount) {
		return allow_quest_item_result::disallow;
	}

	return allow_quest_item_result::allow;
}

auto player_quests::give_rewards(game_quest_id quest_id, bool start) -> result {
	auto &quest_info = channel_server::get_instance().get_quest_data_provider().get_info(quest_id);

	game_job_id job = m_player->get_stats()->get_job();
	array<game_inventory, inventories::count> needed_slots = {0};
	array<bool, inventories::count> chance_item = {false};

	auto check_rewards = [this, &quest_id, &needed_slots, &chance_item](const quest_reward_info &info) -> iteration_result {
		if (info.is_item) {
			game_inventory inv = game_logic_utilities::get_inventory(info.id) - 1;
			if (info.count > 0) {
				if (info.prop > 0 && !chance_item[inv]) {
					chance_item[inv] = true;
					needed_slots[inv]++;
				}
				else if (info.prop == 0) {
					needed_slots[inv]++;
				}
			}
		}
		else if (info.is_mesos) {
			game_mesos mesos = info.id + m_player->get_inventory()->get_mesos();
			if (mesos < 0) {
				// Will trigger for both too low and too high
				m_player->send(packets::quests::quest_error(quest_id, packets::quests::error_not_enough_mesos));
				return iteration_result::stop_iterating;
			}
		}

		return iteration_result::continue_iterating;
	};

	if (quest_info.for_each_reward(start, job, check_rewards) == completion_result::incomplete) {
		return result::failure;
	}

	for (game_inventory i = 0; i < inventories::count; i++) {
		if (needed_slots[i] != 0 && m_player->get_inventory()->get_open_slots_num(i + 1) < needed_slots[i]) {
			m_player->send(packets::quests::quest_error(quest_id, packets::quests::error_no_item_space));
			return result::failure;
		}
	}

	vector<quest_reward_info> items;
	int32_t chance = 0;
	auto ref_player = ref_ptr<player>{m_player};
	quest_info.for_each_reward(start, job, [this, ref_player, &chance, &items](const quest_reward_info &info) -> iteration_result {
		if (info.is_item && info.prop > 0) {
			chance += info.prop;
			items.push_back(info);
		}
		else if (info.is_item) {
			if (info.count > 0) {
				m_player->send(packets::quests::give_item(info.id, info.count));
				inventory::add_new_item(ref_player, info.id, info.count);
			}
			else if (info.count < 0) {
				m_player->send(packets::quests::give_item(info.id, info.count));
				inventory::take_item(ref_player, info.id, -info.count);
			}
			else if (info.id > 0) {
				m_player->send(packets::quests::give_item(info.id, -m_player->get_inventory()->get_item_amount(info.id)));
				inventory::take_item(ref_player, info.id, m_player->get_inventory()->get_item_amount(info.id));
			}
		}
		else if (info.is_exp) {
			m_player->get_stats()->give_exp(static_cast<uint32_t>(info.id) * channel_server::get_instance().get_config().rates.quest_exp_rate, true);
		}
		else if (info.is_mesos) {
			m_player->get_inventory()->modify_mesos(info.id);
			m_player->send(packets::quests::give_mesos(info.id));
		}
		else if (info.is_fame) {
			m_player->get_stats()->set_fame(m_player->get_stats()->get_fame() + static_cast<game_fame>(info.id));
			m_player->send(packets::quests::give_fame(info.id));
		}
		else if (info.is_buff) {
			inventory::use_item(ref_player, info.id);
		}
		else if (info.is_skill) {
			m_player->get_skills()->set_max_skill_level(info.id, static_cast<game_skill_level>(info.master_level), true);
			if (!info.master_level_only && info.count) {
				m_player->get_skills()->add_skill_level(info.id, static_cast<game_skill_level>(info.count), true);
			}
		}

		return iteration_result::continue_iterating;
	});

	if (chance > 0) {
		int32_t random = randomizer::rand<int32_t>(chance - 1);
		chance = 0;
		for (const auto &info : items) {
			if (chance >= random) {
				m_player->send(packets::quests::give_item(info.id, info.count));
				if (info.count > 0) {
					inventory::add_new_item(ref_player, info.id, info.count);
				}
				else {
					inventory::take_item(ref_player, info.id, -info.count);
				}
				break;
			}
			else {
				chance += info.prop;
			}
		}
	}

	return result::successful;
}

auto player_quests::remove_quest(game_quest_id quest_id) -> void {
	if (is_quest_active(quest_id)) {
		m_quests.erase(quest_id);
		m_player->send(packets::quests::forfeit_quest(quest_id));
	}
}

auto player_quests::is_quest_active(game_quest_id quest_id) -> bool {
	return m_quests.find(quest_id) != std::end(m_quests);
}

auto player_quests::is_quest_complete(game_quest_id quest_id) -> bool {
	return m_completed.find(quest_id) != std::end(m_completed);
}

auto player_quests::connect_packet(packet_builder &builder) -> void {
	builder.add<uint16_t>(static_cast<uint16_t>(m_quests.size()));
	for (const auto &kvp : m_quests) {
		builder.add<game_quest_id>(kvp.first);
		builder.add<string>(kvp.second.get_quest_data());
	}

	builder.add<uint16_t>(static_cast<uint16_t>(m_completed.size()));
	for (const auto &kvp : m_completed) {
		builder.add<game_quest_id>(kvp.first);
		builder.add<file_time>(kvp.second);
	}
}

auto player_quests::set_quest_data(game_quest_id id, const string &data) -> void {
	// TODO FIXME figure out how this works
	// e.g. Battleship quest
	/*
	if (!is_quest_active(id)) {
		m_quests[id] = active_quest{};
		m_player->send(quests_packet::accept_quest(id, 0));
		m_player->send(quests_packet::accept_quest_notice(id));
	}
	*/

	auto &quest = m_quests[id];
	quest.data = data;
	m_player->send(packets::quests::update_quest(quest));
}

auto player_quests::get_quest_data(game_quest_id id) -> string {
	return is_quest_active(id) ? m_quests[id].data : "";
}

}
}