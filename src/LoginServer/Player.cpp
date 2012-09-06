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
#include "Player.h"
#include "Characters.h"
#include "CmsgHeader.h"
#include "Database.h"
#include "IpUtilities.h"
#include "Login.h"
#include "LoginPacket.h"
#include "LoginServer.h"
#include "PacketReader.h"
#include "Worlds.h"
#include <iostream>
#include <stdexcept>

Player::Player() :
	m_status(PlayerStatus::NotLoggedIn),
	m_invalidLogins(0),
	m_quietBanTime(0),
	m_quietBanReason(0)
{
}

void Player::handleRequest(PacketReader &packet) {
	try {
		switch (packet.getHeader()) {
			case CMSG_AUTHENTICATION: Login::loginUser(this, packet); break;
			case CMSG_PLAYER_LIST: Worlds::Instance()->channelSelect(this, packet); break;
			case CMSG_WORLD_STATUS: Worlds::Instance()->selectWorld(this, packet); break;
			case CMSG_PIN: Login::handleLogin(this, packet); break;
			case CMSG_WORLD_LIST:
			case CMSG_WORLD_LIST_REFRESH: Worlds::Instance()->showWorld(this); break;
			case CMSG_CHANNEL_CONNECT: Characters::connectGame(this, packet); break;
			case CMSG_CLIENT_ERROR: LoginServer::Instance()->log(LogTypes::ClientError, packet.getString()); break;
			case CMSG_CLIENT_STARTED: LoginServer::Instance()->log(LogTypes::Info, "Client connected and started from " + IpUtilities::ipToString(this->getIp())); break;
			case CMSG_PLAYER_GLOBAL_LIST: Characters::showAllCharacters(this); break;
			case CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT: Characters::connectGameWorld(this, packet); break;
			case CMSG_PLAYER_NAME_CHECK: Characters::checkCharacterName(this, packet); break;
			case CMSG_PLAYER_CREATE: Characters::createCharacter(this, packet); break;
			case CMSG_PLAYER_DELETE: Characters::deleteCharacter(this, packet); break;
			case CMSG_ACCOUNT_GENDER: Login::setGender(this, packet); break;
			case CMSG_REGISTER_PIN: Login::registerPin(this, packet); break;
			case CMSG_LOGIN_RETURN: LoginPacket::relogResponse(this); break;
		}
	}
	catch (std::range_error) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly

		packet.reset();
		std::ostringstream x;
		x << "User ID: " << getUserId() << "; Packet: " << packet;
		LoginServer::Instance()->log(LogTypes::MalformedPacket, x.str());
		getSession()->disconnect();
	}
}

Player::~Player() {
	setOnline(false);
}

void Player::setOnline(bool online) {
	Database::getCharDb() << "UPDATE user_accounts u " <<
								"SET " <<
								"	u.online = :online," <<
								"	u.last_login = NOW() " <<
								"WHERE u.user_id = :id",
								soci::use((online ? 1 : 0), "online"),
								soci::use(getUserId(), "id");
}