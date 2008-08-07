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
#include "Quests.h"
#include "Inventory.h"
#include "NPCs.h"
#include "Player.h"
#include "QuestsPacket.h"
#include "ReadPacket.h"

hash_map <int, QuestInfo> Quests::quests;

void Quests::addRequest(int id, QuestRequestsInfo request) {		
	QuestInfo quest;
	if (quests.find(id) != quests.end())
		quest = quests[id];
	quest.requests = request;
	quests[id] = quest;
}

void Quests::addReward(int id, QuestRewardsInfo raws) {		
	QuestInfo quest;
	if (quests.find(id) != quests.end())
		quest = quests[id];
	quest.rewards = raws;
	quests[id] = quest;
}

void Quests::setNextQuest(int id, int questid) {		
	QuestInfo quest;
	if (quests.find(id) != quests.end())
		quest = quests[id];
	quest.nextquest = questid;
	quests[id] = quest;
}

bool Quests::giveItem(Player *player, int itemid, int amount) {
	//Temp
	QuestsPacket::giveItem(player, itemid, amount);
	if (amount > 0) {
		Inventory::addNewItem(player, itemid, amount);
	}
	else {
		if (player->inv->getItemAmount(itemid) < amount) { // Player does not have (enough of) what is being taken
			return false;
		}
		Inventory::takeItem(player, itemid, -amount);
	}

	return true;
}

bool Quests::giveMesos(Player *player, int amount) {
	if (amount < 0 && player->inv->getMesos() + amount < 0) { // Do a bit of checking if meso is being taken to see if it's enough
		return false;
	}
	player->inv->setMesos(player->inv->getMesos()+amount);
	QuestsPacket::giveMesos(player, amount);

	return true;
}

void Quests::getQuest(Player *player, ReadPacket *packet) {
	char act = packet->getByte();
	short questid = packet->getShort();
	int npcid = packet->getInt();
	if (act == 0) {	
		int item = packet->getInt();
		QuestsPacket::giveItem(player, item, npcid);
		Inventory::addNewItem(player, item, npcid);
	}
	else if (act == 1) {
		player->quests->addQuest(questid, npcid);
	}
	else if (act == 2) {
		player->quests->finishQuest(questid, npcid);
	}
	else if (act == 4) {
		NPCs::handleQuestNPC(player, npcid, 1);
	}
	else if (act == 5) {
		NPCs::handleQuestNPC(player, npcid, 0);
	}
}
