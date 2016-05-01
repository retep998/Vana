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
#include "Login.hpp"
#include "Common/Algorithm.hpp"
#include "Common/Database.hpp"
#include "Common/FileTime.hpp"
#include "Common/GameConstants.hpp"
#include "Common/HashUtilities.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Randomizer.hpp"
#include "Common/Session.hpp"
#include "Common/StringUtilities.hpp"
#include "Common/TimeUtilities.hpp"
#include "Common/UnixTime.hpp"
#include "LoginServer/LoginPacket.hpp"
#include "LoginServer/LoginServer.hpp"
#include "LoginServer/PlayerStatus.hpp"
#include "LoginServer/User.hpp"
#include <iostream>

namespace Vana {
namespace LoginServer {

auto Login::loginUser(ref_ptr_t<User> user, PacketReader &reader) -> void {
	string_t username = reader.get<string_t>();
	string_t password = reader.get<string_t>();

	if (!ext::in_range_inclusive<size_t>(username.size(), Characters::MinNameSize, Characters::MaxNameSize)) {
		// Hacking
		return;
	}
	if (!ext::in_range_inclusive<size_t>(password.size(), Characters::MinPasswordSize, Characters::MaxPasswordSize)) {
		// Hacking
		return;
	}

	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	soci::row row;

	sql.once
		<< "SELECT u.* "
		<< "FROM " << db.makeTable("accounts") << " u "
		<< "WHERE u.username = :user",
		soci::use(username, "user"),
		soci::into(row);

	bool valid = true;
	account_id_t accountId = 0;
	auto userIp = user->getIp();
	string_t ip = userIp.is_initialized() ?
		userIp.get().toString() :
		"disconnected";

	if (!sql.got_data()) {
		user->send(Packets::loginError(Packets::Errors::InvalidUsername));
		valid = false;
	}
	else {
		opt_int32_t ipBanned;

		sql.once
			<< "SELECT i.ip_ban_id "
			<< "FROM " << db.makeTable("ip_bans") << " i "
			<< "WHERE i.ip = :ip",
			soci::use(ip, "ip"),
			soci::into(ipBanned);

		if (sql.got_data() && ipBanned.is_initialized()) {
			std::tm banTime;
			banTime.tm_year = 7100;
			banTime.tm_mon = 0;
			banTime.tm_mday = 1;
			auto time = FileTime{UnixTime{mktime(&banTime)}};
			user->send(Packets::loginBan(0, time));
			valid = false;
		}
		else {
			accountId = row.get<account_id_t>("account_id");
			string_t dbPassword = row.get<string_t>("password");
			opt_string_t salt = row.get<opt_string_t>("salt");
			auto &login = LoginServer::getInstance();
			const auto &saltingPolicy = login.getCharacterAccountSaltingPolicy();

			if (!salt.is_initialized()) {
				// We have an unsalted password
				if (dbPassword != password) {
					user->send(Packets::loginError(Packets::Errors::InvalidPassword));
					valid = false;
				}
				else {
					// We have a valid password, so let's hash the password
					salt = HashUtilities::generateSalt(login.getCharacterAccountSaltSize());
					string_t hashedPassword =
						HashUtilities::hashPassword(password, salt.get(), saltingPolicy);

					sql.once
						<< "UPDATE " << db.makeTable("accounts") << " u "
						<< "SET u.password = :password, u.salt = :salt "
						<< "WHERE u.account_id = :account",
						soci::use(hashedPassword, "password"),
						soci::use(salt.get(), "salt"),
						soci::use(accountId, "account");
				}
			}
			else if (dbPassword != HashUtilities::hashPassword(password, salt.get(), saltingPolicy)) {
				user->send(Packets::loginError(Packets::Errors::InvalidPassword));
				valid = false;
			}
			else if (row.get<int32_t>("online") > 0) {
				user->send(Packets::loginError(Packets::Errors::AlreadyLoggedIn));
				valid = false;
			}
			else if (row.get<bool>("banned") && (!row.get<bool>("admin") || row.get<int32_t>("gm_level") == 0)) {
				auto time = FileTime{row.get<UnixTime>("ban_expire")};
				user->send(Packets::loginBan(row.get<int8_t>("ban_reason"), time));
				valid = false;
			}
		}
	}
	if (!valid) {
		int32_t threshold = LoginServer::getInstance().getInvalidLoginThreshold();
		if (threshold != 0 && user->addInvalidLogin() >= threshold) {
			 // Too many invalid logins
			user->disconnect();
		}
	}
	else {
		LoginServer::getInstance().log(LogType::Login, [&](out_stream_t &log) {
			log << username << " from IP " << ip;
		});

		user->setAccountId(accountId);
		if (LoginServer::getInstance().getPinEnabled()) {
			opt_int32_t pin = row.get<opt_int32_t>("pin");
			if (pin.is_initialized()) {
				user->setPin(pin.get());
			}

			auto userPin = user->getPin();
			user->setStatus(userPin.is_initialized() ?
				PlayerStatus::AskPin :
				PlayerStatus::SetPin);
		}
		else {
			user->setStatus(PlayerStatus::LoggedIn);
		}

		optional_t<gender_id_t> gender = row.get<optional_t<gender_id_t>>("gender");
		if (!gender.is_initialized()) {
			user->setStatus(PlayerStatus::SetGender);
		}
		else {
			user->setGender(gender.get());
		}

		optional_t<UnixTime> quietBan = row.get<optional_t<UnixTime>>("quiet_ban_expire");
		if (quietBan.is_initialized()) {
			time_t banTime = quietBan.get();
			if (time(nullptr) > banTime) {
				sql.once
					<< "UPDATE " << db.makeTable("accounts") << " u "
					<< "SET u.quiet_ban_expire = NULL, u.quiet_ban_reason = NULL "
					<< "WHERE u.account_id = :account",
					soci::use(accountId, "account");
			}
			else {
				user->setQuietBanTime(FileTime{UnixTime{banTime}});
				user->setQuietBanReason(row.get<int8_t>("quiet_ban_reason"));
			}
		}

		user->setCreationTime(FileTime{row.get<UnixTime>("creation_date")});
		user->setCharDeletePassword(row.get<opt_int32_t>("char_delete_password"));
		user->setAdmin(row.get<bool>("admin"));
		user->setGmLevel(row.get<int32_t>("gm_level"));

		user->send(Packets::loginConnect(user, username));
	}
}

auto Login::setGender(ref_ptr_t<User> user, PacketReader &reader) -> void {
	if (user->getStatus() != PlayerStatus::SetGender) {
		// Hacking
		return;
	}
	if (reader.get<int8_t>() == 1) {
		// get<bool> candidate?
		gender_id_t gender = reader.get<gender_id_t>();
		if (gender != Gender::Male && gender != Gender::Female) {
			// Hacking
			return;
		}

		user->setStatus(PlayerStatus::NotLoggedIn);

		auto &db = Database::getCharDb();
		auto &sql = db.getSession();
		sql.once
			<< "UPDATE " << db.makeTable("accounts") << " u "
			<< "SET u.gender = :gender "
			<< "WHERE u.account_id = :account",
			soci::use(gender, "gender"),
			soci::use(user->getAccountId(), "account");

		user->setGender(gender);

		if (LoginServer::getInstance().getPinEnabled()) {
			user->setStatus(PlayerStatus::SetPin);
		}
		else {
			user->setStatus(PlayerStatus::LoggedIn);
		}
		user->send(Packets::genderDone(gender));
	}
}

auto Login::handleLogin(ref_ptr_t<User> user, PacketReader &reader) -> void {
	auto status = user->getStatus();
	if (status == PlayerStatus::SetPin) {
		user->send(Packets::loginProcess(PlayerStatus::SetPin));
	}
	else if (status == PlayerStatus::AskPin) {
		user->send(Packets::loginProcess(PlayerStatus::CheckPin));
		user->setStatus(PlayerStatus::CheckPin);
	}
	else if (status == PlayerStatus::CheckPin) {
		checkPin(user, reader);
	}
	else if (status == PlayerStatus::LoggedIn) {
		user->send(Packets::loginProcess(PlayerStatus::LoggedIn));
		// The player successfully logged in, so let set the login column
		user->setOnline(true);
	}
}

auto Login::checkPin(ref_ptr_t<User> user, PacketReader &reader) -> void {
	if (!LoginServer::getInstance().getPinEnabled()) {
		// Hacking
		return;
	}
	int8_t act = reader.get<int8_t>();
	reader.unk<uint8_t>();
	reader.unk<uint32_t>();

	if (act == 0x00) {
		user->setStatus(PlayerStatus::AskPin);
	}
	else if (act == 0x01) {
		int32_t pin = StringUtilities::lexical_cast<int32_t>(reader.get<string_t>());
		opt_int32_t current = user->getPin();
		if (!current.is_initialized()) {
			// Hacking
			return;
		}
		if (pin == current.get()) {
			user->setStatus(PlayerStatus::LoggedIn);
			handleLogin(user, reader);
		}
		else {
			user->send(Packets::loginProcess(Packets::Errors::InvalidPin));
		}
	}
	else if (act == 0x02) {
		int32_t pin = StringUtilities::lexical_cast<int32_t>(reader.get<string_t>());
		auto current = user->getPin();
		if (!current.is_initialized()) {
			// Hacking
			return;
		}
		if (pin == current.get()) {
			user->setStatus(PlayerStatus::SetPin);
			handleLogin(user, reader);
		}
		else {
			user->send(Packets::loginProcess(Packets::Errors::InvalidPin));
		}
	}
}

auto Login::registerPin(ref_ptr_t<User> user, PacketReader &reader) -> void {
	if (!LoginServer::getInstance().getPinEnabled() || user->getStatus() != PlayerStatus::SetPin) {
		// Hacking
		return;
	}
	if (reader.get<int8_t>() == 0x00) {
		auto pin = user->getPin();
		if (pin.is_initialized()) {
			user->setStatus(PlayerStatus::AskPin);
		}
		return;
	}
	int32_t pin = StringUtilities::lexical_cast<int32_t>(reader.get<string_t>());
	user->setStatus(PlayerStatus::NotLoggedIn);

	auto &db = Database::getCharDb();
	auto &sql = db.getSession();

	sql.once
		<< "UPDATE " << db.makeTable("accounts") << " u "
		<< "SET u.pin = :pin "
		<< "WHERE u.account_id = :account",
		soci::use(pin, "pin"),
		soci::use(user->getAccountId(), "account");

	user->send(Packets::pinAssigned());
}

}
}