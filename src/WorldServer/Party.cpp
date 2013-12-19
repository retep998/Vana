/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "GameConstants.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SyncPacket.h"

Party::Party(int32_t id, int32_t leaderId) :
	m_id(id),
	m_leaderId(leaderId)
{
	SyncPacket::PartyPacket::createParty(id, leaderId);
}

void Party::addMember(Player *player, bool first) {
	m_members[player->getId()] = player;
	player->setParty(this);
	if (!first) {
		SyncPacket::PartyPacket::addPartyMember(getId(), player->getId());
	}
}

void Party::deleteMember(Player *player, bool kicked) {
	player->setParty(nullptr);
	m_members.erase(player->getId());
	SyncPacket::PartyPacket::removePartyMember(getId(), player->getId(), kicked);
}

void Party::setLeader(Player *newLeader) {
	m_leaderId = newLeader->getId();
	SyncPacket::PartyPacket::newPartyLeader(getId(), newLeader->getId());
}

void Party::disband() {
	for (const auto &kvp : m_members) {
		kvp.second->setParty(nullptr);
	}
	SyncPacket::PartyPacket::disbandParty(getId());
}

void Party::runFunction(function<void (Player *)> func) {
	for (const auto &kvp : m_members) {
		func(kvp.second);
	}
}