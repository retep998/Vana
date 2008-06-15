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
#ifndef LOGINSERVERCONNECTPLAYER_H
#define LOGINSERVERCONNECTPLAYER_H

#include "ServerPlayer.h"

class ReadPacket;

class LoginServerConnectPlayer : public AbstractServerConnectPlayer {
public:
	LoginServerConnectPlayer();
	void realHandleRequest(ReadPacket *packet);
};

class LoginServerConnectPlayerFactory : public AbstractPlayerFactory {
public:
	AbstractPlayer * createPlayer() {
		return new LoginServerConnectPlayer();
	}
};

#endif
