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
#include "MapleVersion.h"
#include "PacketReader.h"
#include "Randomizer.h"
#include "Worlds.h"
#include <iostream>
#include <stdexcept>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

Player::Player() :
	m_status(PlayerStatus::NotLoggedIn),
	m_invalidLogins(0),
	m_quietBanTime(0),
	m_quietBanReason(0)
{
	// Random long lol
	boost::random::mt19937 gen;
	gen.seed(time(0));
	boost::random::uniform_int_distribution<long> dist;
	m_connectKey = dist(gen);
}

void Player::handleRequest(PacketReader &packet) {
	std::ostringstream x;
	x << "INBOUND: " << packet;
	LoginServer::Instance()->log(LogTypes::Info, x.str());
	packet.reset();

	try {
		switch (packet.getHeader()) {
			case CMSG_AUTHENTICATION: Login::loginUser(this, packet); break;
			case CMSG_PLAYER_LIST: Worlds::Instance()->channelSelect(this, packet); break;
			case CMSG_WORLD_STATUS: Worlds::Instance()->selectWorld(this, packet); break;
			case CMSG_PIN: Login::handleLogin(this, packet); break;
			case CMSG_WORLD_LIST:
			case CMSG_WORLD_LIST_REFRESH: Worlds::Instance()->showWorld(this); break;
			case CMSG_CHANNEL_CONNECT: Characters::connectGame(this, packet, false, false); break;
			case CMSG_CHANNEL_CONNECT_PIC: Characters::connectGame(this, packet, true, false); break;
			case CMSG_CLIENT_ERROR: LoginServer::Instance()->log(LogTypes::ClientError, packet.getString()); break;
			case CMSG_CLIENT_STARTED: LoginServer::Instance()->log(LogTypes::Info, "Client connected and started from " + IpUtilities::ipToString(this->getIp())); break;
			case CMSG_PLAYER_GLOBAL_LIST: Characters::showAllCharacters(this); break;
			case CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT: Characters::connectGame(this, packet, false, true); break;
			case CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT_PIC: Characters::connectGame(this, packet, true, true); break;
			case CMSG_PLAYER_NAME_CHECK: Characters::checkCharacterName(this, packet); break;
			case CMSG_PLAYER_CREATE: 
			case CMSG_PLAYER_CREATE_SPECIAL: Characters::createCharacter(this, packet); break;
			case CMSG_PLAYER_DELETE: Characters::deleteCharacter(this, packet); break;
			case CMSG_ACCOUNT_GENDER: Login::setGender(this, packet); break;
			case CMSG_REGISTER_PIN: Login::registerPin(this, packet); break;
			case CMSG_LOGIN_RETURN: LoginPacket::relogResponse(this); break;
			case CMSG_REQUEST_SPECIAL_CHAR_CREATION: LoginPacket::specialCharacterCreation(this, true); break; // TODO: Find out when you are eligable for the new jobs
			case CMSG_CLIENT_VERSION: handleClientVersion(packet); break;
			default: {
					packet.reset();
					std::ostringstream x;
					x << "UNK PACKET: " << packet;
					LoginServer::Instance()->log(LogTypes::Info, x.str());
					break;
				}
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

void Player::handleClientVersion(PacketReader &packet) {
	uint8_t locale = packet.get<uint8_t>();
	uint16_t version = packet.get<uint16_t>();
	uint16_t subversion = packet.get<uint16_t>();
	if (locale != MapleVersion::Locale || version != MapleVersion::Version) {
		LoginServer::Instance()->log(LogTypes::Error, "Client disconnected: incorrect client version!");
		getSession()->disconnect();
	}
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