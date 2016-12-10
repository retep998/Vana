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
#include "player_storage.hpp"
#include "common/algorithm.hpp"
#include "common/io/database.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "common/util/misc.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/player.hpp"
#include "channel_server/storage_packet.hpp"
#include <algorithm>

namespace vana {
namespace channel_server {

player_storage::player_storage(ref_ptr<player> player) :
	m_player{player}
{
	load();
}

player_storage::~player_storage() {
	/* TODO FIXME just convert the damn Item * to ref_ptr or owned_ptr */
	std::for_each(std::begin(m_items), std::end(m_items), [](item *item) { delete item; });
}

auto player_storage::take_item(game_storage_slot slot) -> void {
	auto iter = std::begin(m_items) + slot;
	delete *iter;
	m_items.erase(iter);
}

auto player_storage::set_mesos(game_mesos mesos) -> void {
	m_mesos.set_mesos(mesos);
}

auto player_storage::set_slots(game_storage_slot slots) -> void {
	m_slots = ext::constrain_range(slots, constant::inventory::min_slots_storage, constant::inventory::max_slots_storage);
}

auto player_storage::add_item(item *item) -> void {
	game_inventory inv = vana::util::game_logic::inventory::get_inventory(item->get_id());
	game_storage_slot i;
	for (i = 0; i < m_items.size(); ++i) {
		if (vana::util::game_logic::inventory::get_inventory(m_items[i]->get_id()) > inv) {
			break;
		}
	}
	m_items.insert(std::begin(m_items) + i, item);
}

auto player_storage::get_num_items(game_inventory inv) -> game_storage_slot {
	game_storage_slot item_num = 0;
	for (game_storage_slot i = 0; i < m_items.size(); ++i) {
		if (vana::util::game_logic::inventory::get_inventory(m_items[i]->get_id()) == inv) {
			item_num++;
		}
	}
	return item_num;
}

auto player_storage::modify_mesos(game_mesos mod) -> vana::util::meso_modify_result {
	auto query = m_mesos.modify_mesos(mod);
	if (query.get_result() == stack_result::none) {
		return query;
	}

	if (auto player = m_player.lock()) {
		player->send(packets::storage::change_mesos(get_slots(), query.get_final_amount()));
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");

	return query;
}

auto player_storage::can_modify_mesos(game_mesos mesos) const -> stack_result {
	return m_mesos.can_modify_mesos(mesos);
}

auto player_storage::load() -> void {
	if (auto player = m_player.lock()) {
		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		soci::row row;
		game_account_id account_id = player->get_account_id();
		game_world_id world_id = player->get_world_id();

		sql.once
			<< "SELECT s.slots, s.mesos, s.char_slots "
			<< "FROM " << db.make_table(vana::table::storage) << " s "
			<< "WHERE s.account_id = :account AND s.world_id = :world "
			<< "LIMIT 1",
			soci::use(account_id, "account"),
			soci::use(world_id, "world"),
			soci::into(row);

		if (sql.got_data()) {
			m_slots = row.get<game_storage_slot>("slots");
			m_mesos.set_mesos(row.get<game_mesos>("mesos"));
			m_char_slots = row.get<int32_t>("char_slots");
		}
		else {
			auto &config = channel_server::get_instance().get_config();
			m_slots = config.default_storage_slots;
			m_mesos = 0;
			m_char_slots = config.default_chars;
			sql.once
				<< "INSERT INTO " << db.make_table(vana::table::storage) << " (account_id, world_id, slots, mesos, char_slots) "
				<< "VALUES (:account, :world, :slots, :mesos, :chars)",
				soci::use(account_id, "account"),
				soci::use(world_id, "world"),
				soci::use(m_slots, "slots"),
				soci::use(m_mesos.get_mesos(), "mesos"),
				soci::use(m_char_slots, "chars");
		}

		m_items.reserve(m_slots);

		string location = "storage";

		soci::rowset<> rs = (sql.prepare
			<< "SELECT i.* "
			<< "FROM " << db.make_table(vana::table::items) << " i "
			<< "WHERE i.location = :location AND i.account_id = :account AND i.world_id = :world "
			<< "ORDER BY i.slot ASC",
			soci::use(location, "location"),
			soci::use(account_id, "account"),
			soci::use(world_id, "world"));

		for (const auto &row : rs) {
			item *value = new item{row};
			add_item(value);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_storage::save() -> void {
	if (auto player = m_player.lock()) {
		using namespace soci;
		game_world_id world_id = player->get_world_id();
		game_account_id account_id = player->get_account_id();
		game_player_id player_id = player->get_id();

		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		sql.once
			<< "UPDATE " << db.make_table(vana::table::storage) << " "
			<< "SET slots = :slots, mesos = :mesos, char_slots = :chars "
			<< "WHERE account_id = :account AND world_id = :world",
			use(account_id, "account"),
			use(world_id, "world"),
			use(m_slots, "slots"),
			use(m_mesos.get_mesos(), "mesos"),
			use(m_char_slots, "chars");

		sql.once
			<< "DELETE FROM " << db.make_table(vana::table::items) << " "
			<< "WHERE location = :location AND account_id = :account AND world_id = :world",
			use(item::storage, "location"),
			use(account_id, "account"),
			use(world_id, "world");

		game_storage_slot max = get_num_items();

		if (max > 0) {
			vector<item_db_record> v;
			for (game_storage_slot i = 0; i < max; ++i) {
				item_db_record rec{i, player_id, account_id, world_id, item::storage, get_item(i)};
				v.push_back(rec);
			}
			item::database_insert(db, v);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

}
}