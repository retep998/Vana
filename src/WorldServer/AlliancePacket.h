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
class Player;

namespace AlliancePacket {
	void sendAllianceInfo(Alliance *alliance, Player *requestee);
	void sendInvite(Alliance *alliance, Player *inviter, Player *invitee);
	void sendInviteAccepted(Alliance *alliance, Guild *guild);
	void sendInviteDenied(Alliance *alliance, Guild *guild);
	void sendGuildLeft(Alliance *alliance, Guild *guild, bool expelled);
	void sendNewAlliance(Alliance *alliance);
	void sendDeleteAlliance(Alliance *alliance);
	void sendUpdateCapacity(Alliance *alliance);
	void sendUpdateLeader(Alliance *alliance, Player *oldLeader);
	void sendUpdatePlayer(Alliance *alliance, Player *player, uint8_t option);
	void sendUpdateNotice(Alliance *alliance);
	void sendUpdateTitles(Alliance *alliance);

	void addAllianceInfo(PacketCreator &packet, Alliance *alliance);
	void addGuildsInfo(PacketCreator &packet, Alliance *alliance, bool addSize = true);
	void sendToAlliance(PacketCreator &packet, Alliance *alliance, Player *skipped = 0);
};
