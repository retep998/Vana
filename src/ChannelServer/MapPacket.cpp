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
#include "Pet.hpp"
#include "Player.hpp"
#include "PlayerPacketHelper.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "TimeUtilities.hpp"
#include "WidePos.hpp"
#include <unordered_map>

namespace MapPacket {

PACKET_IMPL(playerPacket, Player *player) {
	PacketBuilder builder;
	auto &enter = player->getActiveBuffs()->getMapEntryBuffs();

	builder
		.add<header_t>(SMSG_MAP_PLAYER_SPAWN)
		.add<int32_t>(player->getId())
		.addString(player->getName())
		.addString("") // Guild
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
						if (!(kvp.first == 0x01 && cbyte == Byte5)) { // Glitch in global, Slow doesn't display properly and if you try, it error 38s
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
								builder.add<int32_t>(player->getActiveBuffs()->getCharge());
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

	builder
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int16_t>(0)
		.add<int32_t>(1065638850) // Unknown
		.add<int16_t>(0)
		.add<int8_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(1065638850)
		.add<int16_t>(0)
		.add<int8_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(1065638850)
		.add<int16_t>(0)
		.add<int8_t>(0)
		.add<int32_t>(enter.mountId) // No point to having an if, these are 0 when not in use
		.add<int32_t>(enter.mountSkill)
		.add<int32_t>(1065638850)
		.add<int8_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(1065638850)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(1065638850)
		.add<int8_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(1065638850)
		.add<int16_t>(0)
		.add<int8_t>(0)
		.add<int16_t>(player->getStats()->getJob())
		.addBuffer(PlayerPacketHelper::addPlayerDisplay(player))
		.add<int32_t>(0)
		.add<int32_t>(player->getItemEffect())
		.add<int32_t>(player->getChair())
		.addClass<Pos>(player->getPos())
		.add<int8_t>(player->getStance())
		.add<int16_t>(player->getFoothold())
		.add<int8_t>(0);

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			builder
				.add<int8_t>(1)
				.add<int32_t>(pet->getItemId())
				.addString(pet->getName())
				.add<int64_t>(pet->getId())
				.addClass<Pos>(pet->getPos())
				.add<int8_t>(pet->getStance())
				.add<int16_t>(pet->getFoothold())
				.add<bool>(pet->hasNameTag())
				.add<bool>(pet->hasQuoteItem());
		}
	}

	builder
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int8_t>(0)
		.add<int8_t>(0)
		.add<bool>(!player->getChalkboard().empty())
		.addString(player->getChalkboard())
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int16_t>(0);
	return builder;
}

PACKET_IMPL(removePlayer, int32_t playerId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MAP_PLAYER_DESPAWN)
		.add<int32_t>(playerId);
	return builder;
}

PACKET_IMPL(changeMap, Player *player) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CHANGE_MAP)
		.add<int32_t>(ChannelServer::getInstance().getChannelId())
		.add<uint8_t>(player->getPortalCount(true))
		.add<bool>(false) // Not a connect packet
		.add<int16_t>(0); // Some amount for a funny message at the top of the screen

	if (false) {
		size_t lineAmount = 0;
		builder.addString("Message title");
		for (size_t i = 0; i < lineAmount; i++) {
			builder.addString("Line");
		}
	}

	builder
		.add<int32_t>(player->getMapId())
		.add<int8_t>(player->getMapPos())
		.add<int16_t>(player->getStats()->getHp())
		.add<int8_t>(0x00)
		.add<int64_t>(TimeUtilities::getServerTime());
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
		.add<int32_t>(mist->getId())
		.add<int32_t>(mist->isMobMist() ? 0 : mist->isPoison() ? 1 : 2)
		.add<int32_t>(mist->getOwnerId())
		.add<int32_t>(mist->getSkillId())
		.add<uint8_t>(mist->getSkillLevel())
		.add<int16_t>(mapEntry ? 0 : mist->getDelay())
		.addClass<WidePos>(WidePos(mist->getArea().leftTop()))
		.addClass<WidePos>(WidePos(mist->getArea().rightBottom()))
		.add<int32_t>(0);
	return builder;
}

PACKET_IMPL(removeMist, int32_t id) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MIST_DESPAWN)
		.add<int32_t>(id);
	return builder;
}

PACKET_IMPL(instantWarp, int8_t portalId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MAP_TELEPORT)
		.add<int8_t>(0x01)
		.add<int8_t>(portalId);
	return builder;
}

PACKET_IMPL(changeWeather, bool adminWeather, int32_t itemId, const string_t &message) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MAP_WEATHER_EFFECT)
		.add<bool>(adminWeather)
		.add<int32_t>(itemId);

	if (itemId != 0 && !adminWeather) {
		// Admin weathers doesn't have a message
		builder.addString(message);
	}
	return builder;
}

}