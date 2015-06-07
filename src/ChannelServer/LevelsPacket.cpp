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
#include "LevelsPacket.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace LevelsPacket {

PACKET_IMPL(showExp, experience_t exp, bool white, bool inChat) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(3)
		.add<bool>(white)
		.add<experience_t>(exp)
		.add<bool>(inChat)
		.unk<int32_t>()
		.unk<int8_t>()
		.unk<int8_t>()
		.unk<int32_t>();

	if (inChat) {
		builder.unk<int8_t>();
	}
	builder
		.unk<int8_t>()
		.unk<int32_t>();
	return builder;
}

SPLIT_PACKET_IMPL(levelUp, player_id_t playerId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<player_id_t>(playerId)
		.add<int8_t>(0);

	builder.map.addBuffer(builder.player);
	return builder;
}

PACKET_IMPL(statOk) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_UPDATE)
		.unk<int16_t>(1)
		.unk<int32_t>();
	return builder;
}

SPLIT_PACKET_IMPL(jobChange, player_id_t playerId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<player_id_t>(playerId)
		.add<int8_t>(8);

	builder.map.addBuffer(builder.player);
	return builder;
}

}