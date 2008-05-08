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
#ifndef ABSTRACTPLAYER_H
#define ABSTRACTPLAYER_H

#include "PacketHandler.h"
#include <time.h>

class AbstractPlayer {
public:
	virtual void setPacketHandler (PacketHandler* packetHandler) {
		this->packetHandler = packetHandler;
	}
	virtual void realHandleRequest (unsigned char* buf, int len) = 0;
	void handleRequest (unsigned char* buf, int len) { updateLastAction(); realHandleRequest (buf, len); }
	virtual ~AbstractPlayer(){}
	void disconnect() { packetHandler->disconnect(); }
	void updateLastAction () { lastAction = time(0); }
protected:
	PacketHandler* packetHandler;
	time_t lastAction;
};

class AbstractPlayerFactory {
public:
	virtual AbstractPlayer* createPlayer () = 0;
};
#endif