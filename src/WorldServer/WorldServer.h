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
#include "ConfigFile.h"
#include "LoginServerConnectPlayer.h"
#include "Types.h"
#include "WorldServerAcceptPlayer.h"
#include <string>

using std::string;

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

	LoginServerConnectPlayer * getLoginPlayer() const { return loginPlayer; }
	char getWorldId() const { return worldId; }
	void setWorldId(char id) { worldId = id; }
	uint16_t getInterPort() const { return inter_port; }
	void setInterPort(uint16_t port) { inter_port = port; }
	int32_t getMaxChannels() const { return maxChannels; }
	void setMaxChannels(int32_t maxchan) { maxChannels = maxchan; }
	unsigned char getMaxMultiLevel() const { return maxMultiLevel; }
	void setMaxMultiLevel(unsigned char level) { maxMultiLevel = level; }
	string getScrollingHeader() { return scrollingHeader.c_str(); }
	void setScrollingHeader(const string &message);
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
	WorldServer() { scrollingHeader = ""; };
	WorldServer(const WorldServer&);
	WorldServer& operator=(const WorldServer&);
	static WorldServer *singleton;

	LoginServerConnectPlayer *loginPlayer;
	string login_ip;
	uint16_t login_inter_port;
	uint16_t inter_port;
	char worldId;
	int32_t maxChannels;
	unsigned char maxMultiLevel;
	string external_ip;
	string scrollingHeader;
	// Server rates
	int32_t exprate;
	int32_t questexprate;
	int32_t mesorate;
	int32_t droprate;
};

#endif
