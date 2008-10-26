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
#ifndef LOGINSERVERACCEPTPACKET_H
#define LOGINSERVERACCEPTPACKET_H

#include "Types.h"
#include <string>

using std::string;

class LoginServerAcceptPlayer;
struct World;

namespace LoginServerAcceptPacket {
	void connect(LoginServerAcceptPlayer *player, World *world);
	void noMoreWorld(LoginServerAcceptPlayer *player);
	void connectChannel(LoginServerAcceptPlayer *player, int8_t worldid, const string &ip, int16_t port);
	void newPlayer(LoginServerAcceptPlayer *player, uint16_t channel, int32_t charid);
};

#endif
