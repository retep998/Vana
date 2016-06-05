/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "Common/FileTime.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/SplitPacketBuilder.hpp"
#include "Common/Types.hpp"

namespace vana {
	namespace channel_server {
		class player;
		struct active_quest;

		namespace packets {
			namespace quests {
				enum error_codes : int8_t {
					error_unk = 0x09,
					error_no_item_space = 0x0A,
					error_not_enough_mesos = 0x0B,
					error_equip_worn = 0x0D,
					error_only_one = 0x0E
				};

				PACKET(accept_quest, game_quest_id quest_id, game_npc_id npc_id);
				PACKET(accept_quest_notice, game_quest_id quest_id);
				PACKET(complete_quest_notice, game_quest_id quest_id, file_time time);
				PACKET(complete_quest, game_quest_id quest_id, game_npc_id npc_id, game_quest_id next_quest);
				SPLIT_PACKET(complete_quest_animation, game_player_id player_id);
				PACKET(update_quest, const active_quest &quest);
				PACKET(done_quest, game_quest_id quest_id);
				PACKET(quest_error, game_quest_id quest_id, int8_t error_code);
				PACKET(quest_expire, game_quest_id quest_id);
				PACKET(forfeit_quest, game_quest_id quest_id);
				PACKET(give_item, game_item_id item_id, game_slot_qty amount);
				PACKET(give_mesos, game_mesos amount);
				PACKET(give_fame, game_fame amount);
			}
		}
	}
}