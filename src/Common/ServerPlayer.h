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
#ifndef SERVERPLAYER_H
#define SERVERPLAYER_H

#include "AbstractPlayer.h"
#include "Connection/PacketHandler.h"
#include "AuthenticationPacket.h"
#include "BufferUtilities.h"
#include <iostream>

class AbstractServerConnectPlayer : public AbstractPlayer {
public:
	void sendAuth(char *pass) {
		AuthenticationPacket::sendPassword(this, pass);
	}
};

class AbstractServerAcceptPlayer : public AbstractPlayer {
public:
	void processAuth(unsigned char *buf, char *password) {
		char pass[255];
		BufferUtilities::getString(buf+4, BufferUtilities::getShort(buf+2), pass);
		if(strcmp(pass, password) == 0) {
			std::cout << "World Server successfully authenticated." << std::endl;
			authenticated = true;
		}
	}
private:
	bool authenticated;
};

#endif