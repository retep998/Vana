/*
Copyright (C) 2008-2009 Vana Development Team

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
#include <boost/utility.hpp>
#include <string>

using std::string;

class WorldServerConnectPlayer;
class PacketCreator;

// ChannelServer main application class, implemented as singleton
class ChannelServer : public AbstractServer, boost::noncopyable {
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
	void sendToWorld(PacketCreator &packet);

	void setWorld(int8_t id) { world = id; }
	void setMaxMultiLevel(uint8_t level) { maxMultiLevel = level; }
	void setWorldPort(uint16_t port) { world_port = port; }
	void setPort(uint16_t port) { this->port = port; }
	void setChannel(uint16_t channel) { this->channel = channel; }
	void setExprate(int32_t exprate) { this->exprate = exprate; }
	void setQuestExprate(int32_t questexprate) { this->questexprate = questexprate; }
	void setMesorate(int32_t mesorate) { this->mesorate = mesorate; }
	void setDroprate(int32_t droprate) { this->droprate = droprate; }
	void setMaxStats(int16_t max) { this->maxStats = max; }
	void setWorldClock(clock_t clock) { worldClock = clock; }
	void setScrollingHeader(const string &message);
	void setWorldIp(uint32_t ip) { world_ip = ip; }

	int8_t getWorld() const { return world; }
	uint8_t getMaxMultiLevel() const { return maxMultiLevel; }
	int16_t getMaxStats() const { return maxStats; }
	uint16_t getChannel() const { return channel; }
	int32_t getOnlineId() const { return 20000 + (int32_t) world * 100 + channel; }
	int32_t getExprate() const { return exprate; }
	int32_t getQuestExprate() const { return questexprate; }
	int32_t getMesorate() const { return mesorate; }
	int32_t getDroprate() const { return droprate; }
	clock_t getWorldClock() const { return worldClock; }
	string getScrollingHeader() const { return scrollingHeader; }
	WorldServerConnectPlayer * getWorldPlayer() const { return worldPlayer; }
private:
	ChannelServer() {};
	static ChannelServer *singleton;

	WorldServerConnectPlayer *worldPlayer;

	int8_t world;
	uint8_t maxMultiLevel;
	int16_t maxStats;
	uint16_t world_port;
	uint16_t channel;
	uint16_t login_inter_port;
	uint16_t port;
	int32_t exprate;
	int32_t questexprate;
	int32_t mesorate;
	int32_t droprate;
	clock_t worldClock;
	uint32_t login_ip;
	uint32_t world_ip;
	string scrollingHeader;
};

#endif
