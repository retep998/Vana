/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "UserConnection.hpp"
#include "Characters.hpp"
#include "CmsgHeader.hpp"
#include "Database.hpp"
#include "Login.hpp"
#include "LoginPacket.hpp"
#include "LoginServer.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Worlds.hpp"
#include <iostream>
#include <stdexcept>

UserConnection::~UserConnection() {
	setOnline(false);
}

auto UserConnection::handleRequest(PacketReader &reader) -> void {
	try {
		switch (reader.get<header_t>()) {
			case CMSG_AUTHENTICATION: Login::loginUser(this, reader); break;
			case CMSG_PLAYER_LIST: LoginServer::getInstance().getWorlds().channelSelect(this, reader); break;
			case CMSG_WORLD_STATUS: LoginServer::getInstance().getWorlds().selectWorld(this, reader); break;
			case CMSG_PIN: Login::handleLogin(this, reader); break;
			case CMSG_WORLD_LIST:
			case CMSG_WORLD_LIST_REFRESH: LoginServer::getInstance().getWorlds().showWorld(this); break;
			case CMSG_CHANNEL_CONNECT: Characters::connectGame(this, reader); break;
			case CMSG_CLIENT_ERROR: LoginServer::getInstance().log(LogType::ClientError, reader.get<string_t>()); break;
			case CMSG_CLIENT_STARTED: LoginServer::getInstance().log(LogType::Info, [&](out_stream_t &log) { log << "Client connected and started from " << getIp(); }); break;
			case CMSG_PLAYER_GLOBAL_LIST: Characters::showAllCharacters(this); break;
			case CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT: Characters::connectGameWorldFromViewAllCharacters(this, reader); break;
			case CMSG_PLAYER_NAME_CHECK: Characters::checkCharacterName(this, reader); break;
			case CMSG_PLAYER_CREATE: Characters::createCharacter(this, reader); break;
			case CMSG_PLAYER_DELETE: Characters::deleteCharacter(this, reader); break;
			case CMSG_ACCOUNT_GENDER: Login::setGender(this, reader); break;
			case CMSG_REGISTER_PIN: Login::registerPin(this, reader); break;
			case CMSG_LOGIN_RETURN: this->send(LoginPacket::relogResponse()); break;
		}
	}
	catch (const PacketContentException &e) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly

		reader.reset();
		LoginServer::getInstance().log(LogType::MalformedPacket, [&](out_stream_t &log) {
			log << "User ID: " << getUserId()
				<< "; Packet: " << reader
				<< "; Error: " << e.what();
		});
		disconnect();
	}
}

auto UserConnection::setOnline(bool online) -> void {
	Database::getCharDb()
		<< "UPDATE " << Database::makeCharTable("user_accounts") << " u "
		<< "SET "
		<< "	u.online = :online,"
		<< "	u.last_login = NOW() "
		<< "WHERE u.user_id = :id",
		soci::use((online ? 1 : 0), "online"),
		soci::use(m_userId, "id");
}