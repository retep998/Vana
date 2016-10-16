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
#include "player_buddy_list.hpp"
#include "common/algorithm.hpp"
#include "common/io/database.hpp"
#include "common/util/misc.hpp"
#include "common/util/string.hpp"
#include "channel_server/buddy_list_packet.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/player.hpp"
#include "channel_server/sync_packet.hpp"
#include <algorithm>

namespace vana {
namespace channel_server {

player_buddy_list::player_buddy_list(ref_ptr<player> player) :
	m_player{player}
{
	load();
}

auto player_buddy_list::load() -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();

	if (auto player = m_player.lock()) {
		soci::rowset<> rs = (sql.prepare
			<< "SELECT bl.id, bl.buddy_character_id, bl.name AS name_cache, c.name, bl.group_name, CASE WHEN c.online = 1 THEN u.online ELSE 0 END AS `online` "
			<< "FROM " << db.make_table(vana::table::buddylist) << " bl "
			<< "LEFT JOIN " << db.make_table(vana::table::characters) << " c ON bl.buddy_character_id = c.character_id "
			<< "LEFT JOIN " << db.make_table(vana::table::accounts) << " u ON c.account_id = u.account_id "
			<< "WHERE bl.character_id = :char",
			soci::use(player->get_id(), "char"));

		for (const auto &row : rs) {
			add_buddy(db, row);
		}

		rs = (sql.prepare
			<< "SELECT p.* "
			<< "FROM " << db.make_table(vana::table::buddylist_pending) << " p "
			<< "LEFT JOIN " << db.make_table(vana::table::characters) << " c ON c.character_id = p.inviter_character_id "
			<< "WHERE c.world_id = :world AND p.character_id = :char ",
			soci::use(player->get_id(), "char"),
			soci::use(channel_server::get_instance().get_world_id(), "world"));

		buddy_invite invite;
		for (const auto &row : rs) {
			invite = buddy_invite{};
			invite.id = row.get<game_player_id>("inviter_character_id");
			invite.name = row.get<string>("inviter_name");
			m_pending_buddies.push_back(invite);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_buddy_list::add_buddy(const string &name, const string &group, bool invite) -> uint8_t {
	if (auto player = m_player.lock()) {
		if (list_size() >= player->get_buddy_list_size()) {
			// Buddy list full
			return packets::buddy::errors::buddy_list_full;
		}

		if (!ext::in_range_inclusive<size_t>(name.size(), constant::character::min_name_size, constant::character::max_name_size) || group.size() > constant::buddy::max_group_name_size) {
			// Invalid name or group length
			return packets::buddy::errors::user_does_not_exist;
		}

		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		soci::row row;

		sql.once
			<< "SELECT c.character_id, c.name, u.gm_level, u.admin, c.buddylist_size AS buddylist_limit, ("
			<< "	SELECT COUNT(b.id) "
			<< "	FROM " << db.make_table(vana::table::buddylist) << " b "
			<< "	WHERE b.character_id = c.character_id"
			<< ") AS buddylist_size "
			<< "FROM " << db.make_table(vana::table::characters) << " c "
			<< "INNER JOIN " << db.make_table(vana::table::accounts) << " u ON c.account_id = u.account_id "
			<< "WHERE c.name = :name AND c.world_id = :world ",
			soci::use(name, "name"),
			soci::use(channel_server::get_instance().get_world_id(), "world"),
			soci::into(row);

		if (!sql.got_data()) {
			// Name does not exist
			return packets::buddy::errors::user_does_not_exist;
		}

		if (row.get<int32_t>("gm_level") > 0 && !player->is_gm()) {
			// GM cannot be in buddy list unless the player is a GM
			return packets::buddy::errors::no_gms;
		}

		if (row.get<bool>("admin") && !player->is_admin()) {
			return packets::buddy::errors::no_gms;
		}

		if (row.get<int64_t>("buddylist_size") >= row.get<int32_t>("buddylist_limit")) {
			// Opposite-end buddy list full
			return packets::buddy::errors::target_list_full;
		}

		game_player_id char_id = row.get<game_player_id>("character_id");

		if (m_buddies.find(char_id) != std::end(m_buddies)) {
			if (m_buddies[char_id]->group_name == group) {
				// Already in buddy list
				return packets::buddy::errors::already_in_list;
			}
			else {
				sql.once
					<< "UPDATE " << db.make_table(vana::table::buddylist) << " "
					<< "SET group_name = :name "
					<< "WHERE buddy_character_id = :buddy AND character_id = :owner ",
					soci::use(group, "name"),
					soci::use(char_id, "buddy"),
					soci::use(player->get_id(), "owner");

				m_buddies[char_id]->group_name = group;
			}
		}
		else {
			sql.once
				<< "INSERT INTO " << db.make_table(vana::table::buddylist) << " (character_id, buddy_character_id, name, group_name) "
				<< "VALUES (:owner, :buddy, :name, :group)",
				soci::use(name, "name"),
				soci::use(group, "group"),
				soci::use(char_id, "buddy"),
				soci::use(player->get_id(), "owner");

			int32_t row_id = db.get_last_id<int32_t>();

			sql.once
				<< "SELECT bl.id, bl.buddy_character_id, bl.name AS name_cache, c.name, bl.group_name, CASE WHEN c.online = 1 THEN u.online ELSE 0 END AS `online` "
				<< "FROM " << db.make_table(vana::table::buddylist) << " bl "
				<< "LEFT JOIN " << db.make_table(vana::table::characters) << " c ON bl.buddy_character_id = c.character_id "
				<< "LEFT JOIN " << db.make_table(vana::table::accounts) << " u ON c.account_id = u.account_id "
				<< "WHERE bl.id = :row",
				soci::use(row_id, "row"),
				soci::into(row);

			add_buddy(db, row);

			sql.once
				<< "SELECT id "
				<< "FROM " << db.make_table(vana::table::buddylist) << " "
				<< "WHERE character_id = :char AND buddy_character_id = :buddy",
				soci::use(char_id, "char"),
				soci::use(player->get_id(), "buddy"),
				soci::into(row_id);

			if (!sql.got_data()) {
				if (invite) {
					channel_server::get_instance().send_world(packets::interserver::buddy::buddy_invite(player->get_id(), char_id));
				}
			}
			else {
				channel_server::get_instance().send_world(packets::interserver::buddy::readd_buddy(player->get_id(), char_id));
			}
		}

		player->send(packets::buddy::update(player, packets::buddy::action_types::add));
		return packets::buddy::errors::none;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_buddy_list::remove_buddy(game_player_id char_id) -> void {
	if (m_pending_buddies.size() != 0 && m_sent_request) {
		buddy_invite invite = m_pending_buddies.front();
		if (invite.id == char_id) {
			remove_pending_buddy(char_id, false);
		}
		return;
	}

	if (m_buddies.find(char_id) == std::end(m_buddies)) {
		// Hacking
		return;
	}

	if (auto player = m_player.lock()) {
		channel_server::get_instance().send_world(packets::interserver::buddy::remove_buddy(player->get_id(), char_id));
		m_buddies.erase(char_id);

		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		sql.once
			<< "DELETE FROM " << db.make_table(vana::table::buddylist) << " "
			<< "WHERE character_id = :char AND buddy_character_id = :buddy",
			soci::use(player->get_id(), "char"),
			soci::use(char_id, "buddy");

		player->send(packets::buddy::update(player, packets::buddy::action_types::remove));
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_buddy_list::add_buddy(vana::io::database &db, const soci::row &row) -> void {
	game_player_id char_id = row.get<game_player_id>("buddy_character_id");
	int32_t row_id = row.get<int32_t>("id");
	opt_string name = row.get<opt_string>("name");
	opt_string group = row.get<opt_string>("group_name");
	string cache = row.get<string>("name_cache");

	auto &sql = db.get_session();
	if (name.is_initialized() && name.get() != cache) {
		// Outdated name cache, i.e. character renamed
		sql.once
			<< "UPDATE " << db.make_table(vana::table::buddylist) << " "
			<< "SET name = :name "
			<< "WHERE id = :id ",
			soci::use(name.get(), "name"),
			soci::use(row_id, "id");
	}

	ref_ptr<buddy> value = make_ref_ptr<buddy>();
	value->char_id = char_id;

	// Note that the cache is for displaying the character name when the character in question is deleted
	value->name = name.get(cache);

	if (auto player = m_player.lock()) {
		if (!group.is_initialized()) {
			value->group_name = "default group";
			sql.once
				<< "UPDATE " << db.make_table(vana::table::buddylist) << " "
				<< "SET group_name = :name "
				<< "WHERE buddy_character_id = :buddy AND character_id = :owner ",
				soci::use(value->group_name, "name"),
				soci::use(char_id, "buddy"),
				soci::use(player->get_id(), "owner");
		}
		else {
			value->group_name = group.get();
		}

		sql.once
			<< "SELECT id "
			<< "FROM " << db.make_table(vana::table::buddylist) << " "
			<< "WHERE character_id = :char AND buddy_character_id = :buddy ",
			soci::use(char_id, "char"),
			soci::use(player->get_id(), "buddy"),
			soci::into(row_id);

		if (sql.got_data()) {
			value->opposite_status = packets::buddy::opposite_status::registered;
		}
		else {
			value->opposite_status = packets::buddy::opposite_status::unregistered;
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");

	m_buddies[char_id] = value;
}

auto player_buddy_list::add_buddies(packet_builder &builder) -> void {
	auto &provider = channel_server::get_instance().get_player_data_provider();
	for (const auto &kvp : m_buddies) {
		const ref_ptr<buddy> &buddy = kvp.second;
		auto data = provider.get_player_data(buddy->char_id);

		builder.add<int32_t>(buddy->char_id);
		builder.add<string>(buddy->name, 13);
		builder.add<uint8_t>(buddy->opposite_status);

		if (buddy->opposite_status == packets::buddy::opposite_status::unregistered) {
			builder.add<int16_t>(0x00);
			builder.add<uint8_t>(0xF0);
			builder.add<uint8_t>(0xB2);
		}
		else if (data == nullptr) {
			builder.add<int32_t>(-1);
		}
		else {
			builder.add<int32_t>(data->channel.get(-1));
		}

		builder.add<string>(buddy->group_name, 13);
		builder.add<int8_t>(0x00);
		builder.add<int8_t>(20); // Seems to be the amount of buddy slots for the character...
		builder.add<uint8_t>(0xFD);
		builder.add<uint8_t>(0xBA);
	}
}

auto player_buddy_list::check_for_pending_buddy() -> void {
	if (m_pending_buddies.size() == 0 || m_sent_request) {
		// No buddies pending or request sent (didn't receive answer yet)
		return;
	}

	if (auto player = m_player.lock()) {
		player->send(packets::buddy::invitation(m_pending_buddies.front()));
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");

	m_sent_request = true;
}

auto player_buddy_list::buddy_accepted(game_player_id buddy_id) -> void {
	m_buddies[buddy_id]->opposite_status = packets::buddy::opposite_status::registered;
	if (auto player = m_player.lock()) {
		player->send(packets::buddy::update(player, packets::buddy::action_types::add));
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_buddy_list::remove_pending_buddy(game_player_id id, bool accepted) -> void {
	if (m_pending_buddies.size() == 0 || !m_sent_request) {
		// Hacking
		return;
	}

	if (auto player = m_player.lock()) {
		buddy_invite invite = m_pending_buddies.front();
		if (invite.id != id) {
			// Hacking
			channel_server::get_instance().log(vana::log::type::warning, [&](out_stream &log) {
				log << "Player tried to accept a player with player ID " << id
					<< " but the sent player ID was " << invite.id
					<< ". Player: " << player->get_name();
			});
			return;
		}

		if (accepted) {
			int8_t error = add_buddy(invite.name, "default group", false);
			if (error != packets::buddy::errors::none) {
				player->send(packets::buddy::error(error));
			}

			auto &db = vana::io::database::get_char_db();
			auto &sql = db.get_session();
			sql.once
				<< "DELETE FROM " << db.make_table(vana::table::buddylist_pending) << " "
				<< "WHERE character_id = :char AND inviter_character_id = :buddy",
				soci::use(player->get_id(), "char"),
				soci::use(id, "buddy");

			channel_server::get_instance().send_world(packets::interserver::buddy::accept_buddy_invite(player->get_id(), id));
		}

		player->send(packets::buddy::update(player, packets::buddy::action_types::first));

		m_pending_buddies.pop_front();
		m_sent_request = false;
		check_for_pending_buddy();
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_buddy_list::get_buddy_ids() -> vector<game_player_id> {
	vector<game_player_id> ids;
	for (const auto &kvp : m_buddies) {
		if (kvp.second->opposite_status == packets::buddy::opposite_status::registered) {
			ids.push_back(kvp.second->char_id);
		}
	}

	return ids;
}

}
}