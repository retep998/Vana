/*
Copyright (C) 2008-2011 Vana Development Team

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

class Player;
class PacketReader;

namespace InventoryHandler {
	void itemMove(Player *player, PacketReader &packet);
	void useItem(Player *player, PacketReader &packet);
	void cancelItem(Player *player, PacketReader &packet);
	void useSkillbook(Player *player, PacketReader &packet);
	void useChair(Player *player, PacketReader &packet);
	void useItemEffect(Player *player, PacketReader &packet);
	void handleChair(Player *player, PacketReader &packet);
	void useSummonBag(Player *player, PacketReader &packet);
	void useReturnScroll(Player *player, PacketReader &packet);
	void useScroll(Player *player, PacketReader &packet);
	void useCashItem(Player *player, PacketReader &packet);
	void handleRockFunctions(Player *player, PacketReader &packet);
	bool handleRockTeleport(Player *player, int32_t itemid, PacketReader &packet);
	void handleHammerTime(Player *player);
	void handleRewardItem(Player *player, PacketReader &packet);
	void handleScriptItem(Player *player, PacketReader &packet);
}
