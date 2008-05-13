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
#ifndef CHANNELSERVER_H
#define CHANNELSERVER_H

#include "AbstractServer.h"

class WorldServerConnectPlayer;

// ChannelServer main application class, implemented as singleton
class ChannelServer : public AbstractServer {
public:
	static ChannelServer * Instance() {
		if (singleton == 0)
			singleton = new ChannelServer;
		return singleton;
	}
	void loadData();
	void loadConfig();
	void listen();
	void connectWorld();
	void shutdown();

	char getWorld() { return world; }
	void setWorld(char id) { world = id; }
	void setWorldIp(char *ip) { strcpy_s(world_ip, ip); }
	void setWorldPort(int port) { world_port = port; }
	int getChannel() { return channel; }
	void setChannel(int channel) {}
	void setPort(int port) { this->port = port; }
	int getOnlineId() { return 20000 + (int) world * 1000 + channel * 10; }
private:
	ChannelServer() {};
	ChannelServer(const ChannelServer&);
	ChannelServer& operator=(const ChannelServer&);
	static ChannelServer *singleton;

	WorldServerConnectPlayer *worldPlayer;

	char login_ip[15];
	int login_inter_port;
	int port;
	char world;
	char world_ip[15];
	int world_port;
	int channel;
};

#endif