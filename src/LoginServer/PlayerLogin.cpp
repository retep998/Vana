/*
Copyright (C) 2008 Vana Development Team

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
#include "Login.h"
#include "Worlds.h"
#include "MySQLM.h"
#include "ReadPacket.h"
#include "RecvHeader.h"

void PlayerLogin::realHandleRequest(ReadPacket *packet) {
	switch(packet->getShort()) {
		case RECV_LOGIN_INFO: Login::loginUser(this, packet); break;
		case RECV_CHANNEL_SELECT: Worlds::channelSelect(this, packet); break;
		case RECV_WORLD_SELECT: Worlds::selectWorld(this, packet); break;
		case RECV_LOGIN_PROCESS: Login::handleLogin(this, packet); break;
		case RECV_SHOW_WORLD:
		case RECV_SHOW_WORLD2: Worlds::showWorld(this); break;
		case RECV_GET_CHANNEL_SERVER_INFO: Characters::connectGame(this, packet); break;
		case RECV_CHECK_CHAR_NAME: Characters::checkCharacterName(this, packet); break;
		case RECV_CREATE_CHAR: Characters::createCharacter(this, packet); break;
		case RECV_DELETE_CHAR: Characters::deleteCharacter(this, packet); break;
		case RECV_SET_GENDER: Login::setGender(this, packet); break;
		case RECV_REGISTER_PIN: Login::registerPIN(this, packet); break;
	}
}

PlayerLogin::~PlayerLogin() {
	setOnline(false);
}

void PlayerLogin::setOnline(bool online) {
	mysqlpp::Query query = chardb.query();
	query << "UPDATE users SET online = " << mysqlpp::quote << online << " WHERE id = " << mysqlpp::quote << getUserid();
	query.exec();
}