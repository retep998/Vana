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
#include "user.hpp"
#include "common/io/database.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "login_server/characters.hpp"
#include "login_server/cmsg_header.hpp"
#include "login_server/login.hpp"
#include "login_server/login_packet.hpp"
#include "login_server/login_server.hpp"
#include "login_server/worlds.hpp"
#include <iostream>
#include <stdexcept>

namespace vana {
namespace login_server {

auto user::handle(packet_reader &reader) -> result {
	try {
		switch (reader.get<packet_header>()) {
			case CMSG_ACCOUNT_GENDER: login::set_gender(shared_from_this(), reader); break;
			case CMSG_AUTHENTICATION: login::login_user(shared_from_this(), reader); break;
			case CMSG_CHANNEL_CONNECT: characters::connect_game(shared_from_this(), reader); break;
			case CMSG_CLIENT_ERROR: login_server::get_instance().log(vana::log::type::client_error, reader.get<string>()); break;
			case CMSG_CLIENT_STARTED: login_server::get_instance().log(vana::log::type::info, [&](out_stream &log) { log << "client connected and started from " << get_ip().get(ip{0}); }); break;
			case CMSG_LOGIN_RETURN: send(packets::relog_response()); break;
			case CMSG_PIN: login::handle_login(shared_from_this(), reader); break;
			case CMSG_PLAYER_CREATE: characters::create_character(shared_from_this(), reader); break;
			case CMSG_PLAYER_DELETE: characters::delete_character(shared_from_this(), reader); break;
			case CMSG_PLAYER_GLOBAL_LIST: characters::show_all_characters(shared_from_this()); break;
			case CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT: characters::connect_game_world_from_view_all_characters(shared_from_this(), reader); break;
			case CMSG_PLAYER_LIST: login_server::get_instance().get_worlds().channel_select(shared_from_this(), reader); break;
			case CMSG_PLAYER_NAME_CHECK: characters::check_character_name(shared_from_this(), reader); break;
			case CMSG_REGISTER_PIN: login::register_pin(shared_from_this(), reader); break;
			case CMSG_WORLD_LIST:
			case CMSG_WORLD_LIST_REFRESH: login_server::get_instance().get_worlds().show_world(shared_from_this()); break;
			case CMSG_WORLD_STATUS: login_server::get_instance().get_worlds().select_world(shared_from_this(), reader); break;
		}
	}
	catch (const packet_content_exception &e) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly

		reader.reset();
		login_server::get_instance().log(vana::log::type::malformed_packet, [&](out_stream &log) {
			log << "User ID: " << get_account_id()
				<< "; Packet: " << reader
				<< "; Error: " << e.what();
		});
		return result::failure;
	}

	return result::success;
}

auto user::on_disconnect() -> void {
	set_online(false);
	login_server::get_instance().finalize_user(shared_from_this());
}

auto user::set_online(bool online) -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	sql.once
		<< "UPDATE " << db.make_table(vana::table::accounts) << " u "
		<< "SET "
		<< "	u.online = :online,"
		<< "	u.last_login = NOW() "
		<< "WHERE u.account_id = :id",
		soci::use((online ? 1 : 0), "online"),
		soci::use(m_account_id, "id");
}

}
}