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
#include "SkillsPacket.hpp"
#include "GameConstants.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "Skills.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace SkillsPacket {

PACKET_IMPL(addSkill, int32_t skillId, const PlayerSkillInfo &skillInfo) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_SKILL_ADD)
		.add<int8_t>(1)
		.add<int16_t>(1)
		.add<int32_t>(skillId)
		.add<int32_t>(skillInfo.level)
		.add<int32_t>(skillInfo.playerMaxSkillLevel)
		.add<int8_t>(1);
	return builder;
}

SPLIT_PACKET_IMPL(showSkill, int32_t playerId, int32_t skillId, uint8_t level, uint8_t direction, bool party, bool self) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet
		.add<int8_t>(party ? 2 : 1)
		.add<int32_t>(skillId)
		.add<int8_t>(level);

	switch (skillId) {
		case Skills::Hero::MonsterMagnet:
		case Skills::Paladin::MonsterMagnet:
		case Skills::DarkKnight::MonsterMagnet:
			packet.add<uint8_t>(direction);
			break;
	}

	if (self) {
		if (party) {
			builder.player.add<header_t>(SMSG_THEATRICS);
		}
		else {
			builder.player
				.add<header_t>(SMSG_SKILL_SHOW)
				.add<int32_t>(playerId);
		}
		builder.player.addBuffer(packet);
	}
	else {
		builder.map
			.add<header_t>(SMSG_SKILL_SHOW)
			.add<int32_t>(playerId)
			.addBuffer(packet);
	}
	return builder;
}

PACKET_IMPL(healHp, int16_t hp) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_THEATRICS)
		.add<int8_t>(0x0A)
		.add<int16_t>(hp);
	return builder;
}

SPLIT_PACKET_IMPL(showSkillEffect, int32_t playerId, int32_t skillId) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	switch (skillId) {
		case Skills::FpWizard::MpEater:
		case Skills::IlWizard::MpEater:
		case Skills::Cleric::MpEater:
			packet
				.add<int8_t>(1)
				.add<int32_t>(skillId)
				.add<int8_t>(1);
			break;
		case Skills::ChiefBandit::MesoGuard:
		case Skills::DragonKnight::DragonBlood:
			packet
				.add<int8_t>(5)
				.add<int32_t>(skillId);
			break;
		default:
			return builder;
	}

	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.addBuffer(packet);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<int32_t>(playerId)
		.addBuffer(packet);
	return builder;
}

SPLIT_PACKET_IMPL(showChargeOrStationarySkill, int32_t playerId, const ChargeOrStationarySkillInfo &info) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_CHARGE_OR_STATIONARY_SKILL)
		.add<int32_t>(playerId)
		.add<int32_t>(info.skillId)
		.add<int8_t>(info.level)
		.add<int8_t>(info.direction)
		.add<int8_t>(info.weaponSpeed);
	return builder;
}

SPLIT_PACKET_IMPL(endChargeOrStationarySkill, int32_t playerId, const ChargeOrStationarySkillInfo &info) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_CHARGE_OR_STATIONARY_SKILL_END)
		.add<int32_t>(playerId)
		.add<int32_t>(info.skillId);
	return builder;
}

SPLIT_PACKET_IMPL(showMagnetSuccess, int32_t mapMobId, uint8_t success) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_MOB_DRAGGED)
		.add<int32_t>(mapMobId)
		.add<uint8_t>(success);
	return builder;
}

PACKET_IMPL(sendCooldown, int32_t skillId, int16_t time) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_SKILL_COOLDOWN)
		.add<int32_t>(skillId)
		.add<int16_t>(time);
	return builder;
}

SPLIT_PACKET_IMPL(showBerserk, int32_t playerId, uint8_t level, bool on) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet
		.add<int8_t>(1)
		.add<int32_t>(Skills::DarkKnight::Berserk)
		.add<int8_t>(level)
		.add<bool>(on);

	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.addBuffer(packet);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<int32_t>(playerId)
		.addBuffer(packet);
	return builder;
}

}