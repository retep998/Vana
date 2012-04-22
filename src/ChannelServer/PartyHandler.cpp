/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "PartyHandler.h"
#include "InterHelper.h"
#include "PacketReader.h"
#include "Party.h"
#include "PartyPacket.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SyncPacket.h"

void PartyHandler::handleRequest(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case PartyActions::Create:
		case PartyActions::Leave:
			SyncPacket::PartyPacket::sync(type, player->getId());
			break;
		case PartyActions::Join: {
			int32_t partyId = packet.get<int32_t>();
			if (Party *party = PlayerDataProvider::Instance()->getParty(partyId)) {
				if (party->getMembersCount() == Parties::MaxMembers) {
					PartyPacket::error(player, PartyPacket::Errors::PartyFull);
				}
				else {
					SyncPacket::PartyPacket::sync(type, player->getId(), partyId);
				}
			}
			break;
		}
		case PartyActions::Expel:
		case PartyActions::SetLeader: {
			SyncPacket::PartyPacket::sync(type, player->getId(), packet.get<int32_t>());
			break;
		}
		case PartyActions::Invite: {
			const string &invName = packet.getString();
			if (Player *invitee = PlayerDataProvider::Instance()->getPlayer(invName)) {
				if (invitee->getParty() != nullptr) {
					PartyPacket::error(player, PartyPacket::Errors::PlayerHasParty);
				}
				else {
					PartyPacket::invitePlayer(invitee, player->getParty(), player->getName());
				}
			}
			else {
				PartyPacket::error(player, PartyPacket::Errors::DifferingChannel);
			}
			break;
		}
	}
}