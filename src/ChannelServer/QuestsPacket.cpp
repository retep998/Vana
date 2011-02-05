/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "QuestsPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Quests.h"
#include "SendHeader.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

void QuestsPacket::acceptQuest(Player *player, int16_t questid, int32_t npcid) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(1);
	packet.add<int16_t>(questid);
	packet.add<int8_t>(1);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_QUEST_UPDATE);
	packet.add<int8_t>(8);
	packet.add<int16_t>(questid);
	packet.add<int32_t>(npcid);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void QuestsPacket::updateQuest(Player *player, const ActiveQuest &quest) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(1);
	packet.add<int16_t>(quest.id);
	packet.add<int8_t>(1);
	packet.addString(quest.getQuestData());
	player->getSession()->send(packet);
}

void QuestsPacket::doneQuest(Player *player, int16_t questid) {
	PacketCreator packet;
	packet.addHeader(SMSG_QUEST_COMPLETED);
	packet.add<int16_t>(questid);
	player->getSession()->send(packet);
}

void QuestsPacket::questError(Player *player, int16_t questid, int8_t errorcode) {
	PacketCreator packet;
	packet.addHeader(SMSG_QUEST_UPDATE);
	packet.add<int8_t>(errorcode);
	packet.add<int16_t>(questid);
	player->getSession()->send(packet);
}

void QuestsPacket::questExpire(Player *player, int16_t questid) {
	PacketCreator packet;
	packet.addHeader(SMSG_QUEST_UPDATE);
	packet.add<int8_t>(0x0F);
	packet.add<int16_t>(questid);
	player->getSession()->send(packet);
}

void QuestsPacket::questFinish(Player *player, int16_t questid, int32_t npcid, int16_t nextquest, int64_t time) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(1);
	packet.add<int16_t>(questid);
	packet.add<int8_t>(2);
	packet.add<int64_t>(time);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_QUEST_UPDATE);
	packet.add<int8_t>(8);
	packet.add<int16_t>(questid);
	packet.add<int32_t>(npcid);
	packet.add<int16_t>(nextquest);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_THEATRICS);
	packet.add<int8_t>(9);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(9);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void QuestsPacket::forfeitQuest(Player *player, int16_t questid) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(1);
	packet.add<int16_t>(questid);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void QuestsPacket::giveItem(Player *player, int32_t itemid, int32_t amount) {
	PacketCreator packet;
	packet.addHeader(SMSG_THEATRICS);
	packet.add<int8_t>(3);
	packet.add<int8_t>(1); // Number of different items (itemid and amount gets repeated)
	packet.add<int32_t>(itemid);
	packet.add<int32_t>(amount);
	player->getSession()->send(packet);
}

void QuestsPacket::giveMesos(Player *player, int32_t amount) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(5);
	packet.add<int32_t>(amount);
	player->getSession()->send(packet);
}

void QuestsPacket::giveFame(Player *player, int32_t amount) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(4);
	packet.add<int32_t>(amount);
	player->getSession()->send(packet);
}