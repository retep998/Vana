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
#include "PacketCreator.hpp"
#include "Player.hpp"
#include "Quests.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include <string>
#include <vector>

auto QuestsPacket::acceptQuest(Player *player, int16_t questId, int32_t npcId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_NOTICE);
	packet.add<int8_t>(1);
	packet.add<int16_t>(questId);
	packet.add<int8_t>(1);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<header_t>(SMSG_QUEST_UPDATE);
	packet.add<int8_t>(8);
	packet.add<int16_t>(questId);
	packet.add<int32_t>(npcId);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

auto QuestsPacket::updateQuest(Player *player, const ActiveQuest &quest) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_NOTICE);
	packet.add<int8_t>(1);
	packet.add<int16_t>(quest.id);
	packet.add<int8_t>(1);
	packet.addString(quest.getQuestData());
	player->getSession()->send(packet);
}

auto QuestsPacket::doneQuest(Player *player, int16_t questId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_QUEST_COMPLETED);
	packet.add<int16_t>(questId);
	player->getSession()->send(packet);
}

auto QuestsPacket::questError(Player *player, int16_t questId, int8_t errorCode) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_QUEST_UPDATE);
	packet.add<int8_t>(errorCode);
	packet.add<int16_t>(questId);
	player->getSession()->send(packet);
}

auto QuestsPacket::questExpire(Player *player, int16_t questId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_QUEST_UPDATE);
	packet.add<int8_t>(0x0F);
	packet.add<int16_t>(questId);
	player->getSession()->send(packet);
}

auto QuestsPacket::questFinish(Player *player, int16_t questId, int32_t npcId, int16_t nextQuest, int64_t time) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_NOTICE);
	packet.add<int8_t>(1);
	packet.add<int16_t>(questId);
	packet.add<int8_t>(2);
	packet.add<int64_t>(time);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<header_t>(SMSG_QUEST_UPDATE);
	packet.add<int8_t>(8);
	packet.add<int16_t>(questId);
	packet.add<int32_t>(npcId);
	packet.add<int16_t>(nextQuest);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<header_t>(SMSG_THEATRICS);
	packet.add<int8_t>(9);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.add<header_t>(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(9);
	player->getMap()->sendPacket(packet, player);
}

auto QuestsPacket::forfeitQuest(Player *player, int16_t questId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_NOTICE);
	packet.add<int8_t>(1);
	packet.add<int16_t>(questId);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

auto QuestsPacket::giveItem(Player *player, int32_t itemId, int32_t amount) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_THEATRICS);
	packet.add<int8_t>(3);
	packet.add<int8_t>(1); // Number of different items (itemId and amount gets repeated)
	packet.add<int32_t>(itemId);
	packet.add<int32_t>(amount);
	player->getSession()->send(packet);
}

auto QuestsPacket::giveMesos(Player *player, int32_t amount) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_NOTICE);
	packet.add<int8_t>(5);
	packet.add<int32_t>(amount);
	player->getSession()->send(packet);
}

auto QuestsPacket::giveFame(Player *player, int32_t amount) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_NOTICE);
	packet.add<int8_t>(4);
	packet.add<int32_t>(amount);
	player->getSession()->send(packet);
}