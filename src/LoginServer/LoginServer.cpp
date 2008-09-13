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
#include "LoginServer.h"
#include "ConnectionManager.h"
#include "Worlds.h"

LoginServer * LoginServer::singleton = 0;

void LoginServer::listen() {
	ConnectionManager::Instance()->accept(port, new PlayerLoginFactory(), "0");
	ConnectionManager::Instance()->accept(inter_port, new LoginServerAcceptPlayerFactory());
}

void LoginServer::loadData() {
	Initializing::initializeMySQL();
	Initializing::checkSchemaVersion(true);
	Initializing::setUsersOffline(1);
}

void LoginServer::loadConfig() {
	ConfigFile config("conf/loginserver.lua");
	pinEnabled = config.getBool("pin");
	port = config.getShort("port");
	inter_port = config.getShort("inter_port");
	invalid_login_threshold = config.getInt("invalid_login_threshold");
	to_listen = true;

	// Let's load our worlds
	config.loadFile("conf/worlds.lua");
	size_t i = 0;
	while (1) {
		char buf[25];
		sprintf_s(buf, "world%d_name", i);
		if (!config.keyExist(buf))
			break; //No more worlds

		World *world = new World();
		world->name = config.getString(buf);

		sprintf_s(buf, "world%d_channels", i);
		world->maxChannels = config.getInt(buf);

		sprintf_s(buf, "world%d_id", i);
		world->id = (uint8_t) config.getInt(buf);

		sprintf_s(buf, "world%d_ribbon", i);
		world->ribbon = (uint8_t) config.getInt(buf);

		sprintf_s(buf, "world%d_port", i);
		world->port = config.getShort(buf);

		sprintf_s(buf, "world%d_exprate", i);
		world->exprate = config.getInt(buf);

		sprintf_s(buf, "world%d_questexprate", i);
		world->questexprate = config.getInt(buf);

		sprintf_s(buf, "world%d_mesorate", i);
		world->mesorate = config.getInt(buf);

		sprintf_s(buf, "world%d_droprate", i);
		world->droprate = config.getInt(buf);

		sprintf_s(buf, "world%d_max_multi_level", i);
		world->maxMultiLevel = (uint8_t) config.getInt(buf);

		world->connected = false;
		Worlds::worlds[world->id] = world;
		i++;
	}
}

void LoginServer::shutdown() {
	//TODO
}


