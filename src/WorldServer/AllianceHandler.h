/*
Copyright (C) 2009 Vana Development Team

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
#ifndef ALLIANCEHANDLER_H
#define ALLIANCEHANLDER_H

#include "Types.h"
#include <string>

using std::string;

class Guild;
class PacketCreator;
class PacketReader;
class Player;
class WorldServerAcceptConnection;

namespace AllianceHandler {
	void handlePacket(WorldServerAcceptConnection *player, PacketReader &packet);
	void handleAllianceCreation(PacketReader &packet);
	
	void loadAlliance(int32_t allianceid);

	void sendAllianceInfo(int32_t playerid, int32_t allianceid);
	void sendNoticeUpdate(int32_t allianceid, PacketReader &packet);
	void sendTitleUpdate(int32_t allianceid, PacketReader &packet);
	void sendPlayerUpdate(int32_t allianceid, int32_t playerid);
	void sendPlayerChangeRank(int32_t allianceid, PacketReader &packet);
	void sendChangeGuild(int32_t allianceid, PacketReader &packet);
	void sendExpellGuild(int32_t allianceid, PacketReader &packet);
	void sendAllianceDisband(int32_t allianceid, int32_t playerid);
	void sendInvite(int32_t allianceid, PacketReader &packet);
	void sendChangeLeader(int32_t allianceid, PacketReader &packet);
	void sendIncreaseCapacity(int32_t allianceid, int32_t playerid);
	void sendDenyPacket(PacketReader &packet);
	void removeGuild(Guild *guild);
};

#endif
