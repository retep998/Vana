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

#include "Types.h"
#include <string>
#include <vector>
#include <boost/tr1/memory.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>

using std::string;
using std::vector;
using std::tr1::shared_ptr;
using std::tr1::unordered_map;

class WorldServerAcceptConnection;
class PacketCreator;

struct Channel : boost::noncopyable {
	Channel() : players(0) { }

	WorldServerAcceptConnection *player;
	uint16_t id;
	uint32_t ip;
	vector<vector<uint32_t> > external_ip;

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
	void registerChannel(WorldServerAcceptConnection *player, uint16_t channel, uint32_t ip, const vector<vector<uint32_t> > &extIp, uint16_t port);
	void removeChannel(uint16_t channel);
	Channel * getChannel(uint16_t num);
	void sendToAll(PacketCreator &packet);
	uint16_t size();
	uint16_t getAvailableChannel();
private:
	Channels() {};
	static Channels *singleton;

	unordered_map<uint16_t, shared_ptr<Channel> > channels;
};
