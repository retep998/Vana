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
#include "PetsPacket.h"
#include "GameConstants.h"
#include "ItemConstants.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Pet.h"
#include "Player.h"
#include "SendHeader.h"

void PetsPacket::petSummoned(Player *player, Pet *pet, bool kick, bool onlyPlayer, int8_t index) {
	PacketCreator packet;
	packet.addHeader(SMSG_PET_SUMMON);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(index != -1 ? index : pet->getIndex());
	packet.addBool(pet->isSummoned());
	packet.addBool(kick); // Kick existing pet (only when player doesn't have follow the lead)
	if (pet->isSummoned()) {
		packet.add<int32_t>(pet->getItemId());
		packet.addString(pet->getName());
		packet.add<int64_t>(pet->getId());
		packet.addPos(pet->getPos());
		packet.add<int8_t>(pet->getStance());
		packet.add<int16_t>(pet->getFh());
		packet.addBool(pet->hasNameTag());
		packet.addBool(pet->hasQuoteItem());
	}
	if (onlyPlayer) {
		player->getSession()->send(packet);
	}
	else {
		Maps::getMap(player->getMap())->sendPacket(packet);
	}
}

void PetsPacket::showChat(Player *player, Pet *pet, const string &message, int8_t act) {
	PacketCreator packet;
	packet.addHeader(SMSG_PET_MESSAGE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->getIndex());
	packet.add<int8_t>(0);
	packet.add<int8_t>(act);
	packet.addString(message);
	packet.addBool(pet->hasQuoteItem());
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PetsPacket::showMovement(Player *player, Pet *pet, unsigned char *buf, int32_t buflen) {
	PacketCreator packet;
	packet.addHeader(SMSG_PET_MOVEMENT);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->getIndex());
	packet.addBuffer(buf, buflen);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PetsPacket::showAnimation(Player *player, Pet *pet, int8_t animation) {
	PacketCreator packet;
	packet.addHeader(SMSG_PET_ANIMATION);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->getIndex()); // Index for multiple pets
	packet.addBool(animation == 1);
	packet.add<int8_t>(animation);
	packet.add<int8_t>(0); // Unknown
	packet.addBool(pet->hasQuoteItem());
	player->getSession()->send(packet);
}

void PetsPacket::updatePet(Player *player, Pet *pet) {
	PacketCreator packet;
	packet.addHeader(SMSG_INVENTORY_ITEM_MOVE);
	packet.add<int8_t>(0);
	packet.add<int8_t>(2);
	packet.add<int8_t>(3);
	packet.add<int8_t>(5);
	packet.add<int8_t>(pet->getInventorySlot());
	packet.add<int16_t>(0);
	packet.add<int8_t>(5);
	packet.add<int8_t>(pet->getInventorySlot());
	packet.add<int8_t>(0);
	addInfo(packet, pet);
	player->getSession()->send(packet);
}

void PetsPacket::levelUp(Player *player, Pet *pet) {
	PacketCreator packet;
	packet.addHeader(SMSG_THEATRICS);
	packet.add<int8_t>(4);
	packet.add<int8_t>(0);
	packet.add<int8_t>(pet->getIndex());
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int16_t>(4);
	packet.add<int8_t>(pet->getIndex());
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PetsPacket::changeName(Player *player, Pet *pet) {
	PacketCreator packet;
	packet.addHeader(SMSG_PET_NAME_CHANGE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->getIndex());
	packet.addString(pet->getName());
	packet.addBool(pet->hasNameTag());
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PetsPacket::showPet(Player *player, Pet *pet) {
	PacketCreator packet;
	packet.addHeader(SMSG_PET_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->getIndex());
	packet.add<int64_t>(pet->getId());
	packet.addBool(pet->hasNameTag());
	player->getSession()->send(packet);
}

void PetsPacket::updateSummonedPets(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_UPDATE);
	packet.add<int8_t>(0);
	packet.add<int16_t>(Stats::Pet);
	packet.add<int16_t>(0x18);
	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			packet.add<int32_t>(pet->getId());
		}
		else {
			packet.add<int32_t>(0);
		}
		packet.add<int32_t>(0);
	}
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void PetsPacket::blankUpdate(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_UPDATE);
	packet.add<int8_t>(1);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void PetsPacket::addInfo(PacketCreator &packet, Pet *pet) {
	packet.add<int8_t>(3);
	packet.add<int32_t>(pet->getItemId());
	packet.add<int8_t>(1);
	packet.add<int64_t>(pet->getId());
	packet.add<int64_t>(Items::NoExpiration);
	packet.addString(pet->getName(), 13);
	packet.add<int8_t>(pet->getLevel());
	packet.add<int16_t>(pet->getCloseness());
	packet.add<int8_t>(pet->getFullness());
	packet.add<int64_t>(0LL);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0); // Time to expire (for trial pet)
}