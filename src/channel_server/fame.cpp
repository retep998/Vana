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
#include "fame.hpp"
#include "common/io/database.hpp"
#include "common/packet_reader.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/fame_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"

namespace vana {
namespace channel_server {

auto fame::handle_fame(ref_ptr<player> player, packet_reader &reader) -> void {
	game_player_id target_id = reader.get<game_player_id>();
	uint8_t type = reader.get<uint8_t>();
	if (target_id > 0) {
		if (player->get_id() == target_id) {
			// Hacking
			return;
		}
		int32_t check_result = can_fame(player, target_id);
		if (check_result != 0) {
			player->send(packets::fame::send_error(check_result));
		}
		else {
			auto famee = channel_server::get_instance().get_player_data_provider().get_player(target_id);
			game_fame new_fame = famee->get_stats()->get_fame() + (type == 1 ? 1 : -1);
			famee->get_stats()->set_fame(new_fame);
			add_fame_log(player->get_id(), target_id);
			player->send(packets::fame::send_fame(famee->get_name(), type, new_fame));
			famee->send(packets::fame::receive_fame(player->get_name(), type));
		}
	}
	else {
		player->send(packets::fame::send_error(packets::fame::errors::incorrect_user));
	}
}

auto fame::can_fame(ref_ptr<player> player, game_player_id to) -> int32_t {
	game_player_id from = player->get_id();
	if (player->get_stats()->get_level() < 15) {
		return packets::fame::errors::level_under15;
	}
	if (get_last_fame_log(from) == search_result::found) {
		return packets::fame::errors::already_famed_today;
	}
	if (get_last_fame_sp_log(from, to) == search_result::found) {
		return packets::fame::errors::famed_this_month;
	}
	return 0;
}

auto fame::add_fame_log(game_player_id from, game_player_id to) -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	sql.once
		<< "INSERT INTO " << db.make_table(vana::table::fame_log) << " (from_character_id, to_character_id, fame_time) "
		<< "VALUES (:from, :to, NOW())",
		soci::use(from, "from"),
		soci::use(to, "to");
}

auto fame::get_last_fame_log(game_player_id from) -> search_result {
	int32_t fame_time = static_cast<int32_t>(channel_server::get_instance().get_config().fame_time.count());
	if (fame_time == 0) {
		return search_result::found;
	}
	if (fame_time == -1) {
		return search_result::not_found;
	}

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	optional<unix_time> time;

	sql.once
		<< "SELECT fame_time "
		<< "FROM " << db.make_table(vana::table::fame_log) << " "
		<< "WHERE from_character_id = :from AND UNIX_TIMESTAMP(fame_time) > UNIX_TIMESTAMP() - :fame_time "
		<< "ORDER BY fame_time DESC",
		soci::use(from, "from"),
		soci::use(fame_time, "fame_time"),
		soci::into(time);

	return time.is_initialized() ?
		search_result::found :
		search_result::not_found;
}

auto fame::get_last_fame_sp_log(game_player_id from, game_player_id to) -> search_result {
	int32_t fame_reset_time = static_cast<int32_t>(channel_server::get_instance().get_config().fame_reset_time.count());
	if (fame_reset_time == 0) {
		return search_result::found;
	}
	if (fame_reset_time == -1) {
		return search_result::not_found;
	}

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	optional<unix_time> time;

	sql.once
		<< "SELECT fame_time "
		<< "FROM " << db.make_table(vana::table::fame_log) << " "
		<< "WHERE from_character_id = :from AND to_character_id = :to AND UNIX_TIMESTAMP(fame_time) > UNIX_TIMESTAMP() - :fame_reset_time "
		<< "ORDER BY fame_time DESC",
		soci::use(from, "from"),
		soci::use(to, "to"),
		soci::use(fame_reset_time, "fame_reset_time"),
		soci::into(time);

	return time.is_initialized() ?
		search_result::found :
		search_result::not_found;
}

}
}