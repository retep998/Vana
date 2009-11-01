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
#include "PartyHandler.h"
#include "ChannelServer.h"
#include "InterHelper.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SyncPacket.h"

void PartyHandler::handleRequest(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case 0x01: // Create party
		case 0x02: // Leave party
			SyncPacket::partyOperation(ChannelServer::Instance()->getWorldConnection(), type, player->getId());
			break;
		case 0x03: // Join party
		case 0x05: // Expel Player
		case 0x06: // Give leader rights
			SyncPacket::partyOperation(ChannelServer::Instance()->getWorldConnection(), type, player->getId(), packet.get<int32_t>());
			break;
		case 0x04: // Invite
			SyncPacket::partyInvite(ChannelServer::Instance()->getWorldConnection(), player->getId(), packet.getString());
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