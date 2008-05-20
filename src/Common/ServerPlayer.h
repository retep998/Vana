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
#include "InterHeader.h"
#include <iostream>

class AbstractServerConnectPlayer : public AbstractPlayer {
public:
	void sendAuth(const char *pass) {
		AuthenticationPacket::sendPassword(this, pass, getIP());
	}
	char getType() { return type; }
protected:
	char type;
};

class AbstractServerAcceptPlayer : public AbstractPlayer {
public:
	void processAuth(unsigned char *buf, char *password) {
		short header = buf[0] + buf[1]*0x100;
		if (header == INTER_PASSWORD) {
			char pass[255];
			short passlen = BufferUtilities::getShort(buf+2);
			BufferUtilities::getString(buf+4, passlen, pass);
			if(strcmp(pass, password) == 0) {
				std::cout << "Server successfully authenticated." << std::endl;
				is_authenticated = true;
				short iplen = BufferUtilities::getShort(buf+5+passlen);
				if (iplen) {
					char ip[15];
					BufferUtilities::getString(buf+7+passlen, iplen, ip);
					setIP(ip); // setIP in abstractPlayer
				}
				authenticated(buf[2+2+passlen]);
			}
			else {
				disconnect();
			}
		}
		else if (is_authenticated == false) {
			// Trying to do something while unauthenticated? DC!
			disconnect();
		}
	}
	virtual void authenticated(char type) = 0;
private:
	bool is_authenticated;
};

#endif