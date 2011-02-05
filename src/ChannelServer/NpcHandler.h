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

class PacketReader;
class Player;

namespace NpcHandler {
	void handleNpc(Player *player, PacketReader &packet);
	void handleNpcIn(Player *player, PacketReader &packet);
	void handleNpcAnimation(Player *player, PacketReader &packet);
	void handleQuestNpc(Player *player, int32_t npcid, bool start, int16_t questid = 0);
	void useShop(Player *player, PacketReader &packet);
	void useStorage(Player *player, PacketReader &packet);
	bool showShop(Player *player, int32_t shopid);
	bool showStorage(Player *player, int32_t npcid);
	bool showGuildRank(Player *player, int32_t npcid);
}
