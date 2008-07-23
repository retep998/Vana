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
#include <string>

using std::string;

class WorldServerConnectPlayer;
class Packet;

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
	void sendToWorld(Packet &packet);

	char getWorld() { return world; }
	void setWorld(char id) { world = id; }
	void setWorldIp(const string &ip) { world_ip = ip; }
	void setWorldPort(short port) { world_port = port; }
	WorldServerConnectPlayer * getWorldPlayer() { return worldPlayer; }
	int getChannel() { return channel; }
	void setChannel(int channel) { this->channel = channel; }
	void setPort(short port) { this->port = port; }
	int getOnlineId() { return 20000 + (int) world * 100 + channel; }
	string getScrollingHeader() { return scrollingHeader; }
	void setScrollingHeader(const string &message);
	// Server rates
	void setExprate(int exprate) { this->exprate = exprate; }
	void setQuestExprate(int questexprate) { this->questexprate = questexprate; }
	void setMesorate (int mesorate) { this->mesorate = mesorate; }
	void setDroprate (int droprate) { this->droprate = droprate; }
	int getExprate() { return exprate; }
	int getQuestExprate() { return questexprate; }
	int getMesorate() { return mesorate; }
	int getDroprate() { return droprate; }
private:
	ChannelServer() {};
	ChannelServer(const ChannelServer&);
	ChannelServer& operator=(const ChannelServer&);
	static ChannelServer *singleton;

	WorldServerConnectPlayer *worldPlayer;

	string login_ip;
	short login_inter_port;
	short port;
	char world;
	string world_ip;
	short world_port;
	int channel;
	string external_ip;
	string scrollingHeader;
	// Server rates
	int exprate;
	int questexprate;
	int mesorate;
	int droprate;
};

#endif
