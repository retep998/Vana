/*
Copyright (C) 2008-2011 Vana Development Team

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
#pragma once

#include "ServerConnection.h"
#include "Types.h"

class PacketReader;

class LoginServerAcceptConnection : public AbstractServerAcceptConnection {
public:
	LoginServerAcceptConnection() : worldId(-1) { };
	~LoginServerAcceptConnection();
	void realHandleRequest(PacketReader &packet);
	void authenticated(int8_t type);

	void setWorldId(int8_t id) { worldId = id; }
	int8_t getWorldId() const { return worldId; }
private:
	int8_t worldId;
};

class LoginServerAcceptConnectionFactory : public AbstractConnectionFactory {
public:
	AbstractConnection * createConnection() {
		return new LoginServerAcceptConnection();
	}
};
