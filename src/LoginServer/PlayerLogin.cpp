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
#include "RecvHeader.h"

void PlayerLogin::realHandleRequest(unsigned char* buf, int len){
	short header = buf[0] + buf[1]*0x100;
	switch(header){
		case RECV_LOGIN_INFO: Login::loginUser(this, buf+2); break;
		case RECV_CHANNEL_SELECT: Worlds::channelSelect(this, buf+2); break;
		case RECV_WORLD_SELECT: Worlds::selectWorld(this, buf+2); break;
		case RECV_LOGIN_PROCESS: Login::handleLogin(this, buf+2); break;
		case RECV_SHOW_WORLD:
		case RECV_SHOW_WORLD2: Worlds::showWorld(this); break;
		case RECV_GET_CHANNEL_SERVER_INFO: Characters::connectGame(this, buf+2); break;
		case RECV_CHECK_CHAR_NAME: Characters::checkCharacterName(this, buf+2); break;
		case RECV_CREATE_CHAR: Characters::createCharacter(this, buf+2); break;
		case RECV_DELETE_CHAR: Characters::deleteCharacter(this, buf+2); break;
		case RECV_RETURN_TO_LOGIN: Login::loginBack(this); break;
		case RECV_REGISTER_PIN: Login::registerPIN(this, buf+2); break;
	}
}

PlayerLogin::~PlayerLogin() {
	setOnline(false);
}

void PlayerLogin::setOnline(bool online) {
	mysqlpp::Query query = db.query();
	query << "UPDATE users SET online = " << mysqlpp::quote << online << " WHERE id = " << mysqlpp::quote << getUserid();
	query.exec();
}