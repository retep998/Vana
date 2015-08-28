/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "FileTime.hpp"
#include "PacketBuilder.hpp"
#include "SplitPacketBuilder.hpp"
#include "Types.hpp"

namespace Vana {
	class Player;
	struct ActiveQuest;

	namespace Packets {
		namespace Quests {
			enum ErrorCodes : int8_t {
				ErrorUnk = 0x09,
				ErrorNoItemSpace = 0x0A,
				ErrorNotEnoughMesos = 0x0B,
				ErrorEquipWorn = 0x0D,
				ErrorOnlyOne = 0x0E
			};

			PACKET(acceptQuest, quest_id_t questId, npc_id_t npcId);
			PACKET(acceptQuestNotice, quest_id_t questId);
			PACKET(completeQuestNotice, quest_id_t questId, FileTime time);
			PACKET(completeQuest, quest_id_t questId, npc_id_t npcId, quest_id_t nextQuest);
			SPLIT_PACKET(completeQuestAnimation, player_id_t playerId);
			PACKET(updateQuest, const ActiveQuest &quest);
			PACKET(doneQuest, quest_id_t questId);
			PACKET(questError, quest_id_t questId, int8_t errorCode);
			PACKET(questExpire, quest_id_t questId);
			PACKET(forfeitQuest, quest_id_t questId);
			PACKET(giveItem, item_id_t itemId, slot_qty_t amount);
			PACKET(giveMesos, mesos_t amount);
			PACKET(giveFame, fame_t amount);
		}
	}
}