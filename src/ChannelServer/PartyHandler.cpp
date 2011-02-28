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
#include "PartyHandler.h"
#include "ChannelServer.h"
#include "InterHelper.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "WorldServerConnectPacket.h"

void PartyHandler::handleRequest(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case 0x01: // Create party
		case 0x02: // Leave party
			WorldServerConnectPacket::partyOperation(ChannelServer::Instance()->getWorldConnection(), type, player->getId());
			break;
		case 0x03: // Join party
		case 0x05: // Expel Player
		case 0x06: // Give leader rights
			WorldServerConnectPacket::partyOperation(ChannelServer::Instance()->getWorldConnection(), type, player->getId(), packet.get<int32_t>());
			break;
		case 0x04: // Invite
			WorldServerConnectPacket::partyInvite(ChannelServer::Instance()->getWorldConnection(), player->getId(), packet.getString());
			break;
	}
}

void PartyHandler::handleResponse(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t playerid = packet.get<int32_t>();
	int32_t partyid = packet.get<int32_t>();
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Party *party = PlayerDataProvider::Instance()->getParty(partyid);
	if (player == 0 || party == 0)
		return;
	switch (type) {
		case 0x01: // Leave / Disband
		case 0x06: // Expel
			party->deleteMember(player);
			break;
		case 0x02: // Create / Join
			player->setParty(party);
			party->addMember(player);
			party->showHpBar(player);
			party->receiveHpBar(player);
			break;
		case 0x05: // LogInLogOut
			player->setParty(party);
			party->setMember(player->getId(), player);
			party->showHpBar(player);
			party->receiveHpBar(player);
			break;
	}
}

void PartyHandler::handleDataSync(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case PartyActions::Sync::ChannelStart:
			handleChannelStart(packet);
			break;
		case PartyActions::Sync::Disband:
			disbandParty(packet);
			break;
		case PartyActions::Sync::Create: {
			int32_t partyid = packet.get<int32_t>();
			int32_t leaderid = packet.get<int32_t>();
			Party *party = new Party(partyid);
			Player *leader = PlayerDataProvider::Instance()->getPlayer(leaderid);
			if (leader == 0) {
				party->addMember(leaderid);
			}
			else {
				party->addMember(leader);
			}
			party->setLeader(leaderid, true);
			PlayerDataProvider::Instance()->addParty(party);
			break;
		}
		case PartyActions::Sync::SwitchLeader: {
			int32_t partyid = packet.get<int32_t>();
			if (Party *party = PlayerDataProvider::Instance()->getParty(partyid)) {
				int32_t newleader = packet.get<int32_t>();
				party->setLeader(newleader);
			}
			break;
		}
		case PartyActions::Sync::RemoveMember: {
			int32_t partyid = packet.get<int32_t>();
			if (Party *party = PlayerDataProvider::Instance()->getParty(partyid)) {
				int32_t playerid = packet.get<int32_t>();
				Player *member = PlayerDataProvider::Instance()->getPlayer(playerid);
				if (member == 0) {
					party->deleteMember(playerid);
				}
				else {
					party->deleteMember(member);
				}
			}
			break;
		}
		case PartyActions::Sync::AddMember: {
			int32_t partyid = packet.get<int32_t>();
			if (Party *party = PlayerDataProvider::Instance()->getParty(partyid)) {
				int32_t playerid = packet.get<int32_t>();
				Player *member = PlayerDataProvider::Instance()->getPlayer(playerid);
				if (member == 0) {
					party->addMember(playerid);
				}
				else {
					party->addMember(member);
				}
			}
			break;
		}
	}
}

void PartyHandler::handleChannelStart(PacketReader &packet) {
	int32_t numberparties = packet.get<int32_t>();
	for (int32_t i = 0; i < numberparties; i++) {
		int32_t partyid = packet.get<int32_t>();
		int8_t membersnum = packet.get<int8_t>();
		Party *party = new Party(partyid);
		for (int8_t j = 0; j < membersnum; j++) {
			int32_t memberid = packet.get<int32_t>();
			party->addMember(memberid);
		}
		int32_t leaderid = packet.get<int32_t>();
		party->setLeader(leaderid, true);
		PlayerDataProvider::Instance()->addParty(party);
	}
}

void PartyHandler::disbandParty(PacketReader &packet) {
	int32_t partyid = packet.get<int32_t>();
	if (Party *party = PlayerDataProvider::Instance()->getParty(partyid)) {
		party->disband();
		PlayerDataProvider::Instance()->removeParty(party->getId());
	}
}