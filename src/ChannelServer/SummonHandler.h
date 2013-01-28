/*
Copyright (C) 2008-2013 Vana Development Team

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

#include "Types.h"
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
	void useSummon(Player *player, int32_t skillId, uint8_t level);
	void removeSummon(Player *player, bool puppet, bool packetOnly, int8_t showMessage, bool fromTimer = false);
	void showSummon(Player *player);
	void showSummons(Player *fromPlayer, Player *toPlayer);
	void moveSummon(Player *player, PacketReader &packet);
	void damageSummon(Player *player, PacketReader &packet);
	int32_t loopId();
}