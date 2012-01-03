/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "Login.h"
#include "Database.h"
#include "GameConstants.h"
#include "IpUtilities.h"
#include "LoginPacket.h"
#include "LoginServer.h"
#include "MiscUtilities.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerStatus.h"
#include "Randomizer.h"
#include "Session.h"
#include "StringUtilities.h"
#include "TimeUtilities.h"
#include "VanaConstants.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <iostream>

using TimeUtilities::timeToTick32;
using TimeUtilities::timeToTick;

void Login::loginUser(Player *player, PacketReader &packet) {
	string &username = packet.getString();
	string &password = packet.getString();
	string &ip = IpUtilities::ipToString(player->getIp());

	if (!MiscUtilities::inRangeInclusive<size_t>(username.size(), Characters::MinNameSize, Characters::MaxNameSize)) {
		// Hacking
		return;
	}
	if (!MiscUtilities::inRangeInclusive<size_t>(password.size(), Characters::MinPasswordSize, Characters::MaxPasswordSize)) {
		// Hacking
		return;
	}

	soci::session &sql = Database::getCharDb();
	soci::indicator ind = soci::i_null;
	soci::row row;
	sql.once << "SELECT u.*, CAST(u.ban_expire IS NOT NULL AND u.ban_expire > 0 AND u.ban_expire >= NOW() AS SIGNED) AS banned " <<
				"FROM user_accounts u " <<
				"WHERE u.username = :user",
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

		sql.once << "SELECT i.ip_ban_id FROM ip_bans i WHERE i.ip = :ip",
					soci::use(ip, "ip"),
					soci::into(ipBanned);

		if (sql.got_data() && ipBanned.is_initialized()) {
			//namespace pt = boost::posix_time;
			//int32_t time = timeToTick32(mktime(&pt::to_tm(pt::time_from_string("9000-01-01"))));
			//LoginPacket::loginBan(player, 0, time);
			valid = false;
		}
		else {
			userId = row.get<int32_t>("user_id");
			string &dbPassword = row.get<string>("password");
			string &salt = row.get<string>("salt");
			ind = row.get_indicator("salt");

			if (ind == soci::i_null) {
				// We have an unsalted password
				if (dbPassword != password) {
					LoginPacket::loginError(player, LoginPacket::Errors::InvalidPassword);
					valid = false;
				}
				else {
					// We have a valid password, so let's hash the password
					salt = Randomizer::Instance()->generateSalt(VanaConstants::SaltSize);
					string &hashedPassword = MiscUtilities::hashPassword(password, salt);
					sql.once << "UPDATE user_accounts u " <<
								"SET u.password = :password, u.salt = :salt " <<
								"WHERE u.user_id = :user",
								soci::use(hashedPassword, "password"),
								soci::use(salt, "salt"),
								soci::use(userId, "user");
				}
			}
			else if (dbPassword != MiscUtilities::hashPassword(password, salt)) {
				LoginPacket::loginError(player, LoginPacket::Errors::InvalidPassword);
				valid = false;
			}
			else if (row.get<bool>("online")) {
				LoginPacket::loginError(player, LoginPacket::Errors::AlreadyLoggedIn);
				valid = false;
			}
			/* SOCI
			// SOCI and MySQL do NOT play well with computed columns, consider revision of schema
			else if (row.get<bool>("banned")) {
				int32_t time = timeToTick32(row.get<unix_time_t>("ban_expire"));
				LoginPacket::loginBan(player, row.get<int8_t>("ban_reason"), time);
				valid = false;
			}
			*/
		}
	}
	if (!valid) {
		int32_t threshold = LoginServer::Instance()->getInvalidLoginThreshold();
		if (threshold != 0 && player->addInvalidLogin() >= threshold) {
			 // Too many invalid logins
			player->getSession()->disconnect();
		}
	}
	else {
		LoginServer::Instance()->log(LogTypes::Login, username + " from IP " + IpUtilities::ipToString(player->getIp()));

		player->setUserId(userId);

		if (LoginServer::Instance()->getPinEnabled()) {
			int32_t pin = row.get<int32_t>("pin");
			ind = row.get_indicator("pin");
			if (ind == soci::i_null) {
				player->setPin(-1);
			}
			else {
				player->setPin(pin);
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

		time_t quietBan = row.get<unix_time_t>("quiet_ban_expire");
		if (quietBan > 0) {
			if (time(nullptr) > quietBan) {
				sql.once << "UPDATE user_accounts u " <<
							"SET u.quiet_ban_expire = '0000-00-00 00:00:00', u.quiet_ban_reason = 0 " <<
							"WHERE u.user_id = :user", soci::use(userId, "user");
			}
			else {
				player->setQuietBanTime(timeToTick(quietBan));
				player->setQuietBanReason(row.get<int8_t>("quiet_ban_reason"));
			}
		}

		player->setCreationTime(timeToTick(row.get<unix_time_t>("creation_date")));
		player->setCharDeletePassword(row.get<int32_t>("char_delete_password"));
		player->setAdmin(row.get<bool>("admin"));

		LoginPacket::loginConnect(player, username);
	}
}

void Login::setGender(Player *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::SetGender) {
		// Hacking
		return;
	}
	if (packet.get<int8_t>() == 1) {
		// getBool candidate?
		player->setStatus(PlayerStatus::NotLoggedIn);
		int8_t gender = packet.get<int8_t>();
		Database::getCharDb().once << "UPDATE user_accounts u " <<
										"SET u.gender = :gender " <<
										"WHERE u.user_id = :user",
										soci::use(gender, "gender"),
										soci::use(player->getUserId(), "user");

		if (LoginServer::Instance()->getPinEnabled()) {
			player->setStatus(PlayerStatus::SetPin);
		}
		else {
			player->setStatus(PlayerStatus::LoggedIn);
		}
		LoginPacket::genderDone(player, gender);
	}
}

void Login::handleLogin(Player *player, PacketReader &packet) {
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

void Login::checkPin(Player *player, PacketReader &packet) {
	if (!LoginServer::Instance()->getPinEnabled()) {
		// Hacking
		return;
	}
	int8_t act = packet.get<int8_t>();
	packet.skipBytes(5);

	if (act == 0x00) {
		player->setStatus(PlayerStatus::AskPin);
	}
	else if (act == 0x01) {
		int32_t pin = boost::lexical_cast<int32_t>(packet.getString());
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
		int32_t pin = boost::lexical_cast<int32_t>(packet.getString());
		if (pin == player->getPin()) {
			player->setStatus(PlayerStatus::SetPin);
			handleLogin(player, packet);
		}
		else {
			LoginPacket::loginProcess(player, LoginPacket::Errors::InvalidPin);
		}
	}
}

void Login::registerPin(Player *player, PacketReader &packet) {
	if (!LoginServer::Instance()->getPinEnabled() || player->getStatus() != PlayerStatus::SetPin) {
		// Hacking
		return;
	}
	if (packet.get<int8_t>() == 0x00) {
		if (player->getPin() != -1) {
			player->setStatus(PlayerStatus::AskPin);
		}
		return;
	}
	int32_t pin = boost::lexical_cast<int32_t>(packet.getString());
	player->setStatus(PlayerStatus::NotLoggedIn);
	Database::getCharDb().once << "UPDATE user_accounts u " <<
									"SET u.pin = :pin" <<
									"WHERE u.user_id = :user",
									soci::use(pin, "pin"),
									soci::use(player->getUserId(), "user");

	LoginPacket::pinAssigned(player);
}