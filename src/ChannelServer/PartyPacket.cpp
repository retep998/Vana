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
#include "PartyPacket.h"
#include "GameConstants.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "Session.h"
#include "SmsgHeader.h"

void PartyPacket::error(Player *player, int8_t error) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(error);
	player->getSession()->send(packet);
}

void PartyPacket::createParty(Player *packetTarget, Party *party) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x08);
	packet.add<int32_t>(party->getId());
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(0);
	packetTarget->getSession()->send(packet);
}

void PartyPacket::joinParty(Player *packetTarget, Party *party, const string &player) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x0F);
	packet.add<int32_t>(party->getId());
	packet.addString(player);
	party->updatePacket(packet);
	packetTarget->getSession()->send(packet);
}

void PartyPacket::leaveParty(Player *packetTarget, Party *party, int32_t playerId, const string &name, bool kicked) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x0C);
	packet.add<int32_t>(party->getId());
	packet.add<int32_t>(playerId);
	packet.addBool(kicked);
	packet.addString(name);
	party->updatePacket(packet);
	packetTarget->getSession()->send(packet);
}

void PartyPacket::invitePlayer(Player *packetTarget, Party *party, const string &inviter) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(party->getId());
	packet.addString(inviter);
	packet.add<int8_t>(0);
	packetTarget->getSession()->send(packet);
}

void PartyPacket::disbandParty(Player *packetTarget, Party *party) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x0C);
	packet.add<int32_t>(party->getId());
	packet.add<int32_t>(party->getLeaderId());
	packet.add<int8_t>(0);
	packet.add<int32_t>(party->getId());
	packetTarget->getSession()->send(packet);
}

void PartyPacket::setLeader(Player *packetTarget, Party *party, int32_t newLeader) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x1A);
	packet.add<int32_t>(newLeader);
	packet.addBool(false);
	packetTarget->getSession()->send(packet);
}

void PartyPacket::silentUpdate(Player *packetTarget, Party *party) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x07);
	packet.add<int32_t>(party->getId());
	party->updatePacket(packet);
	packetTarget->getSession()->send(packet);
}