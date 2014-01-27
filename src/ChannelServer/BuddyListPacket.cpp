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
#include "BuddyListPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerBuddyList.h"
#include "SmsgHeader.h"

auto BuddyListPacket::error(Player *player, uint8_t error) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_BUDDY);
	packet.add<int8_t>(error);

	player->getSession()->send(packet);
}

auto BuddyListPacket::update(Player *player, uint8_t type) -> void {
	uint8_t size = player->getBuddyList()->listSize();

	PacketCreator packet;
	packet.add<header_t>(SMSG_BUDDY);
	packet.add<int8_t>(type);
	packet.add<uint8_t>(size);

	player->getBuddyList()->addBuddies(packet);

	for (uint8_t i = 0; i < size; i++) {
		packet.add<int32_t>(0);
	}

	player->getSession()->send(packet);
}

auto BuddyListPacket::showSize(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_BUDDY);
	packet.add<int8_t>(0x15);
	packet.add<uint8_t>(player->getBuddyListSize());
	player->getSession()->send(packet);
}

auto BuddyListPacket::invitation(Player *invitee, const BuddyInvite &invite) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_BUDDY);
	packet.add<int8_t>(0x09);
	packet.add<int32_t>(invite.id);
	packet.addString(invite.name);

	packet.add<int32_t>(invite.id);
	packet.addString(invite.name, 13);
	packet.add<uint8_t>(OppositeStatus::Requested); // Buddy status
	packet.add<uint32_t>(-1); // Doesn't really matter O.o
	packet.addString("Default Group", 13); // Needs to be set to "Default Group", because it's automatically added.
	packet.add<int8_t>(0x00);
	packet.add<int8_t>(20); // Seems to be the amount of buddy slots for the character...
	packet.add<uint8_t>(0xFD);
	packet.add<uint8_t>(0xBA);

	packet.add<int8_t>(0); // Unknown
	invitee->getSession()->send(packet);
}

auto BuddyListPacket::online(Player *player, int32_t charId, int32_t channel) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_BUDDY);
	packet.add<int8_t>(0x14);
	packet.add<int32_t>(charId);
	packet.add<int8_t>(0);
	packet.add<int32_t>(channel);
	player->getSession()->send(packet);
}