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

class Alliance;
class Guild;
class PacketCreator;
class Party;
class Player;

namespace GuildPacket {
	void sendGuildInfo(Guild *guild, Player *requestee, bool isNew = false);
	void sendInvite(Player *inviter, Player *invitee);
	void sendCapacityUpdate(Guild *guild);
	void sendEmblemUpdate(Guild *guild);
	void sendGuildPointsUpdate(Guild *guild);
	void sendTitlesUpdate(Guild *guild);
	void sendNoticeUpdate(Guild *guild);
	void sendRankUpdate(Guild *guild, Player *player);
	void sendPlayerStatUpdate(Guild *guild, Player *player, bool levelMessage, bool isFromLogon = false);
	void sendPlayerUpdate(Guild *guild, Player *player, uint8_t type, bool toSelf = true);
	void sendGuildDisband(Guild *guild);
	void sendGuildContract(Player *player, bool isLeader, int32_t partyId = 0, const string &creator = "", const string &guildName = "");

	void sendGuildDenyResult(Player *inviter, Player *invitee, uint8_t result);
	void sendPlayerGuildMessage(Player *player, uint8_t type);
	void sendPlayerMessage(Player *player, uint8_t sort, const string &message);
	void sendGuildRankBoard(Player *player, int32_t npcid);

	void addGuildInformation(PacketCreator &packet, Guild *guild);
	void sendToGuild(PacketCreator &packet, Guild *guild, Player *player = 0);
};
