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
#include "Database.hpp"
#include "GameConstants.hpp"
#include "LoginPacket.hpp"
#include "LoginServer.hpp"
#include "MiscUtilities.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "PlayerStatus.hpp"
#include "Randomizer.hpp"
#include "Session.hpp"
#include "StringUtilities.hpp"
#include "TimeUtilities.hpp"
#include "VanaConstants.hpp"
#include <iostream>

auto Login::loginUser(Player *player, PacketReader &packet) -> void {
	const string_t &username = packet.getString();
	const string_t &password = packet.getString();
	const string_t &ip = player->getIp().toString();

	if (!MiscUtilities::inRangeInclusive<size_t>(username.size(), Characters::MinNameSize, Characters::MaxNameSize)) {
		// Hacking
		return;
	}
	if (!MiscUtilities::inRangeInclusive<size_t>(password.size(), Characters::MinPasswordSize, Characters::MaxPasswordSize)) {
		// Hacking
		return;
	}

	soci::session &sql = Database::getCharDb();
	soci::row row;

	sql.once
		<< "SELECT u.* "
		<< "FROM user_accounts u "
		<< "WHERE u.username = :user",
		soci::use(username, "user"),
		soci::into(row);

	bool valid = true;
	int32_t userId = 0;
	if (!sql.got_data()) {
		LoginPacket::loginError(player, LoginPacket::Errors::InvalidUsername);
		valid = false;
	}
	else {
		opt_int32_t ipBanned;

		sql.once
			<< "SELECT i.ip_ban_id "
			<< "FROM ip_bans i "
			<< "WHERE i.ip = :ip",
			soci::use(ip, "ip"),
			soci::into(ipBanned);

		if (sql.got_data() && ipBanned.is_initialized()) {
			std::tm banTime;
			banTime.tm_year = 7100;
			banTime.tm_mon = 0;
			banTime.tm_mday = 1;
			int32_t time = TimeUtilities::timeToTick32(mktime(&banTime));
			LoginPacket::loginBan(player, 0, time);
			valid = false;
		}
		else {
			userId = row.get<int32_t>("user_id");
			const string_t &dbPassword = row.get<string_t>("password");
			opt_string_t salt = row.get<opt_string_t>("salt");

			if (!salt.is_initialized()) {
				// We have an unsalted password
				if (dbPassword != password) {
					LoginPacket::loginError(player, LoginPacket::Errors::InvalidPassword);
					valid = false;
				}
				else {
					// We have a valid password, so let's hash the password
					salt = MiscUtilities::generateSalt(VanaConstants::SaltSize);
					const string_t &hashedPassword = MiscUtilities::hashPassword(password, salt.get());

					sql.once
						<< "UPDATE user_accounts u "
						<< "SET u.password = :password, u.salt = :salt "
						<< "WHERE u.user_id = :user",
						soci::use(hashedPassword, "password"),
						soci::use(salt.get(), "salt"),
						soci::use(userId, "user");
				}
			}
			else if (dbPassword != MiscUtilities::hashPassword(password, salt.get())) {
				LoginPacket::loginError(player, LoginPacket::Errors::InvalidPassword);
				valid = false;
			}
			else if (row.get<bool>("online")) {
				LoginPacket::loginError(player, LoginPacket::Errors::AlreadyLoggedIn);
				valid = false;
			}
			else if (row.get<bool>("banned") && (!row.get<bool>("admin") || row.get<int32_t>("gm_level") == 0)) {
				int32_t time = TimeUtilities::timeToTick32(row.get<unix_time_t>("ban_expire"));
				LoginPacket::loginBan(player, row.get<int8_t>("ban_reason"), time);
				valid = false;
			}
		}
	}
	if (!valid) {
		int32_t threshold = LoginServer::getInstance().getInvalidLoginThreshold();
		if (threshold != 0 && player->addInvalidLogin() >= threshold) {
			 // Too many invalid logins
			player->getSession()->disconnect();
		}
	}
	else {
		LoginServer::getInstance().log(LogTypes::Login, username + " from IP " + player->getIp().toString());

		player->setUserId(userId);

		if (LoginServer::getInstance().getPinEnabled()) {
			opt_int32_t pin = row.get<opt_int32_t>("pin");
			if (!pin.is_initialized()) {
				player->setPin(-1);
			}
			else {
				player->setPin(pin.get());
			}

			player->setStatus(player->getPin() == -1 ? PlayerStatus::SetPin : PlayerStatus::AskPin);
		}
		else {
			player->setStatus(PlayerStatus::LoggedIn);
		}

		opt_int8_t gender = row.get<opt_int8_t>("gender");
		if (!gender.is_initialized()) {
			player->setStatus(PlayerStatus::SetGender);
		}
		else {
			player->setGender(gender.get());
		}

		opt_unix_time_t quietBan = row.get<opt_unix_time_t>("quiet_ban_expire");
		if (quietBan.is_initialized()) {
			time_t banTime = quietBan.get();
			if (time(nullptr) > banTime) {
				sql.once
					<< "UPDATE user_accounts u "
					<< "SET u.quiet_ban_expire = NULL, u.quiet_ban_reason = NULL "
					<< "WHERE u.user_id = :user",
					soci::use(userId, "user");
			}
			else {
				player->setQuietBanTime(TimeUtilities::timeToTick(banTime));
				player->setQuietBanReason(row.get<int8_t>("quiet_ban_reason"));
			}
		}

		player->setCreationTime(TimeUtilities::timeToTick(row.get<unix_time_t>("creation_date")));
		player->setCharDeletePassword(row.get<opt_int32_t>("char_delete_password"));
		player->setAdmin(row.get<bool>("admin"));

		LoginPacket::loginConnect(player, username);
	}
}

auto Login::setGender(Player *player, PacketReader &packet) -> void {
	if (player->getStatus() != PlayerStatus::SetGender) {
		// Hacking
		return;
	}
	if (packet.get<int8_t>() == 1) {
		// get<bool> candidate?
		int8_t gender = packet.get<int8_t>();
		if (gender != Gender::Male && gender != Gender::Female) {
			// Hacking
			return;
		}

		player->setStatus(PlayerStatus::NotLoggedIn);

		Database::getCharDb().once
			<< "UPDATE user_accounts u "
			<< "SET u.gender = :gender "
			<< "WHERE u.user_id = :user",
			soci::use(gender, "gender"),
			soci::use(player->getUserId(), "user");

		player->setGender(gender);

		if (LoginServer::getInstance().getPinEnabled()) {
			player->setStatus(PlayerStatus::SetPin);
		}
		else {
			player->setStatus(PlayerStatus::LoggedIn);
		}
		LoginPacket::genderDone(player, gender);
	}
}

auto Login::handleLogin(Player *player, PacketReader &packet) -> void {
	int32_t status = player->getStatus();
	if (status == PlayerStatus::SetPin) {
		LoginPacket::loginProcess(player, PlayerStatus::SetPin);
	}
	else if (status == PlayerStatus::AskPin) {
		LoginPacket::loginProcess(player, PlayerStatus::CheckPin);
		player->setStatus(PlayerStatus::CheckPin);
	}
	else if (status == PlayerStatus::CheckPin) {
		checkPin(player, packet);
	}
	else if (status == PlayerStatus::LoggedIn) {
		LoginPacket::loginProcess(player, PlayerStatus::LoggedIn);
		// The player successfully logged in, so let set the login column
		player->setOnline(true);
	}
}

auto Login::checkPin(Player *player, PacketReader &packet) -> void {
	if (!LoginServer::getInstance().getPinEnabled()) {
		// Hacking
		return;
	}
	int8_t act = packet.get<int8_t>();
	packet.skipBytes(5);

	if (act == 0x00) {
		player->setStatus(PlayerStatus::AskPin);
	}
	else if (act == 0x01) {
		int32_t pin = StringUtilities::lexical_cast<int32_t>(packet.getString());
		int32_t current = player->getPin();
		if (pin == current) {
			player->setStatus(PlayerStatus::LoggedIn);
			handleLogin(player, packet);
		}
		else {
			LoginPacket::loginProcess(player, LoginPacket::Errors::InvalidPin);
		}
	}
	else if (act == 0x02) {
		int32_t pin = StringUtilities::lexical_cast<int32_t>(packet.getString());
		if (pin == player->getPin()) {
			player->setStatus(PlayerStatus::SetPin);
			handleLogin(player, packet);
		}
		else {
			LoginPacket::loginProcess(player, LoginPacket::Errors::InvalidPin);
		}
	}
}

auto Login::registerPin(Player *player, PacketReader &packet) -> void {
	if (!LoginServer::getInstance().getPinEnabled() || player->getStatus() != PlayerStatus::SetPin) {
		// Hacking
		return;
	}
	if (packet.get<int8_t>() == 0x00) {
		if (player->getPin() != -1) {
			player->setStatus(PlayerStatus::AskPin);
		}
		return;
	}
	int32_t pin = StringUtilities::lexical_cast<int32_t>(packet.getString());
	player->setStatus(PlayerStatus::NotLoggedIn);
	Database::getCharDb().once
		<< "UPDATE user_accounts u "
		<< "SET u.pin = :pin "
		<< "WHERE u.user_id = :user",
		soci::use(pin, "pin"),
		soci::use(player->getUserId(), "user");

	LoginPacket::pinAssigned(player);
}