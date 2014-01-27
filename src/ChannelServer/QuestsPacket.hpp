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

class Player;
struct ActiveQuest;

namespace QuestsPacket {
	enum ErrorCodes : int8_t {
		ErrorUnk = 0x09,
		ErrorNoItemSpace = 0x0A,
		ErrorNotEnoughMesos = 0x0B,
		ErrorEquipWorn = 0x0D,
		ErrorOnlyOne = 0x0E
	};
	auto acceptQuest(Player *player, int16_t questId, int32_t npcId) -> void;
	auto updateQuest(Player *player, const ActiveQuest &quest) -> void;
	auto doneQuest(Player *player, int16_t questId) -> void;
	auto questError(Player *player, int16_t questId, int8_t errorCode) -> void;
	auto questExpire(Player *player, int16_t questId) -> void;
	auto questFinish(Player *player, int16_t questId, int32_t npcId, int16_t nextQuest, int64_t time) -> void;
	auto forfeitQuest(Player *player, int16_t questId) -> void;
	auto giveItem(Player *player, int32_t itemId, int32_t amount) -> void;
	auto giveMesos(Player *player, int32_t amount) -> void;
	auto giveFame(Player *player, int32_t amount) -> void;
}