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
#include "Pets.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "PetsPacket.h"
#include "Player.h"
#include "PacketReader.h"
#include "SendHeader.h"

void PetsPacket::petSummoned(Player *player, Pet *pet, bool kick, bool onlyPlayer, int8_t index) {
	PacketCreator packet;
	packet.addShort(SEND_PET_SUMMONED);
	packet.addInt(player->getId());
	packet.addByte(index != -1 ? index : pet->getIndex());
	packet.addByte(pet->isSummoned());
	packet.addByte(kick); // Kick existing pet (only when player doesn't have follow the lead)
	if (pet->isSummoned()) {
		packet.addInt(pet->getType());
		packet.addString(pet->getName());
		packet.addInt(pet->getId());
		packet.addInt(0);
		packet.addPos(pet->getPos());
		packet.addByte(pet->getStance());
		packet.addInt(pet->getFH());
	}
	onlyPlayer ? player->getSession()->send(packet) : Maps::getMap(player->getMap())->sendPacket(packet);
}

void PetsPacket::showChat(Player *player, Pet *pet, const string &message, int8_t act) {
	PacketCreator packet;
	packet.addShort(SEND_PET_SHOW_CHAT);
	packet.addInt(player->getId());
	packet.addByte(pet->getIndex());
	packet.addByte(0);
	packet.addByte(act);
	packet.addString(message);
	packet.addByte(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PetsPacket::showMovement(Player *player, Pet *pet, unsigned char *buf, int32_t buflen) {
	PacketCreator packet;
	packet.addShort(SEND_PET_SHOW_MOVING);
	packet.addInt(player->getId());
	packet.addByte(pet->getIndex());
	packet.addBuffer(buf, buflen);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PetsPacket::showAnimation(Player *player, Pet *pet, int8_t animation, bool success) {
	PacketCreator packet;
	packet.addShort(SEND_PET_ANIMATION);
	packet.addInt(player->getId());
	packet.addByte(pet->getIndex()); // Index for multiple pets
	packet.addByte(animation == 1 && success);
	packet.addByte(animation);
	animation == 1 ? packet.addByte(0) : packet.addShort(success);

	player->getSession()->send(packet);
}

void PetsPacket::updatePet(Player *player, Pet *pet) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_ITEM);
	packet.addByte(0);
	packet.addByte(2);
	packet.addByte(3);
	packet.addByte(5);
	packet.addByte(pet->getInventorySlot());
	packet.addShort(0);
	packet.addByte(5);
	packet.addByte(pet->getInventorySlot());
	packet.addByte(0);
	packet.addByte(3);
	packet.addInt(pet->getType());
	packet.addByte(1);
	packet.addInt(pet->getId());
	packet.addInt(0);
	packet.addBytes("008005BB46E61702");
	packet.addString(pet->getName(), 13);
	packet.addByte(pet->getLevel());
	packet.addShort(pet->getCloseness());
	packet.addByte(pet->getFullness());
	packet.addByte(0);
	packet.addBytes("B8D56000CEC8"); // TODO: Expire date
	packet.addByte(1); // Alive or dead
	packet.addInt(0);
	player->getSession()->send(packet);
}

void PetsPacket::levelUp(Player *player, Pet *pet) {
	PacketCreator packet;
	packet.addShort(SEND_GAIN_ITEM);
	packet.addByte(4);
	packet.addByte(0);
	packet.addByte(pet->getIndex());
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.addShort(SEND_SHOW_SKILL);
	packet.addInt(player->getId());
	packet.addShort(4);
	packet.addByte(pet->getIndex());
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PetsPacket::changeName(Player *player, Pet *pet) {
	PacketCreator packet;
	packet.addShort(SEND_PET_NAME_CHANGE);
	packet.addInt(player->getId());
	packet.addByte(pet->getIndex());
	packet.addString(pet->getName());
	packet.addByte(0);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PetsPacket::showPet(Player *player, Pet *pet) {
	PacketCreator packet;
	packet.addShort(SEND_PET_SHOW);
	packet.addInt(player->getId());
	packet.addByte(pet->getIndex());
	packet.addInt(pet->getId());
	packet.addInt(0);
	packet.addByte(0);
	player->getSession()->send(packet);
}

void PetsPacket::updateSummonedPets(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(0);
	packet.addShort(0x8);
	packet.addShort(0x18);
	for (int8_t i = 0; i < 3; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			packet.addInt(pet->getId());
		}
		else {
			packet.addInt(0);
		}
		packet.addInt(0);
	}
	packet.addByte(0);
	player->getSession()->send(packet);
}

void PetsPacket::blankUpdate(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(1);
	packet.addInt(0);
	player->getSession()->send(packet);
}
