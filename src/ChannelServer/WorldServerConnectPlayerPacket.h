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
#ifndef WORLDSERVERCONNECTPLAYERPACKET_H
#define WORLDSERVERCONNECTPLAYERPACKET_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class WorldServerConnectPlayer;

namespace WorldServerConnectPlayerPacket {
	void playerChangeChannel(WorldServerConnectPlayer *player, int playerid, int channel);
	void registerPlayer(WorldServerConnectPlayer *player, int playerid, const string &name, int map, int job, int level);
	void removePlayer(WorldServerConnectPlayer *player, int playerid);
	void findPlayer(WorldServerConnectPlayer *player, int playerid, const string &findee_name);
	void whisperPlayer(WorldServerConnectPlayer *player, int playerid, const string &whisperee, const string &message);
	void scrollingHeader(WorldServerConnectPlayer *player, const string &message);
	void partyOperation(WorldServerConnectPlayer *player, char type, int playerid, int target = 0);
	void partyInvite(WorldServerConnectPlayer *player, int playerid, const string &invitee);
	void updateLevel(WorldServerConnectPlayer *player, int playerid, int level);
	void updateJob(WorldServerConnectPlayer *player, int playerid, int job);
	void updateMap(WorldServerConnectPlayer *player, int playerid, int map);
	void groupChat(WorldServerConnectPlayer *player, char type, int playerid, const vector<int> &receivers, const string &chat);
};

#endif
