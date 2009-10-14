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
#ifndef ALLIANCEPACKET_H
#define ALLIANCEPACKET_H

#include "Alliances.h"
#include "Guild.h"
#include "GuildBbs.h"
#include "PacketCreator.h"
#include "Types.h"
#include <string>

using std::string;

class Alliance;
class Guild;
class PacketCreator;

namespace AlliancePacket {
	namespace InterServerPacket {
		void changeAlliance(Alliance *alliance, int8_t type);
		void changeGuild(Alliance *alliance, Guild *guild);
		void changeLeader(Alliance *alliance, Player *oldLeader);
		void changePlayerRank(Alliance *alliance, Player *player);
		void changeCapacity(Alliance *alliance);
	};
	
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

#endif
