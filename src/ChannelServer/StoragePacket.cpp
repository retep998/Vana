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
#include "StoragePacket.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "Session.h"
#include "SmsgHeader.h"

auto StoragePacket::showStorage(Player *player, int32_t npcId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_STORAGE);
	packet.add<int8_t>(0x16); // Type of storage action
	packet.add<int32_t>(npcId);
	packet.add<int8_t>(player->getStorage()->getSlots());
	packet.add<int32_t>(0x7e);
	packet.add<int32_t>(0);
	packet.add<int32_t>(player->getStorage()->getMesos());
	packet.add<int16_t>(0);
	packet.add<int8_t>(player->getStorage()->getNumItems());
	for (int8_t i = 0; i < player->getStorage()->getNumItems(); i++) {
		PlayerPacketHelper::addItemInfo(packet, 0, player->getStorage()->getItem(i));
	}
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

auto StoragePacket::addItem(Player *player, int8_t inv) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_STORAGE);
	packet.add<int8_t>(0x0d);
	packet.add<int8_t>(player->getStorage()->getSlots());
	int8_t type = static_cast<int8_t>(pow(2.f, static_cast<int32_t>(inv))) * 2; // Gotta work some magic on type, which starts as inventory
	packet.add<int32_t>(type);
	packet.add<int32_t>(0);
	packet.add<int8_t>(player->getStorage()->getNumItems(inv));
	for (int8_t i = 0; i < player->getStorage()->getNumItems(); i++) {
		Item *item = player->getStorage()->getItem(i);
		if (GameLogicUtilities::getInventory(item->getId()) == inv) {
			PlayerPacketHelper::addItemInfo(packet, 0, item);
		}
	}
	player->getSession()->send(packet);
}

auto StoragePacket::takeItem(Player *player, int8_t inv) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_STORAGE);
	packet.add<int8_t>(0x09);
	packet.add<int8_t>(player->getStorage()->getSlots());
	int8_t type = static_cast<int8_t>(pow(2.f, static_cast<int32_t>(inv))) * 2;
	packet.add<int32_t>(type);
	packet.add<int32_t>(0);
	packet.add<int8_t>(player->getStorage()->getNumItems(inv));
	for (int8_t i = 0; i < player->getStorage()->getNumItems(); i++) {
		Item *item = player->getStorage()->getItem(i);
		if (GameLogicUtilities::getInventory(item->getId()) == inv) {
			PlayerPacketHelper::addItemInfo(packet, 0, item);
		}
	}
	player->getSession()->send(packet);
}

auto StoragePacket::changeMesos(Player *player, int32_t mesos) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_STORAGE);
	packet.add<int8_t>(0x13);
	packet.add<int8_t>(player->getStorage()->getSlots());
	packet.add<int16_t>(2);
	packet.add<int16_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(mesos);
	player->getSession()->send(packet);
}

auto StoragePacket::storageFull(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_STORAGE);
	packet.add<int8_t>(0x11);
	player->getSession()->send(packet);
}

auto StoragePacket::noMesos(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_STORAGE);
	packet.add<int8_t>(0x10);
	player->getSession()->send(packet);
}