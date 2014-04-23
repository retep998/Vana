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
#include "MobsPacket.hpp"
#include "Maps.hpp"
#include "Mob.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "StatusInfo.hpp"
#include "WidePos.hpp"

namespace MobsPacket {

PACKET_IMPL(spawnMob, ref_ptr_t<Mob> mob, int8_t summonEffect, ref_ptr_t<Mob> owner, MobSpawnType spawn) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_SHOW)
		.addBuffer(mobPacket(mob, summonEffect, owner, spawn));
	return builder;
}

PACKET_IMPL(requestControl, ref_ptr_t<Mob> mob, MobSpawnType spawn) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_CONTROL)
		.add<int8_t>(1)
		.addBuffer(mobPacket(mob, 0, nullptr, spawn));
	return builder;
}

PACKET_IMPL(mobPacket, ref_ptr_t<Mob> mob, int8_t summonEffect, ref_ptr_t<Mob> owner, MobSpawnType spawn) {
	PacketBuilder builder;
	builder
		.add<map_object_t>(mob->getMapMobId())
		.add<int8_t>(static_cast<int8_t>(mob->getControlStatus()))
		.add<mob_id_t>(mob->getMobId())
		.add<int32_t>(mob->getStatusBits());

	for (const auto &kvp : mob->getStatusInfo()) {
		if (kvp.first != StatusEffects::Mob::Empty) {
			const StatusInfo &info = kvp.second;
			builder.add<int16_t>(static_cast<int16_t>(info.val));
			if (info.skillId >= 0) {
				builder.add<int32_t>(info.skillId);
			}
			else {
				builder
					.add<int16_t>(info.mobSkill)
					.add<int16_t>(info.level);
			}
			builder.add<int16_t>(1);
		}
		else {
			builder.add<int32_t>(0);
		}
	}

	builder.add<Pos>(mob->getPos());

	int8_t bitfield = (owner != nullptr ? 0x08 : 0x02) | (mob->isFacingLeft() ? 0x01 : 0);
	if (mob->canFly()) {
		bitfield |= 0x04;
	}

	builder
		.add<int8_t>(bitfield) // 0x08 - a summon, 0x04 - flying, 0x02 - ???, 0x01 - faces left
		.add<foothold_id_t>(mob->getFoothold())
		.add<foothold_id_t>(mob->getOriginFoothold());

	if (owner != nullptr) {
		builder
			.add<int8_t>(summonEffect != 0 ? summonEffect : -3)
			.add<map_object_t>(owner->getMapMobId());
	}
	else {
		builder.add<int8_t>(spawn == MobSpawnType::New ? -2 : -1);
	}

	builder
		.add<int8_t>(-1)
		.add<int32_t>(0);
	return builder;
}

PACKET_IMPL(endControlMob, map_object_t mapMobId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_CONTROL)
		.add<int8_t>(0)
		.add<map_object_t>(mapMobId);
	return builder;
}

PACKET_IMPL(moveMobResponse, map_object_t mapMobId, int16_t moveId, bool skillPossible, int32_t mp, mob_skill_id_t skill, mob_skill_level_t level) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_MOVEMENT)
		.add<map_object_t>(mapMobId)
		.add<int16_t>(moveId)
		.add<bool>(skillPossible)
		.add<int16_t>(static_cast<int16_t>(mp))
		.add<mob_skill_id_t>(skill)
		.add<mob_skill_level_t>(level);
	return builder;
}

PACKET_IMPL(moveMob, map_object_t mapMobId, bool skillPossible, int8_t rawAction, mob_skill_id_t skill, mob_skill_level_t level, int16_t option, unsigned char *buf, int32_t len) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_CONTROL_MOVEMENT)
		.add<map_object_t>(mapMobId)
		.add<bool>(skillPossible)
		.add<int8_t>(rawAction)
		.add<mob_skill_id_t>(skill)
		.add<mob_skill_level_t>(level)
		.add<int16_t>(option)
		.addBuffer(buf, len);
	return builder;
}

PACKET_IMPL(healMob, map_object_t mapMobId, int32_t amount) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_DAMAGE)
		.add<map_object_t>(mapMobId)
		.add<int8_t>(0)
		.add<int32_t>(-amount)
		.add<int8_t>(0)
		.add<int8_t>(0)
		.add<int8_t>(0);
	return builder;
}

PACKET_IMPL(hurtMob, map_object_t mapMobId, damage_t amount) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_DAMAGE)
		.add<map_object_t>(mapMobId)
		.add<int8_t>(0)
		.add<damage_t>(amount)
		.add<int8_t>(0)
		.add<int8_t>(0)
		.add<int8_t>(0);
	return builder;
}

PACKET_IMPL(damageFriendlyMob, ref_ptr_t<Mob> mob, damage_t damage) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_DAMAGE)
		.add<map_object_t>(mob->getMapMobId())
		.add<int8_t>(1)
		.add<damage_t>(damage)
		.add<int32_t>(mob->getHp())
		.add<int32_t>(mob->getMaxHp());
	return builder;
}

PACKET_IMPL(applyStatus, map_object_t mapMobId, int32_t statusMask, const vector_t<StatusInfo> &info, int16_t delay, const vector_t<int32_t> &reflection) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_STATUS_ADDITION)
		.add<map_object_t>(mapMobId)
		.add<int32_t>(statusMask);

	for (const auto &status : info) {
		builder.add<int16_t>(static_cast<int16_t>(status.val));
		if (status.skillId >= 0) {
			builder.add<int32_t>(status.skillId);
		}
		else {
			builder
				.add<int16_t>(status.mobSkill)
				.add<int16_t>(status.level);
		}
		builder.add<int16_t>(-1); // Not sure what this is
	}

	for (const auto &reflect : reflection) {
		builder.add<int32_t>(reflect);
	}

	builder.add<int16_t>(delay);

	uint8_t buffCount = info.size();
	if (reflection.size() > 0) {
		buffCount /= 2; // This gives 2 buffs per reflection but it's really one buff
	}
	builder.add<uint8_t>(buffCount);
	return builder;
}

PACKET_IMPL(removeStatus, map_object_t mapMobId, int32_t status) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_STATUS_REMOVE)
		.add<map_object_t>(mapMobId)
		.add<int32_t>(status)
		.add<int8_t>(1);
	return builder;
}

PACKET_IMPL(showHp, map_object_t mapMobId, int8_t percentage) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_HP_DISPLAY)
		.add<map_object_t>(mapMobId)
		.add<int8_t>(percentage);
	return builder;
}

PACKET_IMPL(showBossHp, ref_ptr_t<Mob> mob) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MAP_EFFECT)
		.add<int8_t>(0x05)
		.add<map_object_t>(mob->getMobId())
		.add<int32_t>(mob->getHp())
		.add<int32_t>(mob->getMaxHp())
		.add<int8_t>(mob->getHpBarColor())
		.add<int8_t>(mob->getHpBarBgColor());
	return builder;
}

PACKET_IMPL(dieMob, map_object_t mapMobId, int8_t death) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MOB_DEATH)
		.add<map_object_t>(mapMobId)
		.add<int8_t>(death);
	return builder;
}

PACKET_IMPL(showSpawnEffect, int8_t summonEffect, const Pos &pos) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MAP_EFFECT)
		.add<int8_t>(0x00)
		.add<int8_t>(summonEffect)
		.add<WidePos>(WidePos(pos));
	return builder;
}

}