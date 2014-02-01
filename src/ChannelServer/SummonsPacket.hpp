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
#pragma once

#include "PacketBuilder.hpp"
#include "SplitPacketBuilder.hpp"
#include "Types.hpp"

class Player;
class Summon;
struct Pos;

namespace SummonsPacket {
	SPLIT_PACKET(showSummon, int32_t playerId, Summon *summon, bool animated);
	SPLIT_PACKET(moveSummon, int32_t playerId, Summon *summon, const Pos &startPos, unsigned char *buf, int32_t bufLen);
	SPLIT_PACKET(removeSummon, int32_t playerId, Summon *summon, int8_t message);
	SPLIT_PACKET(damageSummon, int32_t playerId, int32_t summonId, int8_t unk, int32_t damage, int32_t mobId);
}