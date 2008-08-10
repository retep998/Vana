/*
Copyright (C) 2008 Vana Development Team

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
#include "PacketCreator.h"
#include "Player.h"
#include "Quests.h"
#include "SendHeader.h"
#include "Maps.h"
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

using std::string;
using std::vector;

void QuestsPacket::acceptQuest(Player *player, short questid, int npcid) {
	PacketCreator packet;
	packet.addShort(SEND_NOTE);
	packet.addByte(1);
	packet.addShort(questid);
	packet.addByte(1);
	packet.addInt(0);
	packet.addInt(0);
	packet.addShort(0);
	player->getPacketHandler()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_UPDATE_QUEST);
	packet.addByte(8);
	packet.addShort(questid);
	packet.addInt(npcid);
	packet.addInt(0);
	player->getPacketHandler()->send(packet);
}

void QuestsPacket::updateQuest(Player *player, Quest quest) {
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
	player->getPacketHandler()->send(packet);
}

void QuestsPacket::doneQuest(Player *player, int questid) {
	PacketCreator packet;
	packet.addShort(SEND_FINISH_QUEST);
	packet.addShort(questid);
	player->getPacketHandler()->send(packet);
}

void QuestsPacket::questFinish(Player *player, short questid, int npcid, short nextquest, __int64 time) {
	PacketCreator packet;
	packet.addShort(SEND_NOTE);
	packet.addByte(1);
	packet.addShort(questid);
	packet.addByte(2);
	packet.addInt64(time);
	player->getPacketHandler()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_UPDATE_QUEST);
	packet.addByte(8);
	packet.addShort(questid); 
	packet.addInt(npcid); 
	packet.addShort(nextquest); 
	player->getPacketHandler()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_GAIN_ITEM);
	packet.addByte(9);
	player->getPacketHandler()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_SHOW_SKILL);
	packet.addInt(player->getPlayerid());
	packet.addByte(9);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void QuestsPacket::giveItem(Player *player, int itemid, int amount) {
	PacketCreator packet;
	packet.addShort(SEND_GAIN_ITEM); 
	packet.addByte(3);
	packet.addByte(1); // Number of different items (itemid and amount gets repeated)
	packet.addInt(itemid);
	packet.addInt(amount);
	player->getPacketHandler()->send(packet);
}

void QuestsPacket::giveMesos(Player *player, int amount) {
	PacketCreator packet;
	packet.addShort(SEND_NOTE);
	packet.addByte(5);
	packet.addInt(amount);
	player->getPacketHandler()->send(packet);
}
