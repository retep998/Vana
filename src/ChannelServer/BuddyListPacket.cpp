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
#include "BuddyListPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void BuddyListPacket::error(Player *player, uint8_t error) {
	PacketCreator packet;
	packet.addHeader(SMSG_BUDDY);
	packet.add<int8_t>(error);

	player->getSession()->send(packet);
}

void BuddyListPacket::update(Player *player, uint8_t type) {
	PlayerBuddyList *buddyList = player->getBuddyList();
	uint8_t size = buddyList->listSize();

	PacketCreator packet;
	packet.addHeader(SMSG_BUDDY);
	packet.add<int8_t>(type);
	packet.add<uint8_t>(size);

	player->getBuddyList()->addBuddies(packet);

	for (uint8_t i = 0; i < size; i++) {
		packet.add<int32_t>(0);
	}

	player->getSession()->send(packet);
}

void BuddyListPacket::showSize(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_BUDDY);
	packet.add<int8_t>(0x15);
	packet.add<uint8_t>(player->getBuddyListSize());
	player->getSession()->send(packet);
}

void BuddyListPacket::invitation(Player *player, PlayerBuddyList::BuddyInvite &invite) {
	PacketCreator packet;
	packet.addHeader(SMSG_BUDDY);
	packet.add<int8_t>(0x09);
	packet.add<int32_t>(invite.m_id);
	packet.addString(invite.m_name);

	packet.add<int32_t>(invite.m_id);
	packet.addString(invite.m_name, 13);
	packet.add<uint8_t>(OppositeStatus::Requested); // Buddy status
	packet.add<uint32_t>(-1); // Doesn't really matter O.o
	packet.addString("Default Group", 13); // Needs to be set to "Default Group", because it's automatically added.
	packet.add<int8_t>(0x00);
	packet.add<int8_t>(20); // Seems to be the amount of buddy slots for the character...
	packet.add<uint8_t>(0xFD);
	packet.add<uint8_t>(0xBA);

	packet.add<int8_t>(0); // Unknown
	player->getSession()->send(packet);
}

void BuddyListPacket::online(Player *player, int32_t charid, int32_t channel) {
	PacketCreator packet;
	packet.addHeader(SMSG_BUDDY);
	packet.add<int8_t>(0x14);
	packet.add<int32_t>(charid);
	packet.add<int8_t>(0);
	packet.add<int32_t>(channel);
	player->getSession()->send(packet);
}
