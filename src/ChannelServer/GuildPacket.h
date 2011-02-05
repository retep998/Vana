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

#include "Player.h"
#include "Types.h"
#include <string>

using std::string;

class Player;
class PacketReader;

namespace GuildPacket{
	void sendCreateGuildWindow(Player *player);
	void sendChangeGuildEmblem(Player *player);

	void handlePacket(Player *player, PacketReader &packet);
	void handleDenyPacket(Player *player, PacketReader &packet);
	int8_t checkGuildExist(string name);

	void handleEmblemChange(PacketReader &packet);
	void handleNameChange(PacketReader &packet);

	void guildInvite(int32_t guildid, int32_t playerid, const string &name);
	void guildInviteAccepted(int32_t playerid);
	void removeGuildPlayer(int32_t guildid, int32_t playerid, string name, bool expelled = false);
	void sendNewGuildRankTitles(int32_t guildid, PacketReader &pack);
	void changeRankOfPlayer(int32_t guildid, int32_t playerid, int32_t victimid, uint8_t newrank);
	void changeGuildNotice(int32_t guildid, const string &notice);
	void guildDisband(int32_t guildid);
	void sendIncreaseCapacity(int32_t guildid, int32_t playerid);
	void sendRemoveEmblem(int32_t guildid, int32_t playerid);
	void addGuildPoint(int32_t guildid, int32_t amount = 1);
	void sendEmblemChangeInfo(int32_t guildid, int32_t playerid, PacketReader &pack);
	void displayGuildRankBoard(int32_t playerid, int32_t npcid);
	void displayGuildDeny(PacketReader &pack);
}
