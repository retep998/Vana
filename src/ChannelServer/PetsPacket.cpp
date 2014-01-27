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
#include "PetsPacket.hpp"
#include "GameConstants.hpp"
#include "Item.hpp"
#include "ItemConstants.hpp"
#include "Maps.hpp"
#include "PacketCreator.hpp"
#include "PacketReader.hpp"
#include "Pet.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

auto PetsPacket::petSummoned(Player *player, Pet *pet, bool kick, bool onlyPlayer, int8_t index) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PET_SUMMON);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(index != -1 ? index : (pet->isSummoned() ? pet->getIndex().get() : -1));
	packet.add<bool>(pet->isSummoned());
	packet.add<bool>(kick); // Kick existing pet (only when player doesn't have follow the lead)
	if (pet->isSummoned()) {
		packet.add<int32_t>(pet->getItemId());
		packet.addString(pet->getName());
		packet.add<int64_t>(pet->getId());
		packet.addClass<Pos>(pet->getPos());
		packet.add<int8_t>(pet->getStance());
		packet.add<int16_t>(pet->getFoothold());
		packet.add<bool>(pet->hasNameTag());
		packet.add<bool>(pet->hasQuoteItem());
	}
	if (onlyPlayer) {
		player->getSession()->send(packet);
	}
	else {
		player->getMap()->sendPacket(packet);
	}
}

auto PetsPacket::showChat(Player *player, Pet *pet, const string_t &message, int8_t act) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PET_MESSAGE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	packet.add<int8_t>(0);
	packet.add<int8_t>(act);
	packet.addString(message);
	packet.add<bool>(pet->hasQuoteItem());
	player->getMap()->sendPacket(packet, player);
}

auto PetsPacket::showMovement(Player *player, Pet *pet, unsigned char *buf, int32_t bufLen) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PET_MOVEMENT);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	packet.addBuffer(buf, bufLen);
	player->getMap()->sendPacket(packet, player);
}

auto PetsPacket::showAnimation(Player *player, Pet *pet, int8_t animation) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PET_ANIMATION);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	packet.add<bool>(animation == 1);
	packet.add<int8_t>(animation);
	packet.add<int8_t>(0);
	packet.add<bool>(pet->hasQuoteItem());
	player->getSession()->send(packet);
}

auto PetsPacket::updatePet(Player *player, Pet *pet, Item *petItem) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_INVENTORY_OPERATION);
	packet.add<int8_t>(0);
	packet.add<int8_t>(2);
	packet.add<int8_t>(3);
	packet.add<int8_t>(5);
	packet.add<int16_t>(pet->getInventorySlot());
	packet.add<int8_t>(0);
	packet.add<int8_t>(5);
	packet.add<int16_t>(pet->getInventorySlot());
	addInfo(packet, pet, petItem);
	player->getSession()->send(packet);
}

auto PetsPacket::levelUp(Player *player, Pet *pet) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_THEATRICS);
	packet.add<int8_t>(4);
	packet.add<int8_t>(0);
	packet.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<header_t>(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int16_t>(4);
	packet.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	player->getMap()->sendPacket(packet);
}

auto PetsPacket::changeName(Player *player, Pet *pet) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PET_NAME_CHANGE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	packet.addString(pet->getName());
	packet.add<bool>(pet->hasNameTag());
	player->getMap()->sendPacket(packet);
}

auto PetsPacket::showPet(Player *player, Pet *pet) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PET_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	packet.add<int64_t>(pet->getId());
	packet.add<bool>(pet->hasNameTag());
	player->getSession()->send(packet);
}

auto PetsPacket::updateSummonedPets(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_UPDATE);
	packet.add<int8_t>(0);
	packet.add<int16_t>(Stats::Pet);
	packet.add<int16_t>(0x18);
	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			packet.add<int64_t>(pet->getId());
		}
		else {
			packet.add<int64_t>(0);
		}
	}
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

auto PetsPacket::blankUpdate(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_UPDATE);
	packet.add<int8_t>(1);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

auto PetsPacket::addInfo(PacketCreator &packet, Pet *pet, Item *petItem) -> void {
	packet.add<int8_t>(3);
	packet.add<int32_t>(pet->getItemId());
	packet.add<int8_t>(1);
	packet.add<int64_t>(pet->getId());
	packet.add<int64_t>(0LL);
	packet.addString(pet->getName(), 13);
	packet.add<int8_t>(pet->getLevel());
	packet.add<int16_t>(pet->getCloseness());
	packet.add<int8_t>(pet->getFullness());
	packet.add<int64_t>(petItem->getExpirationTime());
	packet.add<int32_t>(0);
	packet.add<int32_t>(0); // Time to expire (for trial pet)
}