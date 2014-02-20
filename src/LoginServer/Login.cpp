/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Algorithm.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "LoginPacket.hpp"
#include "LoginServer.hpp"
#include "MiscUtilities.hpp"
#include "PacketReader.hpp"
#include "PlayerStatus.hpp"
#include "Randomizer.hpp"
#include "Session.hpp"
#include "StringUtilities.hpp"
#include "TimeUtilities.hpp"
#include "UserConnection.hpp"
#include "VanaConstants.hpp"
#include <iostream>

auto Login::loginUser(UserConnection *user, PacketReader &reader) -> void {
	string_t username = reader.get<string_t>();
	string_t password = reader.get<string_t>();
	string_t ip = user->getIp().toString();

	if (!ext::in_range_inclusive<size_t>(username.size(), Characters::MinNameSize, Characters::MaxNameSize)) {
		// Hacking
		return;
	}
	if (!ext::in_range_inclusive<size_t>(password.size(), Characters::MinPasswordSize, Characters::MaxPasswordSize)) {
		// Hacking
		return;
	}

	soci::session &sql = Database::getCharDb();
	soci::row row;

	sql.once
		<< "SELECT u.* "
		<< "FROM " << Database::makeCharTable("user_accounts") << " u "
		<< "WHERE u.username = :user",
		soci::use(username, "user"),
		soci::into(row);

	bool valid = true;
	int32_t userId = 0;
	if (!sql.got_data()) {
		user->send(LoginPacket::loginError(LoginPacket::Errors::InvalidUsername));
		valid = false;
	}
	else {
		opt_int32_t ipBanned;

		sql.once
			<< "SELECT i.ip_ban_id "
			<< "FROM " << Database::makeCharTable("ip_bans") << " i "
			<< "WHERE i.ip = :ip",
			soci::use(ip, "ip"),
			soci::into(ipBanned);

		if (sql.got_data() && ipBanned.is_initialized()) {
			std::tm banTime;
			banTime.tm_year = 7100;
			banTime.tm_mon = 0;
			banTime.tm_mday = 1;
			int32_t time = TimeUtilities::timeToTick32(mktime(&banTime));
			user->send(LoginPacket::loginBan(0, time));
			valid = false;
		}
		else {
			userId = row.get<int32_t>("user_id");
			string_t dbPassword = row.get<string_t>("password");
			opt_string_t salt = row.get<opt_string_t>("salt");

			if (!salt.is_initialized()) {
				// We have an unsalted password
				if (dbPassword != password) {
					user->send(LoginPacket::loginError(LoginPacket::Errors::InvalidPassword));
					valid = false;
				}
				else {
					// We have a valid password, so let's hash the password
					salt = MiscUtilities::generateSalt(VanaConstants::SaltSize);
					const string_t &hashedPassword = MiscUtilities::hashPassword(password, salt.get());

					sql.once
						<< "UPDATE " << Database::makeCharTable("user_accounts") << " u "
						<< "SET u.password = :password, u.salt = :salt "
						<< "WHERE u.user_id = :user",
						soci::use(hashedPassword, "password"),
						soci::use(salt.get(), "salt"),
						soci::use(userId, "user");
				}
			}
			else if (dbPassword != MiscUtilities::hashPassword(password, salt.get())) {
				user->send(LoginPacket::loginError(LoginPacket::Errors::InvalidPassword));
				valid = false;
			}
			else if (row.get<bool>("online")) {
				user->send(LoginPacket::loginError(LoginPacket::Errors::AlreadyLoggedIn));
				valid = false;
			}
			else if (row.get<bool>("banned") && (!row.get<bool>("admin") || row.get<int32_t>("gm_level") == 0)) {
				int32_t time = TimeUtilities::timeToTick32(row.get<unix_time_t>("ban_expire"));
				user->send(LoginPacket::loginBan(row.get<int8_t>("ban_reason"), time));
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
		LoginServer::getInstance().log(LogType::Login, [&](out_stream_t &log) { log << username << " from IP " << user->getIp(); });

		user->setUserId(userId);

		if (LoginServer::getInstance().getPinEnabled()) {
			opt_int32_t pin = row.get<opt_int32_t>("pin");
			if (!pin.is_initialized()) {
				user->setPin(-1);
			}
			else {
				user->setPin(pin.get());
			}

			user->setStatus(user->getPin() == -1 ? PlayerStatus::SetPin : PlayerStatus::AskPin);
		}
		else {
			user->setStatus(PlayerStatus::LoggedIn);
		}

		opt_int8_t gender = row.get<opt_int8_t>("gender");
		if (!gender.is_initialized()) {
			user->setStatus(PlayerStatus::SetGender);
		}
		else {
			user->setGender(gender.get());
		}

		opt_unix_time_t quietBan = row.get<opt_unix_time_t>("quiet_ban_expire");
		if (quietBan.is_initialized()) {
			time_t banTime = quietBan.get();
			if (time(nullptr) > banTime) {
				sql.once
					<< "UPDATE " << Database::makeCharTable("user_accounts") << " u "
					<< "SET u.quiet_ban_expire = NULL, u.quiet_ban_reason = NULL "
					<< "WHERE u.user_id = :user",
					soci::use(userId, "user");
			}
			else {
				user->setQuietBanTime(TimeUtilities::timeToTick(banTime));
				user->setQuietBanReason(row.get<int8_t>("quiet_ban_reason"));
			}
		}

		user->setCreationTime(TimeUtilities::timeToTick(row.get<unix_time_t>("creation_date")));
		user->setCharDeletePassword(row.get<opt_int32_t>("char_delete_password"));
		user->setAdmin(row.get<bool>("admin"));
		user->setGmLevel(row.get<int32_t>("gm_level"));

		user->send(LoginPacket::loginConnect(user, username));
	}
}

auto Login::setGender(UserConnection *user, PacketReader &reader) -> void {
	if (user->getStatus() != PlayerStatus::SetGender) {
		// Hacking
		return;
	}
	if (reader.get<int8_t>() == 1) {
		// get<bool> candidate?
		int8_t gender = reader.get<int8_t>();
		if (gender != Gender::Male && gender != Gender::Female) {
			// Hacking
			return;
		}

		user->setStatus(PlayerStatus::NotLoggedIn);

		Database::getCharDb().once
			<< "UPDATE " << Database::makeCharTable("user_accounts") << " u "
			<< "SET u.gender = :gender "
			<< "WHERE u.user_id = :user",
			soci::use(gender, "gender"),
			soci::use(user->getUserId(), "user");

		user->setGender(gender);

		if (LoginServer::getInstance().getPinEnabled()) {
			user->setStatus(PlayerStatus::SetPin);
		}
		else {
			user->setStatus(PlayerStatus::LoggedIn);
		}
		user->send(LoginPacket::genderDone(gender));
	}
}

auto Login::handleLogin(UserConnection *user, PacketReader &reader) -> void {
	int32_t status = user->getStatus();
	if (status == PlayerStatus::SetPin) {
		user->send(LoginPacket::loginProcess(PlayerStatus::SetPin));
	}
	else if (status == PlayerStatus::AskPin) {
		user->send(LoginPacket::loginProcess(PlayerStatus::CheckPin));
		user->setStatus(PlayerStatus::CheckPin);
	}
	else if (status == PlayerStatus::CheckPin) {
		checkPin(user, reader);
	}
	else if (status == PlayerStatus::LoggedIn) {
		user->send(LoginPacket::loginProcess(PlayerStatus::LoggedIn));
		// The player successfully logged in, so let set the login column
		user->setOnline(true);
	}
}

auto Login::checkPin(UserConnection *user, PacketReader &reader) -> void {
	if (!LoginServer::getInstance().getPinEnabled()) {
		// Hacking
		return;
	}
	int8_t act = reader.get<int8_t>();
	reader.skipBytes(5);

	if (act == 0x00) {
		user->setStatus(PlayerStatus::AskPin);
	}
	else if (act == 0x01) {
		int32_t pin = StringUtilities::lexical_cast<int32_t>(reader.get<string_t>());
		int32_t current = user->getPin();
		if (pin == current) {
			user->setStatus(PlayerStatus::LoggedIn);
			handleLogin(user, reader);
		}
		else {
			user->send(LoginPacket::loginProcess(LoginPacket::Errors::InvalidPin));
		}
	}
	else if (act == 0x02) {
		int32_t pin = StringUtilities::lexical_cast<int32_t>(reader.get<string_t>());
		if (pin == user->getPin()) {
			user->setStatus(PlayerStatus::SetPin);
			handleLogin(user, reader);
		}
		else {
			user->send(LoginPacket::loginProcess(LoginPacket::Errors::InvalidPin));
		}
	}
}

auto Login::registerPin(UserConnection *user, PacketReader &reader) -> void {
	if (!LoginServer::getInstance().getPinEnabled() || user->getStatus() != PlayerStatus::SetPin) {
		// Hacking
		return;
	}
	if (reader.get<int8_t>() == 0x00) {
		if (user->getPin() != -1) {
			user->setStatus(PlayerStatus::AskPin);
		}
		return;
	}
	int32_t pin = StringUtilities::lexical_cast<int32_t>(reader.get<string_t>());
	user->setStatus(PlayerStatus::NotLoggedIn);
	Database::getCharDb().once
		<< "UPDATE " << Database::makeCharTable("user_accounts") << " u "
		<< "SET u.pin = :pin "
		<< "WHERE u.user_id = :user",
		soci::use(pin, "pin"),
		soci::use(user->getUserId(), "user");

	user->send(LoginPacket::pinAssigned());
}