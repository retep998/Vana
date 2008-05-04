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

void PlayerLogin::handleRequest(unsigned char* buf, int len){
	short header = buf[0] + buf[1]*0x100;
	switch(header){
		case 0x1b: Login::loginUser(this, buf+2); break;
		case 0x19: Worlds::channelSelect(this, buf+2); break;
		case 0x13: Worlds::selectWorld(this, buf+2); break;
		case 0x03: Login::handleLogin(this, buf+2); break;
		case 0x18: Worlds::showWorld(this); break;
		case 0x02: Worlds::showWorld(this); break;
		case 0x16: Characters::connectGame(this, buf+2); break;
		case 0x09: Characters::checkCharacterName(this, buf+2); break;
		case 0x0e: Characters::createCharacter(this, buf+2); break;
		case 0x0f: Characters::deleteCharacter(this, buf+2); break;
		case 0x1A: Login::loginBack(this); break;
		//case 0x07: Login::setGender(this, buf+2); break;
		//case 0x09: Login::registerPIN(this, buf+2); break;
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