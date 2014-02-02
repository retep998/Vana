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
#include "PacketReader.hpp"
#include "Pet.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace PetsPacket {

SPLIT_PACKET_IMPL(petSummoned, int32_t playerId, Pet *pet, bool kick, int8_t index) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_PET_SUMMON)
		.add<int32_t>(playerId)
		.add<int8_t>(index != -1 ? index : (pet->isSummoned() ? pet->getIndex().get() : -1))
		.add<bool>(pet->isSummoned())
		.add<bool>(kick); // Kick existing pet (only when player doesn't have follow the lead)

	if (pet->isSummoned()) {
		builder.player
			.add<int32_t>(pet->getItemId())
			.add<string_t>(pet->getName())
			.add<int64_t>(pet->getId())
			.add<Pos>(pet->getPos())
			.add<int8_t>(pet->getStance())
			.add<int16_t>(pet->getFoothold())
			.add<bool>(pet->hasNameTag())
			.add<bool>(pet->hasQuoteItem());
	}

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(showChat, int32_t playerId, Pet *pet, const string_t &message, int8_t act) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_PET_MESSAGE)
		.add<int32_t>(playerId)
		.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1)
		.add<int8_t>(0)
		.add<int8_t>(act)
		.add<string_t>(message)
		.add<bool>(pet->hasQuoteItem());
	return builder;
}

SPLIT_PACKET_IMPL(showMovement, int32_t playerId, Pet *pet, unsigned char *buf, int32_t bufLen) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_PET_MOVEMENT)
		.add<int32_t>(playerId)
		.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1)
		.addBuffer(buf, bufLen);
	return builder;
}

PACKET_IMPL(showAnimation, int32_t playerId, Pet *pet, int8_t animation) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PET_ANIMATION)
		.add<int32_t>(playerId)
		.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1)
		.add<bool>(animation == 1)
		.add<int8_t>(animation)
		.add<int8_t>(0)
		.add<bool>(pet->hasQuoteItem());
	return builder;
}

PACKET_IMPL(updatePet, Pet *pet, Item *petItem) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_INVENTORY_OPERATION)
		.add<int8_t>(0)
		.add<int8_t>(2)
		.add<int8_t>(3)
		.add<int8_t>(5)
		.add<int16_t>(pet->getInventorySlot())
		.add<int8_t>(0)
		.add<int8_t>(5)
		.add<int16_t>(pet->getInventorySlot())
		.addBuffer(addInfo(pet, petItem));
	return builder;
}

SPLIT_PACKET_IMPL(levelUp, int32_t playerId, Pet *pet) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet
		.add<int16_t>(4)
		.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);

	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.addBuffer(packet);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<int32_t>(playerId)
		.addBuffer(packet);
	return builder;
}

SPLIT_PACKET_IMPL(changeName, int32_t playerId, Pet *pet) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_PET_NAME_CHANGE)
		.add<int32_t>(playerId)
		.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1)
		.add<string_t>(pet->getName())
		.add<bool>(pet->hasNameTag());

	builder.map.addBuffer(builder.player);
	return builder;
}
/* TODO FIXME packet
auto showPet(Player *player, Pet *pet) -> void {
	PacketBuilder packet;
	packet.add<header_t>(SMSG_PET_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	packet.add<int64_t>(pet->getId());
	packet.add<bool>(pet->hasNameTag());
	player->send(packet);
}*/

PACKET_IMPL(updateSummonedPets, Player *player) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_UPDATE)
		.add<int8_t>(0)
		.add<int16_t>(Stats::Pet)
		.add<int16_t>(0x18);

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			builder.add<int64_t>(pet->getId());
		}
		else {
			builder.add<int64_t>(0);
		}
	}
	builder.add<int8_t>(0);
	return builder;
}

PACKET_IMPL(blankUpdate) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_UPDATE)
		.add<int8_t>(1)
		.add<int32_t>(0);
	return builder;
}

PACKET_IMPL(addInfo, Pet *pet, Item *petItem) {
	PacketBuilder builder;
	builder
		.add<int8_t>(3)
		.add<int32_t>(pet->getItemId())
		.add<int8_t>(1)
		.add<int64_t>(pet->getId())
		.add<int64_t>(0LL)
		.add<string_t>(pet->getName(), 13)
		.add<int8_t>(pet->getLevel())
		.add<int16_t>(pet->getCloseness())
		.add<int8_t>(pet->getFullness())
		.add<int64_t>(petItem->getExpirationTime())
		.add<int32_t>(0)
		.add<int32_t>(0); // Time to expire (for trial pet)
	return builder;
}

}