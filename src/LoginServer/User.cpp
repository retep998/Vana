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
#include "User.hpp"
#include "Common/Database.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/PacketReader.hpp"
#include "LoginServer/Characters.hpp"
#include "LoginServer/CmsgHeader.hpp"
#include "LoginServer/Login.hpp"
#include "LoginServer/LoginPacket.hpp"
#include "LoginServer/LoginServer.hpp"
#include "LoginServer/Worlds.hpp"
#include <iostream>
#include <stdexcept>

namespace Vana {
namespace LoginServer {

auto User::handle(PacketReader &reader) -> Result {
	try {
		switch (reader.get<header_t>()) {
			case CMSG_ACCOUNT_GENDER: Login::setGender(shared_from_this(), reader); break;
			case CMSG_AUTHENTICATION: Login::loginUser(shared_from_this(), reader); break;
			case CMSG_CHANNEL_CONNECT: Characters::connectGame(shared_from_this(), reader); break;
			case CMSG_CLIENT_ERROR: LoginServer::getInstance().log(LogType::ClientError, reader.get<string_t>()); break;
			case CMSG_CLIENT_STARTED: LoginServer::getInstance().log(LogType::Info, [&](out_stream_t &log) { log << "Client connected and started from " << getIp().get(Ip{0}); }); break;
			case CMSG_LOGIN_RETURN: send(Packets::relogResponse()); break;
			case CMSG_PIN: Login::handleLogin(shared_from_this(), reader); break;
			case CMSG_PLAYER_CREATE: Characters::createCharacter(shared_from_this(), reader); break;
			case CMSG_PLAYER_DELETE: Characters::deleteCharacter(shared_from_this(), reader); break;
			case CMSG_PLAYER_GLOBAL_LIST: Characters::showAllCharacters(shared_from_this()); break;
			case CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT: Characters::connectGameWorldFromViewAllCharacters(shared_from_this(), reader); break;
			case CMSG_PLAYER_LIST: LoginServer::getInstance().getWorlds().channelSelect(shared_from_this(), reader); break;
			case CMSG_PLAYER_NAME_CHECK: Characters::checkCharacterName(shared_from_this(), reader); break;
			case CMSG_REGISTER_PIN: Login::registerPin(shared_from_this(), reader); break;
			case CMSG_WORLD_LIST:
			case CMSG_WORLD_LIST_REFRESH: LoginServer::getInstance().getWorlds().showWorld(shared_from_this()); break;
			case CMSG_WORLD_STATUS: LoginServer::getInstance().getWorlds().selectWorld(shared_from_this(), reader); break;
		}
	}
	catch (const PacketContentException &e) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly

		reader.reset();
		LoginServer::getInstance().log(LogType::MalformedPacket, [&](out_stream_t &log) {
			log << "User ID: " << getAccountId()
				<< "; Packet: " << reader
				<< "; Error: " << e.what();
		});
		return Result::Failure;
	}

	return Result::Successful;
}

auto User::onDisconnect() -> void {
	setOnline(false);
	LoginServer::getInstance().finalizeUser(shared_from_this());
}

auto User::setOnline(bool online) -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	sql.once
		<< "UPDATE " << db.makeTable("accounts") << " u "
		<< "SET "
		<< "	u.online = :online,"
		<< "	u.last_login = NOW() "
		<< "WHERE u.account_id = :id",
		soci::use((online ? 1 : 0), "online"),
		soci::use(m_accountId, "id");
}

}
}