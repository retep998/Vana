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
#include "Party.h"
#include "Player.h"

Party::Party(int32_t id) :
m_id(id),
m_leaderId(0),
m_voters(0),
m_guildContract(0)
{
}

void Party::addMember(Player *player) {
	members[player->getId()] = player;
}

void Party::setLeader(int32_t playerId) {
	m_oldLeaders.push_back(m_leaderId);
	m_leaderId = playerId;
}

void Party::clearGuild() {
	setVoters(0);
	setGuildContract(0);
	setGuildName("");
}