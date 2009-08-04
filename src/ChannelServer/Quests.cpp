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
#include "Quests.h"
#include "Inventory.h"
#include "NPCs.h"
#include "Player.h"
#include "QuestDataProvider.h"
#include "QuestsPacket.h"
#include "PacketReader.h"

bool Quests::giveItem(Player *player, int32_t itemid, int16_t amount) {
	//Temp
	QuestsPacket::giveItem(player, itemid, amount);
	if (amount > 0) {
		Inventory::addNewItem(player, itemid, amount);
	}
	else {
		if (player->getInventory()->getItemAmount(itemid) < amount) { // Player does not have (enough of) what is being taken
			return false;
		}
		Inventory::takeItem(player, itemid, -amount);
	}
	return true;
}

bool Quests::giveMesos(Player *player, int32_t amount) {
	if (amount < 0 && player->getInventory()->getMesos() + amount < 0) { // Do a bit of checking if meso is being taken to see if it's enough
		return false;
	}
	player->getInventory()->modifyMesos(amount);
	QuestsPacket::giveMesos(player, amount);
	return true;
}

void Quests::giveFame(Player *player, int32_t amount) {
	player->getStats()->setFame(player->getStats()->getBaseStat(Stats::Fame) + static_cast<int16_t>(amount));
	QuestsPacket::giveFame(player, amount);
}

void Quests::getQuest(Player *player, PacketReader &packet) {
	int8_t act = packet.get<int8_t>();
	int16_t questid = packet.get<int16_t>();

	if (!QuestDataProvider::Instance()->isQuest(questid)) {
		// Hacking
		return;
	}
	if (act == 3) {
		player->getQuests()->removeQuest(questid);
		return;
	}

	int32_t npcid = packet.get<int32_t>();
	if (act == 0) {	
		// Absolutely no idea what this does
		int32_t item = packet.get<int32_t>();
		QuestsPacket::giveItem(player, item, (int16_t) npcid);
		Inventory::addNewItem(player, item, (int16_t) npcid);
	}
	else if (act == 1) {
		player->getQuests()->addQuest(questid, npcid);
	}
	else if (act == 2) {
		player->getQuests()->finishQuest(questid, npcid);
	}
	else if (act == 4) {
		NPCs::handleQuestNPC(player, npcid, true, questid);
	}
	else if (act == 5) {
		NPCs::handleQuestNPC(player, npcid, false, questid);
	}
}
