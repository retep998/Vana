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
#include "LoginServerAcceptPlayer.h"
#include "LoginServerAcceptHandler.h"
#include "LoginServer.h"
#include "InterHeader.h"
#include "Worlds.h"

void LoginServerAcceptPlayer::realHandleRequest(unsigned char *buf, int len) {
	if(!processAuth(buf, (char *) LoginServer::Instance()->getInterPassword())) return;
	short header = buf[0] + buf[1]*0x100;
	switch(header) {
		case INTER_REGISTER_CHANNEL: LoginServerAcceptHandler::registerChannel(this, buf+2); break;
	}
}

void LoginServerAcceptPlayer::authenticated(char type) {
	if (type == INTER_WORLD_SERVER)
		Worlds::connectWorldServer(this);
	else if (type == INTER_CHANNEL_SERVER)
		Worlds::connectChannelServer(this);
	else
		disconnect();
}