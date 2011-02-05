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
#include "DropsPacket.h"
#include "Drop.h"
#include "GameLogicUtilities.h"
#include "ItemConstants.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void DropsPacket::showDrop(Player *player, Drop *drop, int8_t type, bool newdrop, const Pos &origin) {
	PacketCreator packet;
	packet.addHeader(SMSG_DROP_ITEM);
	packet.add<int8_t>(type);
	packet.add<int32_t>(drop->getId());
	packet.addBool(drop->isMesos());
	packet.add<int32_t>(drop->getObjectId());
	packet.add<int32_t>(drop->getOwner()); // Owner of drop
	packet.add<int8_t>(drop->getType());
	packet.addPos(drop->getPos());
	packet.add<int32_t>(drop->getTime());
	if (type != DropTypes::ShowExisting) {
		// Give the point of origin for things that are just being dropped
		packet.addPos(origin);
		packet.add<int16_t>(0);
	}
	if (!drop->isMesos()) {
		packet.add<int64_t>(Items::NoExpiration);
	}
	packet.addBool(!drop->isPlayerDrop()); // Determines whether pets can pick item up or not

	if (player != nullptr) {
		player->getSession()->send(packet);
	}
	else {
		Maps::getMap(drop->getMap())->sendPacket(packet);
	}
	if (newdrop) {
		showDrop(player, drop, DropTypes::ShowDrop, false, origin);
	}
}

void DropsPacket::takeDrop(Player *player, Drop *drop, int8_t pet_index) {
	PacketCreator packet;
	packet.addHeader(SMSG_DROP_PICKUP);
	packet.add<int8_t>(pet_index != -1 ? 5 : 2);
	packet.add<int32_t>(drop->getId());
	packet.add<int32_t>(player->getId());
	if (pet_index != -1) {
		packet.add<int8_t>(pet_index);
	}

	if (!drop->isQuest()) {
		Maps::getMap(drop->getMap())->sendPacket(packet);
	}
	else {
		player->getSession()->send(packet);
	}
}

void DropsPacket::dontTake(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_INVENTORY_ITEM_MOVE);
	packet.add<int16_t>(1);
	player->getSession()->send(packet);
}

void DropsPacket::removeDrop(Drop *drop) {
	PacketCreator packet;
	packet.addHeader(SMSG_DROP_PICKUP);
	packet.add<int8_t>(0);
	packet.add<int32_t>(drop->getId());
	Maps::getMap(drop->getMap())->sendPacket(packet);
}

void DropsPacket::explodeDrop(Drop *drop) {
	PacketCreator packet;
	packet.addHeader(SMSG_DROP_PICKUP);
	packet.add<int8_t>(4);
	packet.add<int32_t>(drop->getId());
	packet.add<int16_t>(655);
	Maps::getMap(drop->getMap())->sendPacket(packet);
}

void DropsPacket::dropNotAvailableForPickup(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(0);
	packet.add<int8_t>(-2);
	player->getSession()->send(packet);
}

void DropsPacket::cantGetAnymoreItems(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(0);
	packet.add<int8_t>(-1);
	player->getSession()->send(packet);
}

void DropsPacket::pickupDrop(Player *player, int32_t id, int32_t amount, bool isMesos, int16_t cafeBonus) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(0);
	packet.addBool(isMesos);
	packet.add<int32_t>(id);

	if (isMesos)  {
		packet.add<int16_t>(cafeBonus);
	}
	else if (GameLogicUtilities::getInventory(id) != Inventories::EquipInventory) {
		packet.add<int16_t>(static_cast<int16_t>(amount));
	}
	if (!isMesos) {
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
	}

	player->getSession()->send(packet);
}

void DropsPacket::pickupDropSpecial(Player *player, int32_t id) {
	// This packet is used for PQ drops (maybe, got it from the Wing of the Wind item) and monster cards
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(0);
	packet.add<int8_t>(2);
	packet.add<int32_t>(id);
	player->getSession()->send(packet);
}