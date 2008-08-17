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

#include <unordered_map>
#include <memory>
#include <string>

using std::string;
using std::tr1::shared_ptr;
using std::tr1::unordered_map;

class WorldServerAcceptPlayer;
class PacketCreator;

struct Channel {
	Channel() : players(0) { }

	WorldServerAcceptPlayer *player;
	int id;
	string ip;
	short port;
	int players;
};

class Channels {
public:
	static Channels * Instance() {
		if (singleton == 0)
			singleton = new Channels;
		return singleton;
	}
	void registerChannel(WorldServerAcceptPlayer *player, int channel, const string &ip, short port);
	void removeChannel(int channel);
	Channel * getChannel(int num);
	void sendToAll(PacketCreator &packet);
	int size();
private:
	Channels() {};
	Channels(const Channels&);
	Channels& operator=(const Channels&);
	static Channels *singleton;

	unordered_map<int, shared_ptr<Channel>> channels;
};

#endif
