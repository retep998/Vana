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
#ifndef WORLDSERVER_H
#define WORLDSERVER_H

#include "AbstractServer.h"
#include "LoginServerConnectPlayer.h"
#include "WorldServerAcceptPlayer.h"
#include "Config.h"

// WorldServer main application class, implemented as singleton
class WorldServer : public AbstractServer {
public:
	static WorldServer * Instance() {
		if (singleton == 0)
			singleton = new WorldServer;
		return singleton;
	}
	void loadData();
	void loadConfig();
	void listen();
	void shutdown();

	char getWorldId() const { return worldId; }
	void setWorldId(char id) { worldId = id; }
	void setInterPort(int port) { inter_port = port; }
private:
	WorldServer() {};
	WorldServer(const WorldServer&);
	WorldServer& operator=(const WorldServer&);
	static WorldServer *singleton;

	LoginServerConnectPlayer *loginPlayer;
	char login_ip[15];
	int login_inter_port;
	int inter_port;
	char worldId;
};

#endif