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
	short getInterPort() const { return inter_port; }
	void setInterPort(short port) { inter_port = port; }
	int getMaxChannels() const { return maxChannels; }
	void setMaxChannels(int maxchan) { maxChannels = maxchan; }
	string getScrollingHeader() { return scrollingHeader.c_str(); }
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
	WorldServer() { scrollingHeader = ""; };
	WorldServer(const WorldServer&);
	WorldServer& operator=(const WorldServer&);
	static WorldServer *singleton;

	LoginServerConnectPlayer *loginPlayer;
	string login_ip;
	short login_inter_port;
	short inter_port;
	char worldId;
	int maxChannels;
	string external_ip;
	string scrollingHeader;
	// Server rates
	int exprate;
	int questexprate;
	int mesorate;
	int droprate;
};

#endif