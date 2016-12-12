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

class PacketReader;
class Player;
struct Point;

namespace DropHandler {
	auto doDrops(player_id_t playerId, map_id_t mapId, int32_t droppingLevel, int32_t droppingId, const Point &origin, bool explosive, bool ffa, int32_t taunt = 100, bool isSteal = false) -> void;
	auto dropMesos(Player *player, PacketReader &reader) -> void;
	auto petLoot(Player *player, PacketReader &reader) -> void;
	auto lootItem(Player *player, PacketReader &reader, pet_id_t petId = 0) -> void;
}