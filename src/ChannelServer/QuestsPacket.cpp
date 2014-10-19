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
#include "QuestsPacket.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "Quests.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include <string>
#include <vector>

namespace QuestsPacket {

PACKET_IMPL(acceptQuest, quest_id_t questId, npc_id_t npcId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_UPDATE)
		.add<int8_t>(8)
		.add<quest_id_t>(questId)
		.add<npc_id_t>(npcId)
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(acceptQuestNotice, quest_id_t questId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<quest_id_t>(questId)
		.unk<int8_t>(1)
		.unk<int32_t>()
		.unk<int32_t>()
		.unk<int16_t>();
	return builder;
}

PACKET_IMPL(completeQuestNotice, quest_id_t questId, int64_t time) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<quest_id_t>(questId)
		.unk<int8_t>(2)
		.add<int64_t>(time);
	return builder;
}

PACKET_IMPL(completeQuest, quest_id_t questId, npc_id_t npcId, quest_id_t nextQuest) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_UPDATE)
		.add<int8_t>(8)
		.add<quest_id_t>(questId)
		.add<npc_id_t>(npcId)
		.add<quest_id_t>(nextQuest);
	return builder;
}

SPLIT_PACKET_IMPL(completeQuestAnimation, player_id_t playerId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.add<int8_t>(9);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<player_id_t>(playerId)
		.add<int8_t>(9);
	return builder;
}

PACKET_IMPL(updateQuest, const ActiveQuest &quest) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<quest_id_t>(quest.id)
		.unk<int8_t>(1)
		.add<string_t>(quest.getQuestData());
	return builder;
}

PACKET_IMPL(doneQuest, quest_id_t questId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_COMPLETED)
		.add<quest_id_t>(questId);
	return builder;
}

PACKET_IMPL(questError, quest_id_t questId, int8_t errorCode) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_UPDATE)
		.add<int8_t>(errorCode)
		.add<quest_id_t>(questId);
	return builder;
}

PACKET_IMPL(questExpire, quest_id_t questId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_UPDATE)
		.add<int8_t>(0x0F)
		.add<quest_id_t>(questId);
	return builder;
}

PACKET_IMPL(forfeitQuest, quest_id_t questId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<quest_id_t>(questId)
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(giveItem, item_id_t itemId, slot_qty_t amount) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_THEATRICS)
		.add<int8_t>(3)
		.add<int8_t>(1) // Number of different items (itemId and amount gets repeated)
		.add<item_id_t>(itemId)
		.add<int32_t>(amount);
	return builder;
}

PACKET_IMPL(giveMesos, mesos_t amount) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(5)
		.add<mesos_t>(amount);
	return builder;
}

PACKET_IMPL(giveFame, fame_t amount) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(4)
		.add<int32_t>(amount);
	return builder;
}

}