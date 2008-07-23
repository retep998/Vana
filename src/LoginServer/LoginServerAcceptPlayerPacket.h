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
#ifndef LOGINSERVERACCEPTPLAYERPACKET_H
#define LOGINSERVERACCEPTPLAYERPACKET_H

#include <string>

using std::string;

class LoginServerAcceptPlayer;

namespace LoginServerAcceptPlayerPacket {
	void connect(LoginServerAcceptPlayer *player, char worldid, short port, int maxchan, int exprate, int questexprate, int mesorate, int droprate);
	void connectChannel(LoginServerAcceptPlayer *player, char worldid, const string &ip, short port);
	void newPlayer(LoginServerAcceptPlayer *player, int channel, int charid);
};

#endif