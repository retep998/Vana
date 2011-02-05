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
#pragma once

#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class Player;
class WorldServerConnection;

namespace SyncPacket {
	void playerChangeChannel(WorldServerConnection *player, Player *info, uint16_t channel);
	void playerChangeServer(WorldServerConnection *player, Player *info, bool cashShop);
	void playerBuffsTransferred(WorldServerConnection *player, int32_t playerid, bool fromCashOrMts);
	void registerPlayer(WorldServerConnection *player, uint32_t ip, int32_t playerid, const string &name, int32_t map, int16_t job, uint8_t level, int32_t guildid, uint8_t guildrank, int32_t allianceid, uint8_t alliancerank);
	void removePlayer(WorldServerConnection *player, int32_t playerid);
	void partyOperation(WorldServerConnection *player, int8_t type, int32_t playerid, int32_t target = 0);
	void partyInvite(WorldServerConnection *player, int32_t playerid, const string &invitee);
	void updateLevel(WorldServerConnection *player, int32_t playerid, uint8_t level);
	void updateJob(WorldServerConnection *player, int32_t playerid, int16_t job);
	void updateMap(WorldServerConnection *player, int32_t playerid, int32_t map);
	void buddyInvite(WorldServerConnection *player, int32_t playerid, int32_t inviteeid);
	void buddyOnline(WorldServerConnection *player, int32_t playerid, vector<int32_t> players, bool online);
}
