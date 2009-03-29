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
#include "Party.h"
#include "Maps.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "WorldServerConnectPacket.h"
#include "ChannelServer.h"
#include "PacketReader.h"

void Party::showHPBar(Player *player) {
	for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
		Player *m_player = Maps::getMap(player->getMap())->getPlayer(i);
		if (m_player->getPartyId() == player->getPartyId() && m_player != player)
			PlayerPacket::showHPBar(player, m_player);
	}
}

void Party::receiveHPBar(Player *player) {
	for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
		Player *m_player = Maps::getMap(player->getMap())->getPlayer(i);
		if (m_player->getPartyId() == player->getPartyId() && m_player != player)
			PlayerPacket::showHPBar(m_player, player);
	}
}

void Party::handleRequest(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case 0x01: // Create party
		case 0x02: // Leave party
			WorldServerConnectPacket::partyOperation(ChannelServer::Instance()->getWorldPlayer(), type, player->getId());
			break;
		case 0x03: // Join party
		case 0x05: // Expel Player
		case 0x06: // Give leader rights
			WorldServerConnectPacket::partyOperation(ChannelServer::Instance()->getWorldPlayer(), type, player->getId(), packet.get<int32_t>());
			break;
		case 0x04: // Invite
			WorldServerConnectPacket::partyInvite(ChannelServer::Instance()->getWorldPlayer(), player->getId(), packet.getString());
			break;
	}
}

void Party::handleResponse(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t playerid = packet.get<int32_t>();
	int32_t partyid = packet.get<int32_t>();
	Player *player = Players::Instance()->getPlayer(playerid);
	if (player == 0)
		return;
	switch (type) {
		case 0x01: // Leave / Disband
		case 0x06: // Expel
			if (player->getPartyId() == partyid)
				player->setPartyId(0);
			break;
		case 0x02: // Create / Join
		case 0x05: // LogInLogOut
			if (player->getPartyId() == 0) {
				player->setPartyId(partyid);
				showHPBar(player);
				receiveHPBar(player);
			}
			break;
	}
}
