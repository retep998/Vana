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
#include "QuestsPacket.hpp"
#include "common/Session.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/Quests.hpp"
#include "channel_server/SmsgHeader.hpp"
#include <string>
#include <vector>

namespace vana {
namespace channel_server {
namespace packets {
namespace quests {

PACKET_IMPL(accept_quest, game_quest_id quest_id, game_npc_id npc_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_QUEST_UPDATE)
		.add<int8_t>(8)
		.add<game_quest_id>(quest_id)
		.add<game_npc_id>(npc_id)
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(accept_quest_notice, game_quest_id quest_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<game_quest_id>(quest_id)
		.unk<int8_t>(1)
		.unk<int32_t>()
		.unk<int32_t>()
		.unk<int16_t>();
	return builder;
}

PACKET_IMPL(complete_quest_notice, game_quest_id quest_id, file_time time) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<game_quest_id>(quest_id)
		.unk<int8_t>(2)
		.add<file_time>(time);
	return builder;
}

PACKET_IMPL(complete_quest, game_quest_id quest_id, game_npc_id npc_id, game_quest_id next_quest) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_QUEST_UPDATE)
		.add<int8_t>(8)
		.add<game_quest_id>(quest_id)
		.add<game_npc_id>(npc_id)
		.add<game_quest_id>(next_quest);
	return builder;
}

SPLIT_PACKET_IMPL(complete_quest_animation, game_player_id player_id) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_THEATRICS)
		.add<int8_t>(9);

	builder.map
		.add<packet_header>(SMSG_SKILL_SHOW)
		.add<game_player_id>(player_id)
		.add<int8_t>(9);
	return builder;
}

PACKET_IMPL(update_quest, const active_quest &quest) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<game_quest_id>(quest.id)
		.unk<int8_t>(1)
		.add<string>(quest.get_quest_data());
	return builder;
}

PACKET_IMPL(done_quest, game_quest_id quest_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_QUEST_COMPLETED)
		.add<game_quest_id>(quest_id);
	return builder;
}

PACKET_IMPL(quest_error, game_quest_id quest_id, int8_t error_code) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_QUEST_UPDATE)
		.add<int8_t>(error_code)
		.add<game_quest_id>(quest_id);
	return builder;
}

PACKET_IMPL(quest_expire, game_quest_id quest_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_QUEST_UPDATE)
		.add<int8_t>(0x0F)
		.add<game_quest_id>(quest_id);
	return builder;
}

PACKET_IMPL(forfeit_quest, game_quest_id quest_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<game_quest_id>(quest_id)
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(give_item, game_item_id item_id, game_slot_qty amount) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_THEATRICS)
		.add<int8_t>(3)
		.add<int8_t>(1) // Number of different items (itemId and amount gets repeated)
		.add<game_item_id>(item_id)
		.add<int32_t>(amount);
	return builder;
}

PACKET_IMPL(give_mesos, game_mesos amount) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(5)
		.add<game_mesos>(amount);
	return builder;
}

PACKET_IMPL(give_fame, game_fame amount) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(4)
		.add<int32_t>(amount);
	return builder;
}

}
}
}
}