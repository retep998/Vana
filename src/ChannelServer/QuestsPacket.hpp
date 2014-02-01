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
struct ActiveQuest;

namespace QuestsPacket {
	enum ErrorCodes : int8_t {
		ErrorUnk = 0x09,
		ErrorNoItemSpace = 0x0A,
		ErrorNotEnoughMesos = 0x0B,
		ErrorEquipWorn = 0x0D,
		ErrorOnlyOne = 0x0E
	};

	PACKET(acceptQuest, int16_t questId, int32_t npcId);
	PACKET(acceptQuestNotice, int16_t questId);
	PACKET(completeQuestNotice, int16_t questId, int64_t time);
	PACKET(completeQuest, int16_t questId, int32_t npcId, int16_t nextQuest);
	SPLIT_PACKET(completeQuestAnimation, int32_t playerId);
	PACKET(updateQuest, const ActiveQuest &quest);
	PACKET(doneQuest, int16_t questId);
	PACKET(questError, int16_t questId, int8_t errorCode);
	PACKET(questExpire, int16_t questId);
	PACKET(forfeitQuest, int16_t questId);
	PACKET(giveItem, int32_t itemId, int32_t amount);
	PACKET(giveMesos, int32_t amount);
	PACKET(giveFame, int32_t amount);
}