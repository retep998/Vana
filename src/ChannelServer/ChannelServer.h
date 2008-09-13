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
#include "Types.h"
#include <string>

using std::string;

class WorldServerConnectPlayer;
class PacketCreator;

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
	void sendToWorld(PacketCreator &packet);

	char getWorld() { return world; }
	void setWorld(char id) { world = id; }
	void setWorldIp(const string &ip) { world_ip = ip; }
	void setWorldPort(int16_t port) { world_port = port; }
	WorldServerConnectPlayer * getWorldPlayer() { return worldPlayer; }
	uint16_t getChannel() { return channel; }
	void setChannel(uint16_t channel) { this->channel = channel; }
	void setPort(int16_t port) { this->port = port; }
	int32_t getOnlineId() { return 20000 + (int32_t) world * 100 + channel; }
	string getScrollingHeader() { return scrollingHeader; }
	void setScrollingHeader(const string &message);
	unsigned char getMaxMultiLevel() const { return maxMultiLevel; }
	void setMaxMultiLevel(unsigned char level) { maxMultiLevel = level; }
	// Server rates
	void setExprate(int32_t exprate) { this->exprate = exprate; }
	void setQuestExprate(int32_t questexprate) { this->questexprate = questexprate; }
	void setMesorate (int32_t mesorate) { this->mesorate = mesorate; }
	void setDroprate (int32_t droprate) { this->droprate = droprate; }
	int32_t getExprate() { return exprate; }
	int32_t getQuestExprate() { return questexprate; }
	int32_t getMesorate() { return mesorate; }
	int32_t getDroprate() { return droprate; }
private:
	ChannelServer() {};
	ChannelServer(const ChannelServer&);
	ChannelServer& operator=(const ChannelServer&);
	static ChannelServer *singleton;

	WorldServerConnectPlayer *worldPlayer;

	string login_ip;
	uint16_t login_inter_port;
	uint16_t port;
	char world;
	string world_ip;
	uint16_t world_port;
	uint16_t channel;
	string external_ip;
	string scrollingHeader;
	unsigned char maxMultiLevel;
	// Server rates
	int32_t exprate;
	int32_t questexprate;
	int32_t mesorate;
	int32_t droprate;
};

#endif
