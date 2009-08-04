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
#ifndef WORLDSERVERCONNECTPACKET_H
#define WORLDSERVERCONNECTPACKET_H

#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class PacketCreator;
class Player;
class WorldServerConnectPlayer;

namespace WorldServerConnectPacket {
	void playerChangeChannel(WorldServerConnectPlayer *player, Player *info, uint16_t channel);
	void playerBuffsTransferred(WorldServerConnectPlayer *player, int32_t playerid);
	void registerPlayer(WorldServerConnectPlayer *player, uint32_t ip, int32_t playerid, const string &name, int32_t map, int32_t job, int32_t level);
	void removePlayer(WorldServerConnectPlayer *player, int32_t playerid);
	void findPlayer(WorldServerConnectPlayer *player, int32_t playerid, const string &findee_name);
	void whisperPlayer(WorldServerConnectPlayer *player, int32_t playerid, const string &whisperee, const string &message);
	void scrollingHeader(WorldServerConnectPlayer *player, const string &message);
	void partyOperation(WorldServerConnectPlayer *player, int8_t type, int32_t playerid, int32_t target = 0);
	void partyInvite(WorldServerConnectPlayer *player, int32_t playerid, const string &invitee);
	void updateLevel(WorldServerConnectPlayer *player, int32_t playerid, int32_t level);
	void updateJob(WorldServerConnectPlayer *player, int32_t playerid, int32_t job);
	void updateMap(WorldServerConnectPlayer *player, int32_t playerid, int32_t map);
	void groupChat(WorldServerConnectPlayer *player, int8_t type, int32_t playerid, const vector<int32_t> &receivers, const string &chat);
	void rankingCalculation(WorldServerConnectPlayer *player);
	void toChannels(WorldServerConnectPlayer *player, PacketCreator &packet);
	void toWorlds(WorldServerConnectPlayer *player, PacketCreator &packet);
	void worldMessage(WorldServerConnectPlayer *player, const string &message, int8_t type);
	void globalMessage(WorldServerConnectPlayer *player, const string &message, int8_t type);
	void reloadMcdb(WorldServerConnectPlayer *player, const string &type);
};

#endif
