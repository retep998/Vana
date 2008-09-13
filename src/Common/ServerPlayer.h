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
#include "Types.h"
#include <string>

using std::string;

class ReadPacket;

class AbstractServerConnectPlayer : public AbstractPlayer {
public:
	AbstractServerConnectPlayer() { is_server = true; }
	void sendAuth(const string &pass);
	int8_t getType() { return type; }
protected:
	int8_t type;
};

class AbstractServerAcceptPlayer : public AbstractPlayer {
public:
	AbstractServerAcceptPlayer() { is_server = true; }
	bool processAuth(ReadPacket *packet, const string &pass);
	virtual void authenticated(int8_t type) = 0;
private:
	bool is_authenticated;
};

#endif
