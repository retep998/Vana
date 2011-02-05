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
class PacketCreator;
class PacketReader;
struct NpcSpawnInfo;

namespace NpcPacket {
	void showNpc(Player *player, const NpcSpawnInfo &npc, int32_t id, bool show = true);
	void showNpc(int32_t mapid, const NpcSpawnInfo &npc, int32_t id, bool show = true);
	void showNpc(PacketCreator &packet, const NpcSpawnInfo &npc, int32_t id, bool show = true);
	void controlNpc(PacketCreator &packet, const NpcSpawnInfo &npc, int32_t id, bool show = true);
	void animateNpc(Player *player, PacketReader &pack);
	void showNpcEffect(Player *player, int32_t index, bool show = false);
	void showNpcEffect(int32_t mapid, int32_t index, bool show = false);
	void bought(Player *player, uint8_t msg);
};
