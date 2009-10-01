/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "IpUtilities.h"
#include "LoginPacket.h"
#include "LoginServer.h"
#include "MapleSession.h"
#include "MiscUtilities.h"
#include "PacketReader.h"
#include "PlayerLogin.h"
#include "PlayerStatus.h"
#include "Randomizer.h"
#include "StringUtilities.h"
#include "TimeUtilities.h"
#include <iostream>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

void Login::loginUser(PlayerLogin *player, PacketReader &packet) {
	string username = packet.getString();
	string password = packet.getString();
	string ip = IpUtilities::ipToString(player->getIp());

	if (username.size() > 15 || password.size() > 15) {
		return;
	}

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT id, password, salt, online, pin, gender, char_delete_password, creation_date, quiet_ban_reason, quiet_ban_expire, ban_reason, ban_expire, (ban_expire > NOW()) as banned, admin FROM users WHERE username = " << mysqlpp::quote << username << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();
	query << "SELECT id FROM ipbans WHERE ip = " << mysqlpp::quote << ip << " LIMIT 1";	
	mysqlpp::StoreQueryResult resIp = query.store();

	bool valid = true;
	if (res.empty()) {
		LoginPacket::loginError(player, 0x05); //Invalid username
		valid = false;
	}
	else if (!resIp.empty()) {
		int32_t time = TimeUtilities::tickToTick32(TimeUtilities::timeToTick((time_t) mysqlpp::DateTime("9000-00-00 00:00:00")));
		LoginPacket::loginBan(player, 0, time); // Blocked from connection
		valid = false;
	}
	else if (res[0]["salt"].is_null()) {
		// We have an unsalted password here
		if (res[0]["password"] != password) {
			LoginPacket::loginError(player, 0x04); // Invalid password
			valid = false;
		}
		// We have a valid password here, so lets hash the password
		string salt = Randomizer::Instance()->generateSalt(5);
		string hashed_pass = MiscUtilities::hashPassword(password, salt);
		query << "UPDATE users SET password = " << mysqlpp::quote << hashed_pass << ", salt = " << mysqlpp::quote << salt << " WHERE id = " << res[0]["id"];
		query.exec();
	}
	else if (res[0]["password"] != MiscUtilities::hashPassword(password, string(res[0]["salt"].data()))) {
		LoginPacket::loginError(player, 0x04); // Invalid password
		valid = false;
	}
	else if (atoi(res[0]["online"]) != 0) {
		LoginPacket::loginError(player, 0x07); // Already logged in
		valid = false;
	}
	else if (atoi(res[0]["banned"]) == 1) {
		int32_t time = TimeUtilities::tickToTick32(TimeUtilities::timeToTick((time_t) mysqlpp::DateTime(res[0]["ban_expire"])));
		LoginPacket::loginBan(player, (uint8_t) res[0]["ban_reason"], time);
		valid = false;
	}
	if (!valid) {
		int32_t threshold = LoginServer::Instance()->getInvalidLoginThreshold();
		if (threshold != 0 && player->addInvalidLogin() >= threshold) {
			player->getSession()->disconnect(); // Too many invalid logins
		}
	}
	else {
		std::cout << username << " logged in." << std::endl;
		player->setUserId(res[0]["id"]);
		if (LoginServer::Instance()->getPinEnabled()) {
			if (res[0]["pin"].is_null())
				player->setPin(-1);
			else
				player->setPin(res[0]["pin"]);
			int32_t pin = player->getPin();
			if (pin == -1)
				player->setStatus(PlayerStatus::SetPin); // New PIN
			else
				player->setStatus(PlayerStatus::AskPin); // Ask for PIN
		}
		else
			player->setStatus(PlayerStatus::LoggedIn);
		if (res[0]["gender"].is_null())
			player->setStatus(PlayerStatus::SetGender);
		else
			player->setGender((uint8_t) res[0]["gender"]);

		time_t qban = (time_t) mysqlpp::DateTime(res[0]["quiet_ban_expire"]);
		if (qban > 0) {
			if (time(0) > qban) {
				query << "UPDATE users SET quiet_ban_expire = '0000-00-00 00:00:00', quiet_ban_reason = 0 WHERE id = " << player->getUserId();
				query.exec();
			}
			else {
				player->setQuietBanTime(TimeUtilities::timeToTick(qban));
				player->setQuietBanReason(atoi(res[0]["quiet_ban_reason"]));
			}
		}

		player->setCreationTime(TimeUtilities::timeToTick((time_t) mysqlpp::DateTime(res[0]["creation_date"])));
		player->setCharDeletePassword(res[0]["char_delete_password"]);
		player->setAdmin(StringUtilities::atob(res[0]["admin"]));

		LoginPacket::loginConnect(player, username);
	}
}

void Login::setGender(PlayerLogin *player, PacketReader &packet) {
	if (player->getStatus() != PlayerStatus::SetGender) {
		//hacking
		return;
	}
	if (packet.get<int8_t>() == 1) {
		player->setStatus(PlayerStatus::NotLoggedIn);
		int8_t gender = packet.get<int8_t>();
		mysqlpp::Query query = Database::getCharDB().query();
		query << "UPDATE users SET gender = " << (int32_t) gender << " WHERE id = " << player->getUserId();
		query.exec();
		if (LoginServer::Instance()->getPinEnabled())
			player->setStatus(PlayerStatus::SetPin); // Set pin
		else
			player->setStatus(PlayerStatus::LoggedIn);
		LoginPacket::genderDone(player, gender);
	}
}

void Login::handleLogin(PlayerLogin *player, PacketReader &packet) {
	int32_t status = player->getStatus();
	if (status == PlayerStatus::SetPin)
		LoginPacket::loginProcess(player, 0x01);
	else if (status == PlayerStatus::AskPin) {
		LoginPacket::loginProcess(player, 0x04);
		player->setStatus(PlayerStatus::CheckPin);
	}
	else if (status == PlayerStatus::CheckPin)
		checkPin(player, packet);
	else if (status == PlayerStatus::LoggedIn) {
		LoginPacket::loginProcess(player, 0x00);
		// The player successfully logged in, so let set the login column
		player->setOnline(true);
	}
}
void Login::checkPin(PlayerLogin *player, PacketReader &packet) {
	if (!LoginServer::Instance()->getPinEnabled()) {
		//hacking
		return;
	}
	int8_t act = packet.get<int8_t>();
	packet.skipBytes(5);
	if (act == 0x00) {
		player->setStatus(PlayerStatus::AskPin);
	}
	else if (act == 0x01) {
		int32_t pin = boost::lexical_cast<int32_t>(packet.getString());
		int32_t curpin = player->getPin();
		if (pin == curpin) {
			player->setStatus(PlayerStatus::LoggedIn);
			handleLogin(player, packet);
		}
		else
			LoginPacket::loginProcess(player, 0x02);
	}
	else if (act == 0x02) {
		int32_t pin = boost::lexical_cast<int32_t>(packet.getString());
		int32_t curpin = player->getPin();
		if (pin == curpin) {
			player->setStatus(PlayerStatus::SetPin);
			handleLogin(player, packet);
		}
		else
			LoginPacket::loginProcess(player, 0x02);
	}
}

void Login::registerPin(PlayerLogin *player, PacketReader &packet) {
	if (!LoginServer::Instance()->getPinEnabled() || player->getStatus() != PlayerStatus::SetPin) {
		//hacking
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
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE users SET pin = " << pin << " WHERE id = " << player->getUserId();
	query.exec();
	LoginPacket::pinAssigned(player);
}
