/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "SummonsPacket.hpp"
#include "Common/GameConstants.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/SmsgHeader.hpp"
#include "ChannelServer/Summon.hpp"

namespace Vana {
namespace ChannelServer {
namespace Packets {

SPLIT_PACKET_IMPL(showSummon, player_id_t playerId, Summon *summon, bool isMapEntry) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SUMMON_SPAWN)
		.add<player_id_t>(playerId)
		.add<summon_id_t>(summon->getId())
		.add<skill_id_t>(summon->getSkillId())
		.add<skill_level_t>(summon->getSkillLevel())
		.add<Point>(summon->getPos())
		.add<int8_t>(summon->getStance())
		.add<foothold_id_t>(summon->getFoothold())
		.add<int8_t>(summon->getMovementType())
		.add<int8_t>(summon->getActionType())
		.add<bool>(isMapEntry);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(moveSummon, player_id_t playerId, Summon *summon, const Point &startPos, unsigned char *buf, int32_t bufLen) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SUMMON_MOVEMENT)
		.add<player_id_t>(playerId)
		.add<summon_id_t>(summon->getId())
		.add<Point>(startPos)
		.addBuffer(buf, bufLen);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(removeSummon, player_id_t playerId, Summon *summon, int8_t message) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SUMMON_DESPAWN)
		.add<player_id_t>(playerId)
		.add<summon_id_t>(summon->getId())
		.add<int8_t>(message);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(damageSummon, player_id_t playerId, summon_id_t summonId, int8_t unk, damage_t damage, map_object_t mobId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SUMMON_DAMAGE)
		.add<player_id_t>(playerId)
		.add<summon_id_t>(summonId)
		.add<int8_t>(unk)
		.add<damage_t>(damage)
		.add<map_object_t>(mobId)
		.unk<int8_t>();

	builder.map.addBuffer(builder.player);
	return builder;
}

PACKET_IMPL(summonSkill, player_id_t playerId, skill_id_t skillId, uint8_t display, skill_level_t level) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_SUMMON_SKILL)
		.add<player_id_t>(playerId)
		.add<skill_id_t>(skillId)
		.add<uint8_t>(display);
	return builder;
}

SPLIT_PACKET_IMPL(summonSkillEffect, player_id_t playerId, skill_id_t skillId, uint8_t display, skill_level_t level) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.add<int8_t>(2)
		.add<skill_id_t>(skillId)
		.add<skill_level_t>(level)
		.add<int8_t>(1);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<player_id_t>(playerId)
		.add<int8_t>(2)
		.add<skill_id_t>(skillId)
		.add<uint8_t>(display)
		.add<int8_t>(1);
	return builder;
}

}
}
}