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
#ifndef WORLDS_H
#define WORLDS_H

#include <hash_map>
#include <string>

using std::string;
using stdext::hash_map;

class PlayerLogin;
class LoginServerAcceptPlayer;
class ReadPacket;

struct Channel {
	string ip;
	short port;
};

struct World {
	string name;
	hash_map <int, Channel *> channels;
	int maxChannels;
	char id;
	char ribbon;
	bool connected;
	string ip;
	short port;
	LoginServerAcceptPlayer *player;
	// Rates
	int exprate;
	int questexprate;
	int mesorate;
	int droprate;
};

namespace Worlds {
	void channelSelect(PlayerLogin* player, ReadPacket *packet);
	void selectWorld(PlayerLogin* player, ReadPacket *packet);
	void showWorld(PlayerLogin* player);
	char connectWorldServer(LoginServerAcceptPlayer *player); //Inter-server
	char connectChannelServer(LoginServerAcceptPlayer *player); //Inter-server
	extern hash_map <int, World *> worlds;
};

#endif