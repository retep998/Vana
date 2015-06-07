/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "ChannelServer.hpp"
#include "InterHelper.hpp"
#include "PacketReader.hpp"
#include "Party.hpp"
#include "PartyPacket.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "SyncPacket.hpp"

auto PartyHandler::handleRequest(Player *player, PacketReader &reader) -> void {
	int8_t type = reader.get<int8_t>();
	switch (type) {
		case PartyActions::Create:
		case PartyActions::Leave:
			ChannelServer::getInstance().sendWorld(SyncPacket::PartyPacket::sync(type, player->getId()));
			break;
		case PartyActions::Join: {
			party_id_t partyId = reader.get<party_id_t>();
			if (Party *party = ChannelServer::getInstance().getPlayerDataProvider().getParty(partyId)) {
				if (party->getMembersCount() == Parties::MaxMembers) {
					player->send(PartyPacket::error(PartyPacket::Errors::PartyFull));
				}
				else {
					ChannelServer::getInstance().sendWorld(SyncPacket::PartyPacket::sync(type, player->getId(), partyId));
				}
			}
			break;
		}
		case PartyActions::Expel:
		case PartyActions::SetLeader: {
			ChannelServer::getInstance().sendWorld(SyncPacket::PartyPacket::sync(type, player->getId(), reader.get<int32_t>()));
			break;
		}
		case PartyActions::Invite: {
			string_t invName = reader.get<string_t>();
			if (player->getParty() == nullptr) {
				// ??
				return;
			}
			if (Player *invitee = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(invName)) {
				if (invitee->getParty() != nullptr) {
					player->send(PartyPacket::error(PartyPacket::Errors::PlayerHasParty));
				}
				else {
					invitee->send(PartyPacket::invitePlayer(player->getParty(), player->getName()));
				}
			}
			else {
				player->send(PartyPacket::error(PartyPacket::Errors::DifferingChannel));
			}
			break;
		}
	}
}