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
#include <boost/tr1/functional.hpp>
#include <boost/utility.hpp>
#include <map>
#include <string>

using std::map;
using std::tr1::function;
using std::string;

class Channel;
class LoginServerAcceptConnection;
class PacketCreator;
class PacketReader;
class Player;
class World;

#include "Types.h"

class Worlds : public boost::noncopyable {
public:
	static Worlds * Instance() {
		if (singleton == nullptr)
			singleton = new Worlds;
		return singleton;
	}

	void channelSelect(Player *player, PacketReader &packet);
	void selectWorld(Player *player, PacketReader &packet);
	void showWorld(Player *player);
	void toWorlds(PacketCreator &packet);
	void addWorld(World *world);
	void calculatePlayerLoad(World *world);
	void runFunction(function<bool (World *)> func);
	void setEventMessages(const string &message);

	World * getWorld(uint8_t id);

	// Inter-server
	int8_t addWorldServer(LoginServerAcceptConnection *player);
	int8_t addChannelServer(LoginServerAcceptConnection *player);
	int8_t addCashServer(LoginServerAcceptConnection *player);
private:
	Worlds() {};
	static Worlds *singleton;

	map<uint8_t, World *> worlds;
};
