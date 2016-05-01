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
#include "PlayersPacket.hpp"
#include "Common/AttackData.hpp"
#include "Common/GameConstants.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/InterHeader.hpp"
#include "Common/PacketReader.hpp"
#include "Common/ReturnDamageData.hpp"
#include "Common/Session.hpp"
#include "Common/WidePoint.hpp"
#include "ChannelServer.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Pet.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/SmsgHeader.hpp"

namespace Vana {
namespace ChannelServer {
namespace Packets {
namespace Players {

SPLIT_PACKET_IMPL(showMoving, player_id_t playerId, unsigned char *buf, size_t size) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_PLAYER_MOVEMENT)
		.add<player_id_t>(playerId)
		.unk<int32_t>()
		.addBuffer(buf, size);
	return builder;
}

SPLIT_PACKET_IMPL(faceExpression, player_id_t playerId, int32_t face) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_EMOTE)
		.add<player_id_t>(playerId)
		.add<int32_t>(face);
	return builder;
}

PACKET_IMPL(showChat, player_id_t playerId, bool isGm, const string_t &msg, bool bubbleOnly) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_CHAT)
		.add<player_id_t>(playerId)
		.add<bool>(isGm)
		.add<string_t>(msg)
		.add<bool>(bubbleOnly);
	return builder;
}

SPLIT_PACKET_IMPL(damagePlayer, player_id_t playerId, damage_t dmg, mob_id_t mob, uint8_t hit, int8_t type, uint8_t stance, skill_id_t noDamageSkill, const ReturnDamageData &pgmr) {
	SplitPacketBuilder builder;
	const int8_t BumpDamage = -1;
	const int8_t MapDamage = -2;

	builder.map
		.add<header_t>(SMSG_PLAYER_DAMAGE)
		.add<player_id_t>(playerId)
		.add<int8_t>(type);

	switch (type) {
		case MapDamage:
			builder.map
				.add<damage_t>(dmg)
				.add<damage_t>(dmg);
			break;
		default:
			builder.map
				.add<damage_t>(pgmr.reduction > 0 ? pgmr.damage : dmg)
				.add<mob_id_t>(mob)
				.add<uint8_t>(hit)
				.add<uint8_t>(pgmr.reduction);

			if (pgmr.reduction > 0) {
				builder.map
					.add<bool>(pgmr.isPhysical) // Maybe? No Mana Reflection on global to test with
					.add<map_object_t>(pgmr.mapMobId)
					.unk<int8_t>(6)
					.add<Point>(pgmr.pos);
			}

			builder.map
				.add<uint8_t>(stance)
				.add<damage_t>(dmg);

			if (noDamageSkill > 0) {
				builder.map.add<skill_id_t>(noDamageSkill);
			}
			break;
	}
	return builder;
}

PACKET_IMPL(showInfo, ref_ptr_t<Vana::ChannelServer::Player> getInfo, bool isSelf) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_INFO)
		.add<player_id_t>(getInfo->getId())
		.add<player_level_t>(getInfo->getStats()->getLevel())
		.add<job_id_t>(getInfo->getStats()->getJob())
		.add<fame_t>(getInfo->getStats()->getFame())
		.add<bool>(false) // Married
		.add<string_t>("-") // Guild
		.add<string_t>("") // Guild Alliance
		.add<bool>(isSelf);

	getInfo->getPets()->petInfoPacket(builder);
	getInfo->getMounts()->mountInfoPacket(builder);
	getInfo->getInventory()->wishlistInfoPacket(builder);
	getInfo->getMonsterBook()->infoPacket(builder);
	return builder;
}

PACKET_IMPL(whisperPlayer, const string_t &whispererName, channel_id_t channel, const string_t &message) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_COMMAND)
		.add<int8_t>(0x12)
		.add<string_t>(whispererName)
		.add<int16_t>(channel)
		.add<string_t>(message);
	return builder;
}

PACKET_IMPL(findPlayer, const string_t &name, opt_int32_t map, uint8_t is, bool isChannel) {
	PacketBuilder builder;
	builder.add<header_t>(SMSG_COMMAND);
	if (map.is_initialized()) {
		builder
			.add<int8_t>(0x09)
			.add<string_t>(name)
			.add<int8_t>(isChannel ? 0x03 : 0x01)
			.add<int32_t>(map.get())
			.unk<int32_t>()
			.unk<int32_t>();
	}
	else {
		builder
			.add<int8_t>(0x0A)
			.add<string_t>(name)
			.add<int8_t>(is);
	}
	return builder;
}

SPLIT_PACKET_IMPL(useMeleeAttack, player_id_t playerId, skill_id_t masterySkillId, skill_level_t masteryLevel, const AttackData &attack) {
	SplitPacketBuilder builder;
	int8_t hitByte = (attack.targets * 0x10) + attack.hits;
	skill_id_t skillId = attack.skillId;
	bool isMesoExplosion = attack.isMesoExplosion;
	if (isMesoExplosion) {
		hitByte = (attack.targets * 0x10) + 0x0A;
	}

	builder.map
		.add<header_t>(SMSG_ATTACK_MELEE)
		.add<player_id_t>(playerId)
		.add<int8_t>(hitByte)
		.add<skill_level_t>(attack.skillLevel);

	if (skillId != Vana::Skills::All::RegularAttack) {
		builder.map.add<skill_id_t>(skillId);
	}

	builder.map
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weaponSpeed)
		.add<uint8_t>(masterySkillId > 0 ? GameLogicUtilities::getMasteryDisplay(masteryLevel) : 0)
		.unk<int32_t>();

	for (const auto &target : attack.damages) {
		builder.map
			.add<map_object_t>(target.first)
			.unk<int8_t>(0x06);

		if (isMesoExplosion) {
			builder.map.add<uint8_t>(static_cast<uint8_t>(target.second.size()));
		}
		for (const auto &hit : target.second) {
			builder.map.add<damage_t>(hit);
		}
	}
	return builder;
}

SPLIT_PACKET_IMPL(useRangedAttack, player_id_t playerId, skill_id_t masterySkillId, skill_level_t masteryLevel, const AttackData &attack) {
	SplitPacketBuilder builder;
	skill_id_t skillId = attack.skillId;

	builder.map
		.add<header_t>(SMSG_ATTACK_RANGED)
		.add<player_id_t>(playerId)
		.add<int8_t>((attack.targets * 0x10) + attack.hits)
		.add<skill_level_t>(attack.skillLevel);

	if (skillId != Vana::Skills::All::RegularAttack) {
		builder.map.add<skill_id_t>(skillId);
	}

	builder.map
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weaponSpeed)
		.add<uint8_t>(masterySkillId > 0 ? GameLogicUtilities::getMasteryDisplay(masteryLevel) : 0)
		// Bug in global:
		// The colored swoosh does not display as it should
		.add<item_id_t>(attack.starId);

	for (const auto &target : attack.damages) {
		builder.map
			.add<map_object_t>(target.first)
			.unk<int8_t>(0x06);

		for (const auto &hit : target.second) {
			damage_t damage = hit;
			switch (skillId) {
				case Vana::Skills::Marksman::Snipe: // Snipe is always crit
					damage |= 0x80000000; // Critical damage = 0x80000000 + damage
					break;
				default:
					break;
			}
			builder.map.add<damage_t>(damage);
		}
	}
	builder.map.add<Point>(attack.projectilePos);
	return builder;
}

SPLIT_PACKET_IMPL(useSpellAttack, player_id_t playerId, const AttackData &attack) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_ATTACK_MAGIC)
		.add<player_id_t>(playerId)
		.add<int8_t>((attack.targets * 0x10) + attack.hits)
		.add<uint8_t>(attack.skillLevel)
		.add<skill_id_t>(attack.skillId)
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weaponSpeed)
		.add<uint8_t>(0) // Mastery byte is always 0 because spells don't have a swoosh
		.unk<int32_t>();

	for (const auto &target : attack.damages) {
		builder.map
			.add<map_object_t>(target.first)
			.unk<int8_t>(0x06);

		for (const auto &hit : target.second) {
			builder.map.add<damage_t>(hit);
		}
	}

	if (attack.charge > 0) {
		builder.map.add<charge_time_t>(attack.charge);
	}
	return builder;
}

SPLIT_PACKET_IMPL(useSummonAttack, player_id_t playerId, const AttackData &attack) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_SUMMON_ATTACK)
		.add<player_id_t>(playerId)
		.add<summon_id_t>(attack.summonId)
		.add<int8_t>(attack.animation)
		.add<int8_t>(attack.targets);

	for (const auto &target : attack.damages) {
		builder.map
			.add<map_object_t>(target.first)
			.unk<int8_t>(0x06);

		for (const auto &hit : target.second) {
			builder.map.add<damage_t>(hit);
		}
	}
	return builder;
}

SPLIT_PACKET_IMPL(useBombAttack, player_id_t playerId, charge_time_t chargeTime, skill_id_t skillId, const WidePoint &pos) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_3RD_PARTY_BOMB)
		.add<player_id_t>(playerId)
		.add<WidePoint>(pos)
		.add<charge_time_t>(chargeTime)
		.add<skill_id_t>(skillId);
	return builder;
}

SPLIT_PACKET_IMPL(useEnergyChargeAttack, player_id_t playerId, int32_t masterySkillId, uint8_t masteryLevel, const AttackData &attack) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_ATTACK_ENERGYCHARGE)
		.add<player_id_t>(playerId)
		.add<int8_t>((attack.targets * 0x10) + attack.hits)
		.add<skill_level_t>(attack.skillLevel)
		.add<skill_id_t>(attack.skillId)
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weaponSpeed)
		.add<uint8_t>(masterySkillId > 0 ? GameLogicUtilities::getMasteryDisplay(masteryLevel) : 0)
		.unk<int32_t>();

	for (const auto &target : attack.damages) {
		builder.map
			.add<map_object_t>(target.first)
			.unk<int8_t>(0x06);

		for (const auto &hit : target.second) {
			builder.map.add<damage_t>(hit);
		}
	}
	return builder;
}

}
}
}
}