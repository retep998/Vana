/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "PartyHandler.hpp"
#include "InterHelper.hpp"
#include "PacketReader.hpp"
#include "Party.hpp"
#include "PartyPacket.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "SyncPacket.hpp"

auto PartyHandler::handleRequest(Player *player, PacketReader &packet) -> void {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case PartyActions::Create:
		case PartyActions::Leave:
			SyncPacket::PartyPacket::sync(type, player->getId());
			break;
		case PartyActions::Join: {
			int32_t partyId = packet.get<int32_t>();
			if (Party *party = PlayerDataProvider::getInstance().getParty(partyId)) {
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
			const string_t &invName = packet.getString();
			if (player->getParty() == nullptr) {
				// ??
				return;
			}
			if (Player *invitee = PlayerDataProvider::getInstance().getPlayer(invName)) {
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