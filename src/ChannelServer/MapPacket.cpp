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
#include "MapPacket.hpp"
#include "Buffs.hpp"
#include "ChannelServer.hpp"
#include "Inventory.hpp"
#include "Map.hpp"
#include "Maps.hpp"
#include "Mist.hpp"
#include "MysticDoor.hpp"
#include "Pet.hpp"
#include "Player.hpp"
#include "PlayerPacketHelper.hpp"
#include "Point.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "TimeUtilities.hpp"
#include "WidePoint.hpp"
#include <unordered_map>

namespace MapPacket {

PACKET_IMPL(playerPacket, Player *player) {
	PacketBuilder builder;
	auto &enter = player->getActiveBuffs()->getMapEntryBuffs();

	builder
		.add<header_t>(SMSG_MAP_PLAYER_SPAWN)
		.add<player_id_t>(player->getId())
		.add<string_t>(player->getName())
		.add<string_t>("") // Guild
		.add<int16_t>(0) // Guild icon garbage
		.add<int8_t>(0) // Guild icon garbage
		.add<int16_t>(0) // Guild icon garbage
		.add<int8_t>(0) // Guild icon garbage
		.add<int32_t>(0)
		.add<uint8_t>(0xf8)
		.add<int8_t>(3)
		.add<int16_t>(0);

	{
		using namespace BuffBytes;
		builder
			.add<uint8_t>(enter.types[Byte5])
			.add<uint8_t>(enter.types[Byte6])
			.add<uint8_t>(enter.types[Byte7])
			.add<uint8_t>(enter.types[Byte8])
			.add<uint8_t>(enter.types[Byte1])
			.add<uint8_t>(enter.types[Byte2])
			.add<uint8_t>(enter.types[Byte3])
			.add<uint8_t>(enter.types[Byte4]);

		const int8_t byteorder[EntryByteQuantity] = { Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8 };

		for (int8_t i = 0; i < EntryByteQuantity; i++) {
			int8_t cbyte = byteorder[i]; // Values are sorted by lower bytes first
			if (enter.types[cbyte] != 0) {
				for (const auto &kvp : enter.values[cbyte]) {
					const MapEntryVals &info = kvp.second;
					if (info.debuff) {
						if (!(kvp.first == 0x01 && cbyte == Byte5)) {
							// Glitch in global, Slow doesn't display properly and if you try, it error 38s
							builder
								.add<int16_t>(info.skill)
								.add<int16_t>(info.val);
						}
					}
					else if (info.use) {
						int16_t value = info.val;
						if (cbyte == Byte3) {
							if (kvp.first == 0x20) {
								builder.add<int8_t>(player->getActiveBuffs()->getCombo() + 1);
							}
							if (kvp.first == 0x40) {
								builder.add<skill_id_t>(player->getActiveBuffs()->getCharge());
							}
						}
						else if (cbyte == Byte5) {
							builder.add<int16_t>(value);
						}
						else {
							builder.add<int8_t>(static_cast<int8_t>(value));
						}
					}
				}
			}
		}
	}

	int32_t unk = 1065638850;
	builder
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int16_t>(0)
		.add<int32_t>(unk)
		.add<int16_t>(0)
		.add<int8_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(unk)
		.add<int16_t>(0)
		.add<int8_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(unk)
		.add<int16_t>(0)
		.add<int8_t>(0)
		.add<item_id_t>(enter.mountId) // No point to having an if, these are 0 when not in use
		.add<skill_id_t>(enter.mountSkill)
		.add<int32_t>(unk)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int8_t>(0)
		.add<int32_t>(unk)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(unk)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int8_t>(0)
		.add<int32_t>(unk)
		.add<int16_t>(0)
		.add<int8_t>(0)
		.add<job_id_t>(player->getStats()->getJob())
		.addBuffer(PlayerPacketHelper::addPlayerDisplay(player))
		.add<int32_t>(0)
		.add<item_id_t>(player->getItemEffect())
		.add<item_id_t>(player->getChair())
		.add<Point>(player->getPos())
		.add<int8_t>(player->getStance())
		.add<foothold_id_t>(player->getFoothold())
		.add<int8_t>(0);

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			builder
				.add<int8_t>(1)
				.add<item_id_t>(pet->getItemId())
				.add<string_t>(pet->getName())
				.add<pet_id_t>(pet->getId())
				.add<Point>(pet->getPos())
				.add<int8_t>(pet->getStance())
				.add<foothold_id_t>(pet->getFoothold())
				.add<bool>(pet->hasNameTag())
				.add<bool>(pet->hasQuoteItem());
		}
	}
	// End of pets
	builder.add<int8_t>(0);

	player->getMounts()->mountInfoMapSpawnPacket(builder);

	builder.add<int8_t>(0); // Player room

	bool hasChalkboard = !player->getChalkboard().empty();
	builder.add<bool>(hasChalkboard);
	if (hasChalkboard) {
		builder.add<string_t>(player->getChalkboard());
	}

	builder
		.add<int8_t>(0) // Rings (crush)
		.add<int8_t>(0) // Rings (friends)
		.add<int8_t>(0) // Ring (marriage)
		.add<int8_t>(0)
		.add<int8_t>(0)
		.add<int8_t>(0);
	return builder;
}

PACKET_IMPL(removePlayer, player_id_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MAP_PLAYER_DESPAWN)
		.add<player_id_t>(playerId);
	return builder;
}

PACKET_IMPL(changeMap, Player *player, bool spawnByPosition, const Point &spawnPosition) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CHANGE_MAP)
		.add<int32_t>(ChannelServer::getInstance().getChannelId())
		.add<uint8_t>(player->getPortalCount(true))
		.add<bool>(false) // Not a connect packet
		.add<int16_t>(0); // Some amount for a funny message at the top of the screen

	if (false) {
		size_t lineAmount = 0;
		builder.add<string_t>("Message title");
		for (size_t i = 0; i < lineAmount; i++) {
			builder.add<string_t>("Line");
		}
	}

	builder
		.add<map_id_t>(player->getMapId())
		.add<portal_id_t>(player->getMapPos())
		.add<health_t>(player->getStats()->getHp())
		.add<bool>(spawnByPosition);
	
	if (spawnByPosition) {
		builder.add<WidePoint>(WidePoint{spawnPosition});
	}

	builder.add<int64_t>(TimeUtilities::getServerTime());
	return builder;
}

PACKET_IMPL(portalBlocked) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_UPDATE)
		.add<int8_t>(0x01)
		.add<int32_t>(0x00);
	return builder;
}

PACKET_IMPL(showClock, int8_t hour, int8_t min, int8_t sec) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_TIMER)
		.add<int8_t>(0x01)
		.add<int8_t>(hour)
		.add<int8_t>(min)
		.add<int8_t>(sec);
	return builder;
}

PACKET_IMPL(showTimer, const seconds_t &sec) {
	PacketBuilder builder;
	if (sec.count() > 0) {
		builder
			.add<header_t>(SMSG_TIMER)
			.add<int8_t>(0x02)
			.add<int32_t>(static_cast<int32_t>(sec.count()));
	}
	else {
		builder.add<header_t>(SMSG_TIMER_OFF);
	}
	return builder;
}

PACKET_IMPL(forceMapEquip) {
	PacketBuilder builder;
	builder.add<header_t>(SMSG_MAP_FORCE_EQUIPMENT);
	return builder;
}

PACKET_IMPL(showEventInstructions) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_EVENT_INSTRUCTION)
		.add<int8_t>(0x00);
	return builder;
}

PACKET_IMPL(spawnMist, Mist *mist, bool mapEntry) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MIST_SPAWN)
		.add<mist_id_t>(mist->getId())
		.add<int32_t>(mist->isMobMist() ? 0 : mist->isPoison() ? 1 : 2)
		.add<int32_t>(mist->getOwnerId())
		.add<skill_id_t>(mist->getSkillId())
		.add<skill_level_t>(mist->getSkillLevel())
		.add<int16_t>(mapEntry ? 0 : mist->getDelay())
		.add<WidePoint>(WidePoint{mist->getArea().leftTop()})
		.add<WidePoint>(WidePoint{mist->getArea().rightBottom()})
		.add<int32_t>(0);
	return builder;
}

PACKET_IMPL(removeMist, map_object_t id) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MIST_DESPAWN)
		.add<map_object_t>(id);
	return builder;
}

PACKET_IMPL(spawnDoor, ref_ptr_t<MysticDoor> door, bool isInsideTown, bool alreadyOpen) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MYSTIC_DOOR_SPAWN)
		.add<bool>(alreadyOpen)
		.add<player_id_t>(door->getOwnerId())
		.add<Point>(isInsideTown ? door->getTownPos() : door->getMapPos());
	return builder;
}

PACKET_IMPL(removeDoor, ref_ptr_t<MysticDoor> door, bool isFade) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MYSTIC_DOOR_DESPAWN)
		.add<bool>(!isFade)
		.add<player_id_t>(door->getOwnerId());
	return builder;
}

PACKET_IMPL(spawnPortal, ref_ptr_t<MysticDoor> door, map_id_t callingMap) {
	PacketBuilder builder;
	builder.add<header_t>(SMSG_PORTAL_ACTION);
	if (door->getMapId() == callingMap) {
		builder
			.add<map_id_t>(door->getMapId())
			.add<map_id_t>(door->getTownId())
			.add<Point>(door->getTownPos());
	}
	else {
		builder
			.add<map_id_t>(door->getTownId())
			.add<map_id_t>(door->getMapId())
			.add<Point>(door->getMapPos());
	}
	return builder;
}

PACKET_IMPL(removePortal) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PORTAL_ACTION)
		.add<map_id_t>(Maps::NoMap)
		.add<map_id_t>(Maps::NoMap);
	return builder;
}

PACKET_IMPL(instantWarp, portal_id_t portalId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MAP_TELEPORT)
		.add<int8_t>(0x01)
		.add<portal_id_t>(portalId);
	return builder;
}

PACKET_IMPL(boatDockUpdate, bool docked, int8_t shipKind) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_SHIP)
		.add<int8_t>(shipKind == ShipKind::Balrog ?
			0x0A :
			(docked ? 0x0C : 0x08))
		.add<int8_t>(shipKind == ShipKind::Balrog ?
			(docked ? 0x04 : 0x05) :
			(docked ? 0x06 : 0x02));
	return builder;
}

PACKET_IMPL(changeWeather, bool adminWeather, item_id_t itemId, const string_t &message) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MAP_WEATHER_EFFECT)
		.add<bool>(adminWeather)
		.add<item_id_t>(itemId);

	if (itemId != 0 && !adminWeather) {
		// Admin weathers doesn't have a message
		builder.add<string_t>(message);
	}
	return builder;
}

}