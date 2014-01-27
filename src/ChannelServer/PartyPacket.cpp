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
#include "PartyPacket.hpp"
#include "GameConstants.hpp"
#include "InterHelper.hpp"
#include "PacketCreator.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

auto PartyPacket::error(Player *player, int8_t error) -> void {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(error);
	player->getSession()->send(packet);
}

auto PartyPacket::createParty(Player *packetTarget, Party *party) -> void {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x08);
	packet.add<int32_t>(party->getId());
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(0);
	packetTarget->getSession()->send(packet);
}

auto PartyPacket::joinParty(Player *packetTarget, Party *party, const string_t &player) -> void {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x0F);
	packet.add<int32_t>(party->getId());
	packet.addString(player);
	party->updatePacket(packet);
	packetTarget->getSession()->send(packet);
}

auto PartyPacket::leaveParty(Player *packetTarget, Party *party, int32_t playerId, const string_t &name, bool kicked) -> void {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x0C);
	packet.add<int32_t>(party->getId());
	packet.add<int32_t>(playerId);
	packet.add<bool>(kicked);
	packet.addString(name);
	party->updatePacket(packet);
	packetTarget->getSession()->send(packet);
}

auto PartyPacket::invitePlayer(Player *packetTarget, Party *party, const string_t &inviter) -> void {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(party->getId());
	packet.addString(inviter);
	packet.add<int8_t>(0);
	packetTarget->getSession()->send(packet);
}

auto PartyPacket::disbandParty(Player *packetTarget, Party *party) -> void {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x0C);
	packet.add<int32_t>(party->getId());
	packet.add<int32_t>(party->getLeaderId());
	packet.add<int8_t>(0);
	packet.add<int32_t>(party->getId());
	packetTarget->getSession()->send(packet);
}

auto PartyPacket::setLeader(Player *packetTarget, Party *party, int32_t newLeader) -> void {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x1A);
	packet.add<int32_t>(newLeader);
	packet.add<bool>(false);
	packetTarget->getSession()->send(packet);
}

auto PartyPacket::silentUpdate(Player *packetTarget, Party *party) -> void {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_PARTY);
	packet.add<int8_t>(0x07);
	packet.add<int32_t>(party->getId());
	party->updatePacket(packet);
	packetTarget->getSession()->send(packet);
}