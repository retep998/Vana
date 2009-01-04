/*
Copyright (C) 2008-2009 Vana Development Team

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
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::vector;

void QuestsPacket::acceptQuest(Player *player, int16_t questid, int32_t npcid) {
	PacketCreator packet;
	packet.addShort(SEND_NOTE);
	packet.addByte(1);
	packet.addShort(questid);
	packet.addByte(1);
	packet.addInt(0);
	packet.addInt(0);
	packet.addShort(0);
	player->getSession()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_UPDATE_QUEST);
	packet.addByte(8);
	packet.addShort(questid);
	packet.addInt(npcid);
	packet.addInt(0);
	player->getSession()->send(packet);
}

void QuestsPacket::updateQuest(Player *player, const Quest &quest) {
	PacketCreator packet;
	packet.addShort(SEND_NOTE);
	packet.addByte(1);
	packet.addShort(quest.id);
	packet.addByte(1);

	std::ostringstream info;
	for (size_t i = 0; i < quest.mobs.size(); i++) {
		info << std::setw(3) << std::setfill('0') << quest.mobs[i].count << '\0';
	}
	packet.addString(info.str());
	packet.addInt(0);
	packet.addInt(0);
	player->getSession()->send(packet);
}

void QuestsPacket::doneQuest(Player *player, int16_t questid) {
	PacketCreator packet;
	packet.addShort(SEND_FINISH_QUEST);
	packet.addShort(questid);
	player->getSession()->send(packet);
}

void QuestsPacket::questFinish(Player *player, int16_t questid, int32_t npcid, int16_t nextquest, int64_t time) {
	PacketCreator packet;
	packet.addShort(SEND_NOTE);
	packet.addByte(1);
	packet.addShort(questid);
	packet.addByte(2);
	packet.addInt64(time);
	player->getSession()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_UPDATE_QUEST);
	packet.addByte(8);
	packet.addShort(questid);
	packet.addInt(npcid);
	packet.addShort(nextquest);
	player->getSession()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_GAIN_ITEM);
	packet.addByte(9);
	player->getSession()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_SHOW_SKILL);
	packet.addInt(player->getId());
	packet.addByte(9);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void QuestsPacket::giveItem(Player *player, int32_t itemid, int32_t amount) {
	PacketCreator packet;
	packet.addShort(SEND_GAIN_ITEM);
	packet.addByte(3);
	packet.addByte(1); // Number of different items (itemid and amount gets repeated)
	packet.addInt(itemid);
	packet.addInt(amount);
	player->getSession()->send(packet);
}

void QuestsPacket::giveMesos(Player *player, int32_t amount) {
	PacketCreator packet;
	packet.addShort(SEND_NOTE);
	packet.addByte(5);
	packet.addInt(amount);
	player->getSession()->send(packet);
}
