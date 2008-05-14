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
#include "LoginServerConnectPlayer.h"
#include "LoginServerConnectHandler.h"
#include "InterHeader.h"

LoginServerConnectPlayer::LoginServerConnectPlayer() {
	type = INTER_WORLD_SERVER;
}

void LoginServerConnectPlayer::realHandleRequest(unsigned char *buf, int len) {
	short header = buf[0] + buf[1]*0x100;
	switch(header) {
		case INTER_WORLD_CONNECT: LoginServerConnectHandler::connect(this, buf+2); break;
		case INTER_NEW_PLAYER: LoginServerConnectHandler::newPlayer(buf+2); break;
	}
}
