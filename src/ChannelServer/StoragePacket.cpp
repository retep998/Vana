/*
Copyright (C) 2008 Vana Development Team

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
#include "StoragePacket.h"
#include "PacketCreator.h"
#include "SendHeader.h"
#include "Player.h"
#include "PlayerPacketHelper.h"

void StoragePacket::showStorage(Player *player, int npcid) {
	PacketCreator packet;
	packet.addShort(SEND_STORAGE_ACTION);
	packet.addByte(0x15); // Type of storage action
	packet.addInt(npcid);
	packet.addByte(player->storage->getSlots());
	packet.addShort(0x7e);
	packet.addShort(0);
	packet.addInt(0);
	packet.addInt(player->storage->getMesos());
	packet.addShort(0);
	packet.addByte(player->storage->getNumItems());
	for (char i = 0; i < player->storage->getNumItems(); i++) {
		PlayerPacketHelper::addItemInfo(packet, 0, player->storage->getItem(i));
	}
	packet.addShort(0);
	packet.addByte(0);
	packet.send(player);
}

void StoragePacket::addItem(Player *player, Item *item) {
	PacketCreator packet;
	packet.addShort(SEND_STORAGE_ACTION);
	packet.addByte(0x0c);
	packet.addByte(player->storage->getSlots());
}

void StoragePacket::takeItem(Player *player, short slot) {
}

void StoragePacket::changeMesos(Player *player, int mesos) {
	PacketCreator packet;
	packet.addShort(SEND_STORAGE_ACTION);
	packet.addByte(0x12);
	packet.addByte(player->storage->getSlots());
	packet.addShort(2);
	packet.addShort(0);
	packet.addInt(0);
	packet.addInt(mesos);
	packet.send(player);
}
