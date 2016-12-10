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
#include "login.hpp"
#include "common/algorithm.hpp"
#include "common/constant/character.hpp"
#include "common/constant/gender.hpp"
#include "common/file_time.hpp"
#include "common/hash_utilities.hpp"
#include "common/io/database.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include "common/unix_time.hpp"
#include "common/util/randomizer.hpp"
#include "common/util/string.hpp"
#include "common/util/time.hpp"
#include "login_server/login_packet.hpp"
#include "login_server/login_server.hpp"
#include "login_server/player_status.hpp"
#include "login_server/user.hpp"
#include <iostream>

namespace vana {
namespace login_server {

auto login::login_user(ref_ptr<user> user_value, packet_reader &reader) -> void {
	string username = reader.get<string>();
	string password = reader.get<string>();

	if (!ext::in_range_inclusive<size_t>(username.size(), constant::character::min_name_size, constant::character::max_name_size)) {
		// Hacking
		return;
	}
	if (!ext::in_range_inclusive<size_t>(password.size(), constant::character::min_password_size, constant::character::max_password_size)) {
		// Hacking
		return;
	}

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	soci::row row;

	sql.once
		<< "SELECT u.* "
		<< "FROM " << db.make_table(vana::table::accounts) << " u "
		<< "WHERE u.username = :user",
		soci::use(username, "user"),
		soci::into(row);

	bool valid = true;
	game_account_id account_id = 0;
	auto user_ip = user_value->get_ip();
	string ip = user_ip.is_initialized() ?
		user_ip.get().to_string() :
		"disconnected";

	if (!sql.got_data()) {
		user_value->send(packets::login_error(packets::errors::invalid_username));
		valid = false;
	}
	else {
		opt_int32_t ip_banned;

		sql.once
			<< "SELECT i.ip_ban_id "
			<< "FROM " << db.make_table(vana::table::ip_bans) << " i "
			<< "WHERE i.ip = :ip",
			soci::use(ip, "ip"),
			soci::into(ip_banned);

		if (sql.got_data() && ip_banned.is_initialized()) {
			std::tm ban_time;
			ban_time.tm_year = 7100;
			ban_time.tm_mon = 0;
			ban_time.tm_mday = 1;
			auto time = file_time{unix_time{mktime(&ban_time)}};
			user_value->send(packets::login_ban(0, time));
			valid = false;
		}
		else {
			account_id = row.get<game_account_id>("account_id");
			string db_password = row.get<string>("password");
			opt_string salt = row.get<opt_string>("salt");
			auto &login = login_server::get_instance();
			const auto &salting_policy = login.get_character_account_salting_policy();

			if (!salt.is_initialized()) {
				// We have an unsalted password
				if (db_password != password) {
					user_value->send(packets::login_error(packets::errors::invalid_password));
					valid = false;
				}
				else {
					// We have a valid password, so let's hash the password
					salt = hash_utilities::generate_salt(login.get_character_account_salt_size());
					string hashed_password =
						hash_utilities::hash_password(password, salt.get(), salting_policy);

					sql.once
						<< "UPDATE " << db.make_table(vana::table::accounts) << " u "
						<< "SET u.password = :password, u.salt = :salt "
						<< "WHERE u.account_id = :account",
						soci::use(hashed_password, "password"),
						soci::use(salt.get(), "salt"),
						soci::use(account_id, "account");
				}
			}
			else if (db_password != hash_utilities::hash_password(password, salt.get(), salting_policy)) {
				user_value->send(packets::login_error(packets::errors::invalid_password));
				valid = false;
			}
			else if (row.get<int32_t>("online") > 0) {
				user_value->send(packets::login_error(packets::errors::already_logged_in));
				valid = false;
			}
			else if (row.get<bool>("banned") && (!row.get<bool>("admin") || row.get<int32_t>("gm_level") == 0)) {
				auto time = file_time{row.get<unix_time>("ban_expire")};
				user_value->send(packets::login_ban(row.get<int8_t>("ban_reason"), time));
				valid = false;
			}
		}
	}
	if (!valid) {
		int32_t threshold = login_server::get_instance().get_invalid_login_threshold();
		if (threshold != 0 && user_value->add_invalid_login() >= threshold) {
			 // Too many invalid logins
			user_value->disconnect();
		}
	}
	else {
		login_server::get_instance().log(vana::log::type::login, [&](out_stream &log) {
			log << username << " from IP " << ip;
		});

		user_value->set_account_id(account_id);
		if (login_server::get_instance().get_pin_enabled()) {
			opt_int32_t pin = row.get<opt_int32_t>("pin");
			if (pin.is_initialized()) {
				user_value->set_pin(pin.get());
			}

			auto user_pin = user_value->get_pin();
			user_value->set_status(user_pin.is_initialized() ?
				player_status::ask_pin :
				player_status::set_pin);
		}
		else {
			user_value->set_status(player_status::logged_in);
		}

		optional<game_gender_id> gender = row.get<optional<game_gender_id>>("gender");
		if (!gender.is_initialized()) {
			user_value->set_status(player_status::set_gender);
		}
		else {
			user_value->set_gender(gender.get());
		}

		optional<unix_time> quiet_ban = row.get<optional<unix_time>>("quiet_ban_expire");
		if (quiet_ban.is_initialized()) {
			time_t ban_time = quiet_ban.get();
			if (time(nullptr) > ban_time) {
				sql.once
					<< "UPDATE " << db.make_table(vana::table::accounts) << " u "
					<< "SET u.quiet_ban_expire = NULL, u.quiet_ban_reason = NULL "
					<< "WHERE u.account_id = :account",
					soci::use(account_id, "account");
			}
			else {
				user_value->set_quiet_ban_time(file_time{unix_time{ban_time}});
				user_value->set_quiet_ban_reason(row.get<int8_t>("quiet_ban_reason"));
			}
		}

		user_value->set_creation_time(file_time{row.get<unix_time>("creation_date")});
		user_value->set_char_delete_password(row.get<opt_int32_t>("char_delete_password"));
		user_value->set_admin(row.get<bool>("admin"));
		user_value->set_gm_level(row.get<int32_t>("gm_level"));

		user_value->send(packets::login_connect(user_value, username));
	}
}

auto login::set_gender(ref_ptr<user> user_value, packet_reader &reader) -> void {
	if (user_value->get_status() != player_status::set_gender) {
		// Hacking
		return;
	}
	if (reader.get<int8_t>() == 1) {
		// get<bool> candidate?
		game_gender_id gender = reader.get<game_gender_id>();
		if (gender != constant::gender::male && gender != constant::gender::female) {
			// Hacking
			return;
		}

		user_value->set_status(player_status::not_logged_in);

		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		sql.once
			<< "UPDATE " << db.make_table(vana::table::accounts) << " u "
			<< "SET u.gender = :gender "
			<< "WHERE u.account_id = :account",
			soci::use(gender, "gender"),
			soci::use(user_value->get_account_id(), "account");

		user_value->set_gender(gender);

		if (login_server::get_instance().get_pin_enabled()) {
			user_value->set_status(player_status::set_pin);
		}
		else {
			user_value->set_status(player_status::logged_in);
		}
		user_value->send(packets::gender_done(gender));
	}
}

auto login::handle_login(ref_ptr<user> user_value, packet_reader &reader) -> void {
	auto status = user_value->get_status();
	if (status == player_status::set_pin) {
		user_value->send(packets::login_process(player_status::set_pin));
	}
	else if (status == player_status::ask_pin) {
		user_value->send(packets::login_process(player_status::check_pin));
		user_value->set_status(player_status::check_pin);
	}
	else if (status == player_status::check_pin) {
		check_pin(user_value, reader);
	}
	else if (status == player_status::logged_in) {
		user_value->send(packets::login_process(player_status::logged_in));
		// The player successfully logged in, so let set the login column
		user_value->set_online(true);
	}
}

auto login::check_pin(ref_ptr<user> user_value, packet_reader &reader) -> void {
	if (!login_server::get_instance().get_pin_enabled()) {
		// Hacking
		return;
	}
	int8_t act = reader.get<int8_t>();
	reader.unk<uint8_t>();
	reader.unk<uint32_t>();

	if (act == 0x00) {
		user_value->set_status(player_status::ask_pin);
	}
	else if (act == 0x01) {
		int32_t pin = vana::util::str::lexical_cast<int32_t>(reader.get<string>());
		opt_int32_t current = user_value->get_pin();
		if (!current.is_initialized()) {
			// Hacking
			return;
		}
		if (pin == current.get()) {
			user_value->set_status(player_status::logged_in);
			handle_login(user_value, reader);
		}
		else {
			user_value->send(packets::login_process(packets::errors::invalid_pin));
		}
	}
	else if (act == 0x02) {
		int32_t pin = vana::util::str::lexical_cast<int32_t>(reader.get<string>());
		auto current = user_value->get_pin();
		if (!current.is_initialized()) {
			// Hacking
			return;
		}
		if (pin == current.get()) {
			user_value->set_status(player_status::set_pin);
			handle_login(user_value, reader);
		}
		else {
			user_value->send(packets::login_process(packets::errors::invalid_pin));
		}
	}
}

auto login::register_pin(ref_ptr<user> user_value, packet_reader &reader) -> void {
	if (!login_server::get_instance().get_pin_enabled() || user_value->get_status() != player_status::set_pin) {
		// Hacking
		return;
	}
	if (reader.get<int8_t>() == 0x00) {
		auto pin = user_value->get_pin();
		if (pin.is_initialized()) {
			user_value->set_status(player_status::ask_pin);
		}
		return;
	}
	int32_t pin = vana::util::str::lexical_cast<int32_t>(reader.get<string>());
	user_value->set_status(player_status::not_logged_in);

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();

	sql.once
		<< "UPDATE " << db.make_table(vana::table::accounts) << " u "
		<< "SET u.pin = :pin "
		<< "WHERE u.account_id = :account",
		soci::use(pin, "pin"),
		soci::use(user_value->get_account_id(), "account");

	user_value->send(packets::pin_assigned());
}

}
}