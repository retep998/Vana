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

#include "Types.h"

class PacketReader;
class Player;
struct Pos;

namespace DropHandler {
	auto doDrops(int32_t playerId, int32_t mapId, int32_t droppingLevel, int32_t droppingId, const Pos &origin, bool explosive, bool ffa, int32_t taunt = 100, bool isSteal = false) -> void;
	auto dropMesos(Player *player, PacketReader &packet) -> void;
	auto petLoot(Player *player, PacketReader &packet) -> void;
	auto lootItem(Player *player, PacketReader &packet, int64_t petId = 0) -> void;
}