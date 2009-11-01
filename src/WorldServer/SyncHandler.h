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
#ifndef SYNCHANDLER_H
#define SYNCHANLDER_H

#include "Types.h"
#include <string>

using std::string;

class Guild;
class LoginServerConnection;
class PacketCreator;
class PacketReader;
class Player;
class WorldServerAcceptConnection;

namespace SyncHandler {
	// Alliance
	void handleAlliancePacket(WorldServerAcceptConnection *player, PacketReader &packet);
	void handleAllianceCreation(PacketReader &packet);
	void loadAlliance(int32_t allianceid);
	void sendAllianceInfo(int32_t playerid, int32_t allianceid);
	void sendNoticeUpdate(int32_t allianceid, PacketReader &packet);
	void sendTitleUpdate(int32_t allianceid, PacketReader &packet);
	void sendPlayerUpdate(int32_t allianceid, int32_t playerid);
	void sendPlayerChangeRank(int32_t allianceid, PacketReader &packet);
	void sendChangeGuild(int32_t allianceid, PacketReader &packet);
	void sendExpelGuild(int32_t allianceid, PacketReader &packet);
	void sendAllianceDisband(int32_t allianceid, int32_t playerid);
	void sendInvite(int32_t allianceid, PacketReader &packet);
	void sendChangeLeader(int32_t allianceid, PacketReader &packet);
	void sendIncreaseCapacity(int32_t allianceid, int32_t playerid);
	void sendDenyPacket(PacketReader &packet);
	void removeGuild(Guild *guild);
	// BBS
	void handleBbsPacket(WorldServerAcceptConnection *player, PacketReader &packet);
	void handleNewThread(PacketReader &packet);
	void handleDeleteThread(PacketReader &packet);
	void handleNewReply(PacketReader &packet);
	void handleDeleteReply(PacketReader &packet);
	void handleShowThreadList(PacketReader &packet);
	void handleShowThread(PacketReader &pack);
	void handleShowThread(int32_t playerid, int32_t threadid);
	// Guild
	void handleGuildPacket(WorldServerAcceptConnection *player, PacketReader &packet);
	void handleLoginServerPacket(LoginServerConnection *player, PacketReader &packet);
	void loadGuild(int32_t id);
	void handleGuildCreation(PacketReader &packet);
	void sendDeletePlayer(int32_t guildid, int32_t pid, const string &name, bool expelled);
	void sendGuildInvite(int32_t guildid, PacketReader &packet);
	void sendNewPlayer(int32_t guildid, int32_t pid, bool newGuild = true);
	void sendUpdateOfTitles(int32_t guildid, PacketReader &packet);
	void sendGuildNotice(int32_t guildid, PacketReader &packet);
	// Party
	void createParty(WorldServerAcceptConnection *player, int32_t playerid);
	void leaveParty(WorldServerAcceptConnection *player, int32_t playerid);
	void invitePlayer(WorldServerAcceptConnection *player, int32_t playerid, const string &invitee);
	void joinParty(WorldServerAcceptConnection *player, int32_t playerid, int32_t partyid);
	void giveLeader(WorldServerAcceptConnection *player, int32_t playerid, int32_t target, bool is);
	void expelPlayer(WorldServerAcceptConnection *player, int32_t playerid, int32_t target);
	void silentUpdate(int32_t playerid);
	void logInLogOut(int32_t playerid);
	// Stuff
	void playerConnect(WorldServerAcceptConnection *player, PacketReader &packet);
	void playerDisconnect(WorldServerAcceptConnection *player, PacketReader &packet);
	void playerChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet);
	void handleChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet);
	void partyOperation(WorldServerAcceptConnection *player, PacketReader &packet);
	void updateLevel(WorldServerAcceptConnection *player, PacketReader &packet);
	void updateJob(WorldServerAcceptConnection *player, PacketReader &packet);
	void updateMap(WorldServerAcceptConnection *player, PacketReader &packet);
};

#endif
