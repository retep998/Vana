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

#include "Types.h"
#include <map>
#include <string>
#include <unordered_map>

using std::map;
using std::string;
using std::tr1::shared_ptr;
using std::tr1::unordered_map;

class PlayerLogin;
class LoginServerAcceptPlayer;
class PacketReader;

struct Channel {
	Channel() : population(0) { }
	string ip;
	int16_t port;
	int32_t population;
};

struct World {
	string name;
	unordered_map<int32_t, shared_ptr<Channel>> channels;
	size_t maxChannels;
	int8_t id;
	int8_t ribbon;
	bool connected;
	string ip;
	int16_t port;
	LoginServerAcceptPlayer *player;
	// Rates
	int32_t exprate;
	int32_t questexprate;
	int32_t mesorate;
	int32_t droprate;
	// Multi-level limit
	uint8_t maxMultiLevel;
	// Max stats
	int16_t maxStats;
	string eventMsg;
	string scrollingHeader;
};

namespace Worlds {
	void channelSelect(PlayerLogin *player, PacketReader &packet);
	void selectWorld(PlayerLogin *player, PacketReader &packet);
	void showWorld(PlayerLogin *player);
	int8_t connectWorldServer(LoginServerAcceptPlayer *player); //Inter-server
	int8_t connectChannelServer(LoginServerAcceptPlayer *player); //Inter-server
	extern map<uint8_t, World *> worlds;
};

#endif
