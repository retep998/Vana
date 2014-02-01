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

namespace NpcHandler {
	auto handleNpc(Player *player, PacketReader &reader) -> void;
	auto handleNpcIn(Player *player, PacketReader &reader) -> void;
	auto handleNpcAnimation(Player *player, PacketReader &reader) -> void;
	auto handleQuestNpc(Player *player, int32_t npcId, bool start, int16_t questId = 0) -> void;
	auto useShop(Player *player, PacketReader &reader) -> void;
	auto useStorage(Player *player, PacketReader &reader) -> void;
	auto showShop(Player *player, int32_t shopId) -> bool;
	auto showStorage(Player *player, int32_t npcId) -> bool;
	auto showGuildRank(Player *player, int32_t npcId) -> bool;
}