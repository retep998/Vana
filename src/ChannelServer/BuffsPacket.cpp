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
#include "BuffsPacket.hpp"
#include "Buffs.hpp"
#include "BuffsPacketHelper.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SkillConstants.hpp"
#include "SmsgHeader.hpp"

namespace BuffsPacket {

SPLIT_PACKET_IMPL(useSkill, int32_t playerId, int32_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill, int16_t addedInfo) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SKILL_USE)
		.addBuffer(BuffsPacketHelper::addBytes(playerSkill.types));

	for (const auto &val : playerSkill.vals) {
		builder.player
			.add<int16_t>(val)
			.add<int32_t>(skillId)
			.add<int32_t>(static_cast<int32_t>(time.count() * 1000));
	}

	builder.player
		.add<int16_t>(0)
		.add<int16_t>(addedInfo)
		.add<int8_t>(0); // Number of times you've been buffed total - only certain skills have this part

	if (playerSkill.hasMapBuff) {
		builder.map
			.add<header_t>(SMSG_3RD_PARTY_SKILL)
			.add<int32_t>(playerId)
			.addBuffer(BuffsPacketHelper::addBytes(mapSkill.typeList));

		if ((mapSkill.typeList[BuffBytes::Byte3] & 0x40) > 0) {
			builder.map.add<int32_t>(skillId);
		}
		else {
			for (size_t i = 0; i < mapSkill.values.size(); i++) {
				uint8_t byte = mapSkill.bytes[i];
				int16_t val = mapSkill.values[i];
				if (byte == BuffBytes::Byte5) {
					builder.map.add<int16_t>(val);
				}
				else {
					builder.map.add<int8_t>(static_cast<int8_t>(val));
				}
			}
		}
		builder.map
			.add<int16_t>(0)
			.add<int16_t>(addedInfo);
	}
	return builder;
}

SPLIT_PACKET_IMPL(giveDebuff, int32_t playerId, uint8_t skillId, uint8_t level, const seconds_t &time, int16_t delay, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SKILL_USE)
		.addBuffer(BuffsPacketHelper::addBytes(playerSkill.types));

	for (const auto &val : playerSkill.vals) {
		builder.player
			.add<int16_t>(val)
			.add<uint16_t>(skillId)
			.add<uint16_t>(level)
			.add<int32_t>(static_cast<int32_t>(time.count() * 1000));
	}

	builder.map
		.add<header_t>(SMSG_3RD_PARTY_SKILL)
		.add<int32_t>(playerId)
		.addBuffer(BuffsPacketHelper::addBytes(mapSkill.typeList));

	for (const auto &val : mapSkill.values) {
		if (skillId == MobSkills::Poison) {
			builder.map.add<int16_t>(val);
		}
		builder.map
			.add<uint16_t>(skillId)
			.add<uint16_t>(level);
	}
	
	builder.map
		.add<int16_t>(0)
		.add<int16_t>(delay);
	return builder;
}

SPLIT_PACKET_IMPL(endSkill, int32_t playerId, const ActiveBuff &playerSkill) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet.addBuffer(BuffsPacketHelper::addBytes(playerSkill.types));

	builder.player
		.add<header_t>(SMSG_SKILL_CANCEL)
		.addBuffer(packet)
		.add<int8_t>(0);

	builder.map
		.add<header_t>(SMSG_3RD_PARTY_BUFF_END)
		.add<int32_t>(playerId)
		.addBuffer(packet);
	return builder;
}

SPLIT_PACKET_IMPL(usePirateBuff, int32_t playerId, int32_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet.add<int16_t>(0);

	for (const auto &val : playerSkill.vals) {
		packet
			.add<int16_t>(val)
			.add<int16_t>(0)
			.add<int32_t>(skillId)
			.add<int32_t>(0) // No idea, seems to be server tick count in ms
			.add<int8_t>(0)
			.add<int16_t>(static_cast<int16_t>(time.count()));
	}

	packet.add<int16_t>(0);

	builder.player
		.add<header_t>(SMSG_SKILL_USE)
		.addBuffer(BuffsPacketHelper::addBytes(playerSkill.types))
		.addBuffer(packet)
		.add<int8_t>(0); // Number of times you've been buffed total - only certain skills have this part

	builder.map
		.add<header_t>(SMSG_3RD_PARTY_SKILL)
		.add<int32_t>(playerId)
		.addBuffer(BuffsPacketHelper::addBytes(mapSkill.typeList))
		.addBuffer(packet);
	return builder;
}

SPLIT_PACKET_IMPL(useSpeedInfusion, int32_t playerId, int32_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill, int16_t addedInfo) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet
		.add<int16_t>(0)
		.add<int32_t>(playerSkill.vals[0])
		.add<int32_t>(skillId)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int16_t>(0)
		.add<int16_t>(static_cast<int16_t>(time.count()))
		.add<int16_t>(addedInfo);

	builder.player
		.add<header_t>(SMSG_SKILL_USE)
		.addBuffer(BuffsPacketHelper::addBytes(playerSkill.types))
		.addBuffer(packet);

	builder.map
		.add<header_t>(SMSG_3RD_PARTY_SKILL)
		.add<int32_t>(playerId)
		.addBuffer(BuffsPacketHelper::addBytes(mapSkill.typeList))
		.addBuffer(packet);
	return builder;
}

SPLIT_PACKET_IMPL(useMount, int32_t playerId, int32_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill, int16_t addedInfo, int32_t mountId) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet
		.add<int16_t>(0)
		.add<int32_t>(mountId)
		.add<int32_t>(skillId)
		.add<int32_t>(0) // Server tick value
		.add<int16_t>(0)
		.add<int8_t>(0)
		.add<int8_t>(0); // Number of times you've been buffed total

	builder.player
		.add<header_t>(SMSG_SKILL_USE)
		.addBuffer(BuffsPacketHelper::addBytes(playerSkill.types))
		.addBuffer(packet);

	builder.map
		.add<header_t>(SMSG_3RD_PARTY_SKILL)
		.add<int32_t>(playerId)
		.addBuffer(BuffsPacketHelper::addBytes(mapSkill.typeList))
		.addBuffer(packet);
	return builder;
}

PACKET_IMPL(useHomingBeacon, int32_t skillId, const ActiveBuff &playerSkill, int32_t mapMobId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_SKILL_USE)
		.addBuffer(BuffsPacketHelper::addBytes(playerSkill.types))
		.add<int16_t>(0)
		.addVector<int16_t>(playerSkill.vals, playerSkill.vals.size())
		.add<int16_t>(0)
		.add<int32_t>(skillId)
		.add<int32_t>(0) // Time
		.add<int8_t>(0)
		.add<int32_t>(mapMobId)
		.add<int16_t>(0);
	return builder;
}

}