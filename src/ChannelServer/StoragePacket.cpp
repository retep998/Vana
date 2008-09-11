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
#include "Inventory.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "SendHeader.h"

void StoragePacket::showStorage(Player *player, int npcid) {
	PacketCreator packet;
	packet.addShort(SEND_STORAGE_ACTION);
	packet.addByte(0x15); // Type of storage action
	packet.addInt(npcid);
	packet.addByte(player->getStorage()->getSlots());
	packet.addInt(0x7e);
	packet.addInt(0);
	packet.addInt(player->getStorage()->getMesos());
	packet.addShort(0);
	packet.addByte(player->getStorage()->getNumItems());
	for (char i = 0; i < player->getStorage()->getNumItems(); i++) {
		PlayerPacketHelper::addItemInfo(packet, 0, player->getStorage()->getItem(i));
	}
	packet.addShort(0);
	packet.addByte(0);
	player->getSession()->send(packet);
}

void StoragePacket::addItem(Player *player, char inv) {
	PacketCreator packet;
	packet.addShort(SEND_STORAGE_ACTION);
	packet.addByte(0x0c);
	packet.addByte(player->getStorage()->getSlots());
	char type = (char) pow((float) 2, (int) inv) * 2; // Gotta work some magic on type, which starts as inventory
	packet.addInt(type);
	packet.addInt(0);
	packet.addByte(player->getStorage()->getNumItems(inv));
	for (char i = 0; i < player->getStorage()->getNumItems(); i++) {
		Item *item = player->getStorage()->getItem(i);
		if (GETINVENTORY(item->id) == inv)
			PlayerPacketHelper::addItemInfo(packet, 0, item);
	}
	player->getSession()->send(packet);
}

void StoragePacket::takeItem(Player *player, char inv) {
	PacketCreator packet;
	packet.addShort(SEND_STORAGE_ACTION);
	packet.addByte(0x09);
	packet.addByte(player->getStorage()->getSlots());
	char type = (char) pow((float) 2, (int) inv) * 2;
	packet.addInt(type);
	packet.addInt(0);
	packet.addByte(player->getStorage()->getNumItems(inv));
	for (char i = 0; i < player->getStorage()->getNumItems(); i++) {
		Item *item = player->getStorage()->getItem(i);
		if (GETINVENTORY(item->id) == inv)
			PlayerPacketHelper::addItemInfo(packet, 0, item);
	}
	player->getSession()->send(packet);
}

void StoragePacket::changeMesos(Player *player, int mesos) {
	PacketCreator packet;
	packet.addShort(SEND_STORAGE_ACTION);
	packet.addByte(0x12);
	packet.addByte(player->getStorage()->getSlots());
	packet.addShort(2);
	packet.addShort(0);
	packet.addInt(0);
	packet.addInt(mesos);
	player->getSession()->send(packet);
}

void StoragePacket::storageFull(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_STORAGE_ACTION);
	packet.addByte(0x10);
	player->getSession()->send(packet);
}
