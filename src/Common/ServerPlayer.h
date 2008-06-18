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
#include "PacketHandler.h"
#include "AuthenticationPacket.h"
#include "InterHeader.h"
#include "ReadPacket.h"
#include <iostream>
#include <string>

using std::string;

class AbstractServerConnectPlayer : public AbstractPlayer {
public:
	AbstractServerConnectPlayer() { is_server = true; }
	void sendAuth(const string &pass) {
		AuthenticationPacket::sendPassword(this, pass, getIP());
	}
	char getType() { return type; }
protected:
	char type;
};

class AbstractServerAcceptPlayer : public AbstractPlayer {
public:
	AbstractServerAcceptPlayer() { is_server = true; }
	bool processAuth(ReadPacket *packet, const string &pass) {
		if (packet->getShort() == INTER_PASSWORD) {
			if (packet->getString() == pass) {
				std::cout << "Server successfully authenticated." << std::endl;
				is_authenticated = true;
				short iplen = packet->getShort();
				if (iplen) {
					setIP(packet->getString(iplen)); // setIP in abstractPlayer
				}
				authenticated(packet->getByte());
			}
			else {
				disconnect();
				return false;
			}
		}
		else if (is_authenticated == false) {
			// Trying to do something while unauthenticated? DC!
			disconnect();
			return false;
		}
		packet->reset();
		return true;
	}
	virtual void authenticated(char type) = 0;
private:
	bool is_authenticated;
};

#endif
