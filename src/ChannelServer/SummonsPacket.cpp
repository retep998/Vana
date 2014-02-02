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
#include "SummonsPacket.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "SmsgHeader.hpp"
#include "Summon.hpp"

namespace SummonsPacket {

SPLIT_PACKET_IMPL(showSummon, int32_t playerId, Summon *summon, bool animated) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SUMMON_SPAWN)
		.add<int32_t>(playerId)
		.add<int32_t>(summon->getId())
		.add<int32_t>(summon->getSummonId())
		.add<int8_t>(summon->getLevel())
		.add<Pos>(summon->getPos())
		.add<int8_t>(4) // ?
		.add<int8_t>(0x53) // ?
		.add<int8_t>(1) // ?
		.add<int8_t>(summon->getType()) // Movement type
		.add<int8_t>(!GameLogicUtilities::isPuppet(summon->getSummonId())) // Attack or not .add<bool> candidate?
		.add<bool>(!animated);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(moveSummon, int32_t playerId, Summon *summon, const Pos &startPos, unsigned char *buf, int32_t bufLen) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SUMMON_MOVEMENT)
		.add<int32_t>(playerId)
		.add<int32_t>(summon->getId())
		.add<Pos>(startPos)
		.addBuffer(buf, bufLen);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(removeSummon, int32_t playerId, Summon *summon, int8_t message) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SUMMON_DESPAWN)
		.add<int32_t>(playerId)
		.add<int32_t>(summon->getId())
		.add<int8_t>(message);

	builder.map.addBuffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(damageSummon, int32_t playerId, int32_t summonId, int8_t unk, int32_t damage, int32_t mobId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SUMMON_DAMAGE)
		.add<int32_t>(playerId)
		.add<int32_t>(summonId)
		.add<int8_t>(unk)
		.add<int32_t>(damage)
		.add<int32_t>(mobId)
		.add<int8_t>(0);

	builder.map.addBuffer(builder.player);
	return builder;
}

}