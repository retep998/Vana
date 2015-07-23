/*
Copyright (C) 2008-2015 Vana Development Team

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

PACKET_IMPL(addSkill, skill_id_t skillId, const PlayerSkillInfo &skillInfo) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_SKILL_ADD)
		.add<int8_t>(1)
		.add<int16_t>(1)
		.add<skill_id_t>(skillId)
		.add<int32_t>(skillInfo.level)
		.add<int32_t>(skillInfo.playerMaxSkillLevel)
		.add<int8_t>(1);
	return builder;
}

SPLIT_PACKET_IMPL(showSkill, player_id_t playerId, skill_id_t skillId, skill_level_t level, uint8_t direction, bool party, bool self) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet
		.add<int8_t>(party ? 2 : 1)
		.add<skill_id_t>(skillId)
		.add<skill_level_t>(level);

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
				.add<player_id_t>(playerId);
		}
		builder.player.addBuffer(packet);
	}
	else {
		builder.map
			.add<header_t>(SMSG_SKILL_SHOW)
			.add<player_id_t>(playerId)
			.addBuffer(packet);
	}
	return builder;
}

PACKET_IMPL(healHp, health_t hp) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_THEATRICS)
		.add<int8_t>(0x0A)
		.add<health_t>(hp);
	return builder;
}

SPLIT_PACKET_IMPL(showSkillEffect, player_id_t playerId, skill_id_t skillId) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	switch (skillId) {
		case Skills::FpWizard::MpEater:
		case Skills::IlWizard::MpEater:
		case Skills::Cleric::MpEater:
			packet
				.add<int8_t>(1)
				.add<skill_id_t>(skillId)
				.add<int8_t>(1);
			break;
		case Skills::ChiefBandit::MesoGuard:
		case Skills::DragonKnight::DragonBlood:
			packet
				.add<int8_t>(5)
				.add<skill_id_t>(skillId);
			break;
		default:
			return builder;
	}

	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.addBuffer(packet);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<player_id_t>(playerId)
		.addBuffer(packet);
	return builder;
}

SPLIT_PACKET_IMPL(showChargeOrStationarySkill, player_id_t playerId, const ChargeOrStationarySkillInfo &info) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_CHARGE_OR_STATIONARY_SKILL)
		.add<player_id_t>(playerId)
		.add<skill_id_t>(info.skillId)
		.add<skill_level_t>(info.level)
		.add<int8_t>(info.direction)
		.add<int8_t>(info.weaponSpeed);
	return builder;
}

SPLIT_PACKET_IMPL(endChargeOrStationarySkill, player_id_t playerId, const ChargeOrStationarySkillInfo &info) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_CHARGE_OR_STATIONARY_SKILL_END)
		.add<player_id_t>(playerId)
		.add<skill_id_t>(info.skillId);
	return builder;
}

SPLIT_PACKET_IMPL(showMagnetSuccess, map_object_t mapMobId, uint8_t success) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_MOB_DRAGGED)
		.add<map_object_t>(mapMobId)
		.add<uint8_t>(success);
	return builder;
}

PACKET_IMPL(sendCooldown, skill_id_t skillId, seconds_t time) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_SKILL_COOLDOWN)
		.add<skill_id_t>(skillId)
		.add<int16_t>(static_cast<int16_t>(time.count()));
	return builder;
}

SPLIT_PACKET_IMPL(showBerserk, player_id_t playerId, skill_level_t level, bool on) {
	SplitPacketBuilder builder;
	PacketBuilder packet;
	packet
		.add<int8_t>(1)
		.add<skill_id_t>(Skills::DarkKnight::Berserk)
		.add<skill_level_t>(level)
		.add<bool>(on);

	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.addBuffer(packet);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<player_id_t>(playerId)
		.addBuffer(packet);
	return builder;
}

}