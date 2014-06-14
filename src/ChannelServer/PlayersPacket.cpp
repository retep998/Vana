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
#include "PlayersPacket.hpp"
#include "ChannelServer.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "InterHeader.hpp"
#include "Maps.hpp"
#include "PacketReader.hpp"
#include "Pet.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace PlayersPacket {

SPLIT_PACKET_IMPL(showMoving, int32_t playerId, unsigned char *buf, size_t size) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_PLAYER_MOVEMENT)
		.add<int32_t>(playerId)
		.add<int32_t>(0)
		.addBuffer(buf, size);
	return builder;
}

SPLIT_PACKET_IMPL(faceExpression, int32_t playerId, int32_t face) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_EMOTE)
		.add<int32_t>(playerId)
		.add<int32_t>(face);
	return builder;
}

PACKET_IMPL(showChat, int32_t playerId, bool isGm, const string_t &msg, bool bubbleOnly) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_CHAT)
		.add<int32_t>(playerId)
		.add<bool>(isGm)
		.add<string_t>(msg)
		.add<bool>(bubbleOnly);
	return builder;
}

SPLIT_PACKET_IMPL(damagePlayer, int32_t playerId, int32_t dmg, int32_t mob, uint8_t hit, int8_t type, uint8_t stance, int32_t noDamageSkill, const ReturnDamageInfo &pgmr) {
	SplitPacketBuilder builder;
	const int8_t BumpDamage = -1;
	const int8_t MapDamage = -2;

	builder.map
		.add<header_t>(SMSG_PLAYER_DAMAGE)
		.add<int32_t>(playerId)
		.add<int8_t>(type);
	switch (type) {
		case MapDamage:
			builder.map
				.add<int32_t>(dmg)
				.add<int32_t>(dmg);
			break;
		default:
			builder.map
				.add<int32_t>(pgmr.reduction > 0 ? pgmr.damage : dmg)
				.add<int32_t>(mob)
				.add<int8_t>(hit)
				.add<int8_t>(pgmr.reduction);

			if (pgmr.reduction > 0) {
				builder.map
					.add<bool>(pgmr.isPhysical) // Maybe? No Mana Reflection on global to test with
					.add<int32_t>(pgmr.mapMobId)
					.add<int8_t>(6)
					.add<Pos>(pgmr.pos);
			}

			builder.map
				.add<int8_t>(stance)
				.add<int32_t>(dmg);

			if (noDamageSkill > 0) {
				builder.map.add<int32_t>(noDamageSkill);
			}
			break;
	}
	return builder;
}

PACKET_IMPL(showInfo, Player *getInfo, bool isSelf) {
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
	getInfo->getInventory()->wishListPacket(builder);
	getInfo->getMonsterBook()->infoData(builder);
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

PACKET_IMPL(findPlayer, const string_t &name, int32_t map, uint8_t is, bool isChannel) {
	PacketBuilder builder;
	builder.add<header_t>(SMSG_COMMAND);
	if (map != -1) {
		builder
			.add<int8_t>(0x09)
			.add<string_t>(name)
			.add<int8_t>(isChannel ? 0x03 : 0x01)
			.add<int32_t>(map)
			.add<int32_t>(0)
			.add<int32_t>(0);
	}
	else {
		builder
			.add<int8_t>(0x0A)
			.add<string_t>(name)
			.add<int8_t>(is);
	}
	return builder;
}

SPLIT_PACKET_IMPL(useMeleeAttack, int32_t playerId, int32_t masterySkillId, uint8_t masteryLevel, const Attack &attack) {
	SplitPacketBuilder builder;
	int8_t hitByte = (attack.targets * 0x10) + attack.hits;
	int32_t skillId = attack.skillId;
	bool isMesoExplosion = attack.isMesoExplosion;
	if (isMesoExplosion) {
		hitByte = (attack.targets * 0x10) + 0x0A;
	}

	builder.map
		.add<header_t>(SMSG_ATTACK_MELEE)
		.add<int32_t>(playerId)
		.add<int8_t>(hitByte)
		.add<uint8_t>(attack.skillLevel);

	if (skillId != Skills::All::RegularAttack) {
		builder.map.add<int32_t>(skillId);
	}

	builder.map
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weaponSpeed)
		.add<uint8_t>(masterySkillId > 0 ? GameLogicUtilities::getMasteryDisplay(masteryLevel) : 0)
		.add<int32_t>(0);

	for (const auto &target : attack.damages) {
		builder.map
			.add<int32_t>(target.first)
			.add<int8_t>(0x06);

		if (isMesoExplosion) {
			builder.map.add<uint8_t>(target.second.size());
		}
		for (const auto &hit : target.second) {
			builder.map.add<int32_t>(hit);
		}
	}
	return builder;
}

SPLIT_PACKET_IMPL(useRangedAttack, int32_t playerId, int32_t masterySkillId, uint8_t masteryLevel, const Attack &attack) {
	SplitPacketBuilder builder;
	int32_t skillId = attack.skillId;

	builder.map
		.add<header_t>(SMSG_ATTACK_RANGED)
		.add<int32_t>(playerId)
		.add<int8_t>((attack.targets * 0x10) + attack.hits)
		.add<uint8_t>(attack.skillLevel);

	if (skillId != Skills::All::RegularAttack) {
		builder.map.add<int32_t>(skillId);
	}

	builder.map
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weaponSpeed)
		.add<uint8_t>(masterySkillId > 0 ? GameLogicUtilities::getMasteryDisplay(masteryLevel) : 0)
		// Bug in global:
		// The colored swoosh does not display as it should
		.add<int32_t>(attack.starId);

	for (const auto &target : attack.damages) {
		builder.map
			.add<int32_t>(target.first)
			.add<int8_t>(0x06);

		for (const auto &hit : target.second) {
			int32_t damage = hit;
			switch (skillId) {
				case Skills::Marksman::Snipe: // Snipe is always crit
					damage += 0x80000000; // Critical damage = 0x80000000 + damage
					break;
				default:
					break;
			}
			builder.map.add<int32_t>(damage);
		}
	}
	builder.map.add<Pos>(attack.projectilePos);
	return builder;
}

SPLIT_PACKET_IMPL(useSpellAttack, int32_t playerId, const Attack &attack) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_ATTACK_MAGIC)
		.add<int32_t>(playerId)
		.add<int8_t>((attack.targets * 0x10) + attack.hits)
		.add<uint8_t>(attack.skillLevel)
		.add<int32_t>(attack.skillId)
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weaponSpeed)
		.add<uint8_t>(0) // Mastery byte is always 0 because spells don't have a swoosh
		.add<int32_t>(0); // No clue

	for (const auto &target : attack.damages) {
		builder.map
			.add<int32_t>(target.first)
			.add<int8_t>(0x06);

		for (const auto &hit : target.second) {
			builder.map.add<int32_t>(hit);
		}
	}

	if (attack.charge > 0) {
		builder.map.add<int32_t>(attack.charge);
	}
	return builder;
}

SPLIT_PACKET_IMPL(useSummonAttack, int32_t playerId, const Attack &attack) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_SUMMON_ATTACK)
		.add<int32_t>(playerId)
		.add<int32_t>(attack.summonId)
		.add<int8_t>(attack.animation)
		.add<int8_t>(attack.targets);
	for (const auto &target : attack.damages) {
		builder.map
			.add<int32_t>(target.first)
			.add<int8_t>(0x06);

		for (const auto &hit : target.second) {
			builder.map.add<int32_t>(hit);
		}
	}
	return builder;
}

SPLIT_PACKET_IMPL(useEnergyChargeAttack, int32_t playerId, int32_t masterySkillId, uint8_t masteryLevel, const Attack &attack) {
	SplitPacketBuilder builder;
	builder.map
		.add<header_t>(SMSG_ATTACK_ENERGYCHARGE)
		.add<int32_t>(playerId)
		.add<int8_t>((attack.targets * 0x10) + attack.hits)
		.add<int8_t>(attack.skillLevel)
		.add<int32_t>(attack.skillId)
		.add<uint8_t>(attack.display)
		.add<uint8_t>(attack.animation)
		.add<uint8_t>(attack.weaponSpeed)
		.add<uint8_t>(masterySkillId > 0 ? GameLogicUtilities::getMasteryDisplay(masteryLevel) : 0)
		.add<int32_t>(0);

	for (const auto &target : attack.damages) {
		builder.map
			.add<int32_t>(target.first)
			.add<int8_t>(0x06);

		for (const auto &hit : target.second) {
			builder.map.add<int32_t>(hit);
		}
	}
	return builder;
}

}