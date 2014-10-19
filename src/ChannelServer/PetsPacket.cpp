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

SPLIT_PACKET_IMPL(petSummoned, player_id_t playerId, Pet *pet, bool kick, int8_t index) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_PET_SUMMON)
		.add<player_id_t>(playerId)
		.add<int8_t>(index != -1 ? index : (pet->isSummoned() ? pet->getIndex().get() : -1))
		.add<bool>(pet->isSummoned())
		.add<bool>(kick); // Kick existing pet (only when player doesn't have follow the lead)

	if (pet->isSummoned()) {
		builder.player
			.add<item_id_t>(pet->getItemId())
			.add<string_t>(pet->getName())
			.add<pet_id_t>(pet->getId())
			.add<Point>(pet->getPos())
			.add<int8_t>(pet->getStance())
			.add<foothold_id_t>(pet->getFoothold())
			.add<bool>(pet->hasNameTag())
			.add<bool>(pet->hasQuoteItem());
	}

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(showChat, player_id_t playerId, Pet *pet, const string_t &message, int8_t act) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_PET_MESSAGE)
		.add<player_id_t>(playerId)
		.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1)
		.unk<int8_t>()
		.add<int8_t>(act)
		.add<string_t>(message)
		.add<bool>(pet->hasQuoteItem());
	return builder;
}

SPLIT_PACKET_IMPL(showMovement, player_id_t playerId, Pet *pet, unsigned char *buf, int32_t bufLen) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_PET_MOVEMENT)
		.add<player_id_t>(playerId)
		.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1)
		.addBuffer(buf, bufLen);
	return builder;
}

PACKET_IMPL(showAnimation, player_id_t playerId, Pet *pet, int8_t animation) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PET_ANIMATION)
		.add<player_id_t>(playerId)
		.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1)
		.add<bool>(animation == 1)
		.add<int8_t>(animation)
		.unk<int8_t>()
		.add<bool>(pet->hasQuoteItem());
	return builder;
}

PACKET_IMPL(updatePet, Pet *pet, Item *petItem) {
	// TODO FIXME remove this and add addInfo packet to addItemInfo and then modify the inventory update packet to support pets
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_INVENTORY_OPERATION)
		.add<int8_t>(0)
		.add<int8_t>(2)
		.add<int8_t>(3)
		.add<int8_t>(5)
		.add<inventory_slot_t>(pet->getInventorySlot())
		.add<int8_t>(0)
		.add<int8_t>(5)
		.add<inventory_slot_t>(pet->getInventorySlot())
		.addBuffer(addInfo(pet, petItem));
	return builder;
}

SPLIT_PACKET_IMPL(levelUp, player_id_t playerId, Pet *pet) {
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
		.add<player_id_t>(playerId)
		.addBuffer(packet);
	return builder;
}

SPLIT_PACKET_IMPL(changeName, player_id_t playerId, Pet *pet) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_PET_NAME_CHANGE)
		.add<player_id_t>(playerId)
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
	packet.add<player_id_t>(player->getId());
	packet.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	packet.add<pet_id_t>(pet->getId());
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
			builder.add<pet_id_t>(pet->getId());
		}
		else {
			builder.add<pet_id_t>(0);
		}
	}
	builder.unk<int8_t>();
	return builder;
}

PACKET_IMPL(blankUpdate) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_UPDATE)
		.add<int8_t>(1)
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(addInfo, Pet *pet, Item *petItem) {
	PacketBuilder builder;
	builder
		.add<int8_t>(3)
		.add<item_id_t>(pet->getItemId())
		.add<int8_t>(1)
		.add<pet_id_t>(pet->getId())
		.add<int64_t>(0LL)
		.add<string_t>(pet->getName(), 13)
		.add<int8_t>(pet->getLevel())
		.add<int16_t>(pet->getCloseness())
		.add<int8_t>(pet->getFullness())
		.add<int64_t>(petItem->getExpirationTime())
		.unk<int32_t>()
		.add<int32_t>(0); // Time to expire (for trial pet)
	return builder;
}

}