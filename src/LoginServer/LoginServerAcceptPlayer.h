/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef LOGINSERVERACCEPTPLAYER_H
#define LOGINSERVERACCEPTPLAYER_H

#include "ServerPlayer.h"
#include "Types.h"

class PacketReader;

class LoginServerAcceptPlayer : public AbstractServerAcceptPlayer {
public:
	LoginServerAcceptPlayer() : worldId(-1) { };
	~LoginServerAcceptPlayer();
	void realHandleRequest(PacketReader &packet);
	void authenticated(int8_t type);

	void setWorldId(int8_t id) { worldId = id; }
	int8_t getWorldId() const { return worldId; }
private:
	int8_t worldId;
};

class LoginServerAcceptPlayerFactory : public AbstractPlayerFactory {
public:
	AbstractPlayer * createPlayer() {
		return new LoginServerAcceptPlayer();
	}
};

#endif
