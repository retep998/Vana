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
#include "Quests.h"
#include "ChannelServer.h"
#include "Inventory.h"
#include "ItemDataProvider.h"
#include "NpcDataProvider.h"
#include "NpcHandler.h"
#include "Player.h"
#include "QuestDataProvider.h"
#include "QuestsPacket.h"
#include "PacketReader.h"

namespace QuestOpcodes {
	enum {
		RestoreLostQuestItem = 0x00,
		StartQuest = 0x01,
		FinishQuest = 0x02,
		ForfeitQuest = 0x03,
		StartNpcQuestChat = 0x04,
		EndNpcQuestChat = 0x05
	};
}

bool Quests::giveItem(Player *player, int32_t itemid, int16_t amount) {
	// Todo: clean it up!
	QuestsPacket::giveItem(player, itemid, amount);
	if (amount > 0) {
		Inventory::addNewItem(player, itemid, amount);
	}
	else {
		if (player->getInventory()->getItemAmount(itemid) < amount) {
			// Player does not have (enough of) what is being taken
			return false;
		}
		Inventory::takeItem(player, itemid, -amount);
	}
	return true;
}

bool Quests::giveMesos(Player *player, int32_t amount) {
	if (amount < 0 && player->getInventory()->getMesos() + amount < 0) {
		// Do a bit of checking if meso is being taken to see if it's enough
		return false;
	}
	player->getInventory()->modifyMesos(amount);
	QuestsPacket::giveMesos(player, amount);
	return true;
}

void Quests::giveFame(Player *player, int32_t amount) {
	player->getStats()->setFame(player->getStats()->getFame() + static_cast<int16_t>(amount));
	QuestsPacket::giveFame(player, amount);
}

void Quests::getQuest(Player *player, PacketReader &packet) {
	int8_t act = packet.get<int8_t>();
	int16_t questid = packet.get<int16_t>();

	if (!QuestDataProvider::Instance()->isQuest(questid)) {
		// Hacking
		return;
	}

	if (act == QuestOpcodes::ForfeitQuest) {
		if (player->getQuests()->isQuestActive(questid)) {
			player->getQuests()->removeQuest(questid);
		}
		else {
			std::stringstream x;
			x << "Player (ID: " << player->getId()
				<< ", Name: " << player->getName()
				<< ") tried to forfeit a quest that wasn't started yet."
				<< " (Quest ID: " << questid << ")";
			ChannelServer::Instance()->log(LogTypes::MalformedPacket, x.str());
		}
		return;
	}

	int32_t npcid = packet.get<int32_t>();
	if (act != QuestOpcodes::StartQuest && act != QuestOpcodes::StartNpcQuestChat) {
		if (!player->getQuests()->isQuestActive(questid)) {
			// Hacking
			std::stringstream x;
			x << "Player (ID: " << player->getId()
				<< ", Name: " << player->getName()
				<< ") tried to perform an action with a non-started quest."
				<< " (NPC ID: " << npcid
				<< ", Quest ID: " << questid << ")";
			ChannelServer::Instance()->log(LogTypes::MalformedPacket, x.str());
			return;
		}
	}

	if (!NpcDataProvider::Instance()->isValidNpcId(npcid)) {
		std::stringstream x;
		x << "Player (ID: " << player->getId()
			<< ", Name: " << player->getName()
			<< ") tried to do a quest action with an invalid NPC ID."
			<< " (NPC ID: " << npcid
			<< ", Quest ID: " << questid << ")";
		ChannelServer::Instance()->log(LogTypes::MalformedPacket, x.str());
		return;
	}

	switch (act) {
		case QuestOpcodes::RestoreLostQuestItem: {
			int32_t itemid = packet.get<int32_t>();
			if (ItemDataProvider::Instance()->isQuest(itemid)) {
				QuestsPacket::giveItem(player, itemid, 1);
				Inventory::addNewItem(player, itemid, 1);
			}
			else {
				std::stringstream x;
				x << "Player (ID: " << player->getId()
					<< ", Name: " << player->getName()
					<< ") tried to restore a lost quest item which isn't a quest item."
					<< " (Item ID: " << itemid
					<< ", NPC ID: " << npcid
					<< ", Quest ID: " << questid << ")";
				ChannelServer::Instance()->log(LogTypes::MalformedPacket, x.str());
			}
			break;
		}
		case QuestOpcodes::StartQuest:
			if (player->getQuests()->isQuestActive(questid)) {
				std::stringstream x;
				x << "Player (ID: " << player->getId()
					<< ", Name: " << player->getName()
					<< ") tried to start an already started quest."
					<< " (NPC ID: " << npcid
					<< ", Quest ID: " << questid << ")";
				ChannelServer::Instance()->log(LogTypes::MalformedPacket, x.str());
			}
			else {
				player->getQuests()->addQuest(questid, npcid);
			}
			break;
		case QuestOpcodes::FinishQuest:
			player->getQuests()->finishQuest(questid, npcid);
			break;
		case QuestOpcodes::StartNpcQuestChat:
		case QuestOpcodes::EndNpcQuestChat:
			NpcHandler::handleQuestNpc(player, npcid, act == QuestOpcodes::StartNpcQuestChat, questid);
			break;
	}
}
