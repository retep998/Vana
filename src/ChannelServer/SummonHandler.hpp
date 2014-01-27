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

#include "Types.hpp"
#include <string>

class PacketReader;
class Player;
template <typename T> class LoopingId;

namespace SummonMessages {
	enum Messages : int8_t {
		OutOfTime = 0x00,
		Disappearing = 0x03,
		None = 0x04
	};
}

namespace SummonHandler {
	extern LoopingId<int32_t> summonIds;
	auto useSummon(Player *player, int32_t skillId, uint8_t level) -> void;
	auto removeSummon(Player *player, bool puppet, bool packetOnly, int8_t showMessage, bool fromTimer = false) -> void;
	auto showSummon(Player *player) -> void;
	auto showSummons(Player *fromPlayer, Player *toPlayer) -> void;
	auto moveSummon(Player *player, PacketReader &packet) -> void;
	auto damageSummon(Player *player, PacketReader &packet) -> void;
	auto loopId() -> int32_t;
}