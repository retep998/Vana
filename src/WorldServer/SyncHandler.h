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

using std::string;

class PacketCreator;
class PacketReader;
class Player;
class WorldServerAcceptConnection;

namespace SyncHandler {
	// Dispatch
	void handle(WorldServerAcceptConnection *connection, PacketReader &packet);
	// Party
	void partyOperation(PacketReader &packet);
	void createParty(int32_t playerid);
	void leaveParty(int32_t playerid);
	void invitePlayer(int32_t playerid, const string &invitee);
	void joinParty(int32_t playerid, int32_t partyid);
	void giveLeader(int32_t playerid, int32_t target, bool is);
	void expelPlayer(int32_t playerid, int32_t target);
	void silentUpdate(int32_t playerid);
	void logInLogOut(int32_t playerid);
	// Player
	void handlePlayerPacket(WorldServerAcceptConnection *connection, PacketReader &packet);
	void playerConnect(uint16_t channel, PacketReader &packet);
	void playerDisconnect(uint16_t channel, PacketReader &packet);
	void playerChangeChannel(WorldServerAcceptConnection *connection, PacketReader &packet);
	void handleChangeChannel(WorldServerAcceptConnection *connection, PacketReader &packet);
	void updateLevel(PacketReader &packet);
	void updateJob(PacketReader &packet);
	void updateMap(PacketReader &packet);
	// Buddy
	void handleBuddyPacket(PacketReader &packet);
	void buddyInvite(PacketReader &packet);
	void buddyOnline(PacketReader &packet);
}