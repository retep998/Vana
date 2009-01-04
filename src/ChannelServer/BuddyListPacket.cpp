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
#include "BuddyListPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void BuddyListPacket::error(Player *player, uint8_t error) {
	PacketCreator packet;
	packet.addShort(SEND_BUDDYLIST);
	packet.addByte(error);
	
	player->getSession()->send(packet);
}

void BuddyListPacket::update(Player *player, uint8_t type) {
	PlayerBuddyList *buddyList = player->getBuddyList();
	uint8_t size = buddyList->size();

	PacketCreator packet;
	packet.addShort(SEND_BUDDYLIST);
	packet.addByte(type);
	packet.addByte(size);

	for (uint8_t i = 0; i < size; i++) {
		PlayerBuddyList::BuddyPtr buddy = buddyList->getBuddy(i);
		packet.addInt(buddy->charid);
		packet.addString(buddy->name, 13);
		packet.addByte(buddy->oppositeStatus);
		packet.addInt(buddy->channel);
	}

	for (uint8_t i = 0; i < size; i++) {
		packet.addInt(0);
	}

	player->getSession()->send(packet);
}
