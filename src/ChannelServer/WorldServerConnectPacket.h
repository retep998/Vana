/*
Copyright (C) 2008-2011 Vana Development Team

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
class WorldServerConnection;

namespace WorldServerConnectPacket {
	void playerChangeChannel(WorldServerConnection *player, Player *info, uint16_t channel);
	void playerBuffsTransferred(WorldServerConnection *player, int32_t playerid);
	void registerPlayer(WorldServerConnection *player, uint32_t ip, int32_t playerid, const string &name, int32_t map, int32_t job, int32_t level);
	void removePlayer(WorldServerConnection *player, int32_t playerid);
	void findPlayer(WorldServerConnection *player, int32_t playerid, const string &findee_name);
	void whisperPlayer(WorldServerConnection *player, int32_t playerid, const string &whisperee, const string &message);
	void scrollingHeader(WorldServerConnection *player, const string &message);
	void partyOperation(WorldServerConnection *player, int8_t type, int32_t playerid, int32_t target = 0);
	void partyInvite(WorldServerConnection *player, int32_t playerid, const string &invitee);
	void updateLevel(WorldServerConnection *player, int32_t playerid, int32_t level);
	void updateJob(WorldServerConnection *player, int32_t playerid, int32_t job);
	void updateMap(WorldServerConnection *player, int32_t playerid, int32_t map);
	void groupChat(WorldServerConnection *player, int8_t type, int32_t playerid, const vector<int32_t> &receivers, const string &chat);
	void rankingCalculation(WorldServerConnection *player);
	void toChannels(WorldServerConnection *player, PacketCreator &packet);
	void toWorlds(WorldServerConnection *player, PacketCreator &packet);
	void worldMessage(WorldServerConnection *player, const string &message, int8_t type);
	void globalMessage(WorldServerConnection *player, const string &message, int8_t type);
	void reloadMcdb(WorldServerConnection *player, const string &type);
};

#endif
