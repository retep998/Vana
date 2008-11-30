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
#ifndef CHANNELS_H
#define CHANNELS_H

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <string>

using std::string;
using std::tr1::shared_ptr;
using std::tr1::unordered_map;

class WorldServerAcceptPlayer;
class PacketCreator;

struct Channel {
	Channel() : players(0) { }

	WorldServerAcceptPlayer *player;
	uint16_t id;
	string ip;
	uint16_t port;
	int32_t players;
};

class Channels {
public:
	static Channels * Instance() {
		if (singleton == 0)
			singleton = new Channels;
		return singleton;
	}
	void registerChannel(WorldServerAcceptPlayer *player, uint16_t channel, const string &ip, uint16_t port);
	void removeChannel(uint16_t channel);
	Channel * getChannel(uint16_t num);
	void sendToAll(PacketCreator &packet);
	uint16_t size();
private:
	Channels() {};
	Channels(const Channels&);
	Channels& operator=(const Channels&);
	static Channels *singleton;

	unordered_map<uint16_t, shared_ptr<Channel> > channels;
};

#endif
