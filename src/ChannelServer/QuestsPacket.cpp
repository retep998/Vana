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

PACKET_IMPL(acceptQuest, int16_t questId, int32_t npcId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_UPDATE)
		.add<int8_t>(8)
		.add<int16_t>(questId)
		.add<int32_t>(npcId)
		.add<int32_t>(0);
	return builder;
}

PACKET_IMPL(acceptQuestNotice, int16_t questId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<int16_t>(questId)
		.add<int8_t>(1)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int16_t>(0);
	return builder;
}

PACKET_IMPL(completeQuestNotice, int16_t questId, int64_t time) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<int16_t>(questId)
		.add<int8_t>(2)
		.add<int64_t>(time);
	return builder;
}

PACKET_IMPL(completeQuest, int16_t questId, int32_t npcId, int16_t nextQuest) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_UPDATE)
		.add<int8_t>(8)
		.add<int16_t>(questId)
		.add<int32_t>(npcId)
		.add<int16_t>(nextQuest);
	return builder;
}

SPLIT_PACKET_IMPL(completeQuestAnimation, int32_t playerId) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_THEATRICS)
		.add<int8_t>(9);

	builder.map
		.add<header_t>(SMSG_SKILL_SHOW)
		.add<int32_t>(playerId)
		.add<int8_t>(9);
	return builder;
}

PACKET_IMPL(updateQuest, const ActiveQuest &quest) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<int16_t>(quest.id)
		.add<int8_t>(1)
		.add<string_t>(quest.getQuestData());
	return builder;
}

PACKET_IMPL(doneQuest, int16_t questId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_COMPLETED)
		.add<int16_t>(questId);
	return builder;
}

PACKET_IMPL(questError, int16_t questId, int8_t errorCode) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_UPDATE)
		.add<int8_t>(errorCode)
		.add<int16_t>(questId);
	return builder;
}

PACKET_IMPL(questExpire, int16_t questId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_QUEST_UPDATE)
		.add<int8_t>(0x0F)
		.add<int16_t>(questId);
	return builder;
}

PACKET_IMPL(forfeitQuest, int16_t questId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(1)
		.add<int16_t>(questId)
		.add<int8_t>(0);
	return builder;
}

PACKET_IMPL(giveItem, int32_t itemId, int32_t amount) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_THEATRICS)
		.add<int8_t>(3)
		.add<int8_t>(1) // Number of different items (itemId and amount gets repeated)
		.add<int32_t>(itemId)
		.add<int32_t>(amount);
	return builder;
}

PACKET_IMPL(giveMesos, int32_t amount) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(5)
		.add<int32_t>(amount);
	return builder;
}

PACKET_IMPL(giveFame, int32_t amount) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_NOTICE)
		.add<int8_t>(4)
		.add<int32_t>(amount);
	return builder;
}

}