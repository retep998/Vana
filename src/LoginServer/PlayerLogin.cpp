/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "PlayerLogin.h"
#include "Characters.h"
#include "Database.h"
#include "Login.h"
#include "LoginPacket.h"
#include "PacketReader.h"
#include "RecvHeader.h"
#include "Worlds.h"
#include <stdexcept>

void PlayerLogin::realHandleRequest(PacketReader &packet) {
	try {
		switch (packet.get<int16_t>()) {
			case CMSG_AUTHENTICATION: Login::loginUser(this, packet); break;
			case CMSG_PLAYER_LIST: Worlds::channelSelect(this, packet); break;
			case CMSG_WORLD_STATUS: Worlds::selectWorld(this, packet); break;
			case CMSG_PIN: Login::handleLogin(this, packet); break;
			case CMSG_WORLD_LIST:
			case CMSG_WORLD_LIST_REFRESH: Worlds::showWorld(this); break;
			case CMSG_CHANNEL_CONNECT: Characters::connectGame(this, packet); break;
			case CMSG_PLAYER_GLOBAL_LIST: Characters::showAllCharacters(this); break;
			case CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT: Characters::connectGameWorld(this, packet); break;
			case CMSG_PLAYER_NAME_CHECK: Characters::checkCharacterName(this, packet); break;
			case CMSG_PLAYER_CREATE: Characters::createCharacter(this, packet); break;
			case CMSG_PLAYER_DELETE: Characters::deleteCharacter(this, packet); break;
			case CMSG_ACCOUNT_GENDER: Login::setGender(this, packet); break;
			case CMSG_REGISTER_PIN: Login::registerPin(this, packet); break;
			case CMSG_LOGIN_RETURN: LoginPacket::relogResponse(this);
		}
	}
	catch (std::range_error) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly
		getSession()->disconnect();
	}
}

PlayerLogin::~PlayerLogin() {
	setOnline(false);
}

void PlayerLogin::setOnline(bool online) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE users SET online = " << online
			<< ", last_login = NOW()"
			<< " WHERE id = " << this->userid;
	query.exec();
}
