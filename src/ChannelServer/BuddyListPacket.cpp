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
	packet.add<int16_t>(SMSG_BUDDY);
	packet.add<int8_t>(error);

	player->getSession()->send(packet);
}

void BuddyListPacket::update(Player *player, uint8_t type) {
	PlayerBuddyList *buddyList = player->getBuddyList();
	uint8_t size = buddyList->size();

	PacketCreator packet;
	packet.add<int16_t>(SMSG_BUDDY);
	packet.add<int8_t>(type);
	packet.add<int8_t>(size);

	for (uint8_t i = 0; i < size; i++) {
		PlayerBuddyList::BuddyPtr buddy = buddyList->getBuddy(i);
		packet.add<int32_t>(buddy->charid);
		packet.addString(buddy->name, 13);
		packet.add<int8_t>(buddy->oppositeStatus);
		packet.add<int32_t>(buddy->channel);
		packet.addString("Default Group", 13);
		packet.add<int32_t>(0);
	}

	for (uint8_t i = 0; i < size; i++) {
		packet.add<int32_t>(0);
	}

	player->getSession()->send(packet);
}

void BuddyListPacket::showSize(Player *player) {
	uint8_t size = static_cast<uint8_t>(player->getBuddyListSize());
	PacketCreator packet;
	packet.add<int16_t>(SMSG_BUDDY);
	packet.add<int8_t>(0x15);
	packet.add<int8_t>(size);
	player->getSession()->send(packet);
}