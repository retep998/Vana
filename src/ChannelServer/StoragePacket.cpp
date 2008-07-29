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
#include "Inventory.h"

void StoragePacket::showStorage(Player *player, int npcid) {
	PacketCreator packet;
	packet.addShort(SEND_STORAGE_ACTION);
	packet.addByte(0x15);
	packet.addInt(npcid);
	packet.addByte(player->storage->getSlots());
	packet.addShort(0x7e);
	packet.addShort(0);
	packet.addInt(0);
	packet.addInt(player->storage->getMesos());
	packet.addBytes("000000");
	vector <Item *> items = player->storage->getItems();
	packet.addByte((char) items.size());
	for (size_t i = 0; i < items.size(); i++) {
		if (ISEQUIP(items[i]->id))
			PlayerPacketHelper::addEquip(packet, i, items[i]);
		else
			PlayerPacketHelper::addItem(packet, i, items[i], false);
	}
	packet.addByte(0);
	packet.send(player);
}

void StoragePacket::addItem(Player *player, Item *item) {
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
