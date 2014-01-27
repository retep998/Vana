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
#include "Quests.hpp"
#include "ChannelServer.hpp"
#include "Inventory.hpp"
#include "ItemDataProvider.hpp"
#include "NpcDataProvider.hpp"
#include "NpcHandler.hpp"
#include "Player.hpp"
#include "QuestDataProvider.hpp"
#include "QuestsPacket.hpp"
#include "PacketReader.hpp"

namespace QuestOpcodes {
	enum : int8_t {
		RestoreLostQuestItem = 0x00,
		StartQuest = 0x01,
		FinishQuest = 0x02,
		ForfeitQuest = 0x03,
		StartNpcQuestChat = 0x04,
		EndNpcQuestChat = 0x05
	};
}

auto Quests::giveItem(Player *player, int32_t itemId, int16_t amount) -> bool {
	// TODO: Clean it up
	QuestsPacket::giveItem(player, itemId, amount);
	if (amount > 0) {
		Inventory::addNewItem(player, itemId, amount);
	}
	else {
		if (player->getInventory()->getItemAmount(itemId) < amount) {
			// Player does not have (enough of) what is being taken
			return false;
		}
		Inventory::takeItem(player, itemId, -amount);
	}
	return true;
}

auto Quests::giveMesos(Player *player, int32_t amount) -> bool {
	if (amount < 0 && player->getInventory()->getMesos() + amount < 0) {
		// Do a bit of checking if meso is being taken to see if it's enough
		return false;
	}
	player->getInventory()->modifyMesos(amount);
	QuestsPacket::giveMesos(player, amount);
	return true;
}

auto Quests::giveFame(Player *player, int32_t amount) -> void {
	player->getStats()->setFame(player->getStats()->getFame() + static_cast<int16_t>(amount));
	QuestsPacket::giveFame(player, amount);
}

auto Quests::getQuest(Player *player, PacketReader &packet) -> void {
	int8_t act = packet.get<int8_t>();
	int16_t questId = packet.get<int16_t>();

	if (!QuestDataProvider::getInstance().isQuest(questId)) {
		// Hacking
		return;
	}
	if (act == QuestOpcodes::ForfeitQuest) {
		if (player->getQuests()->isQuestActive(questId)) {
			player->getQuests()->removeQuest(questId);
		}
		else {
			out_stream_t x;
			x << "Player (ID: " << player->getId()
				<< ", Name: " << player->getName()
				<< ") tried to forfeit a quest that wasn't started yet."
				<< " (Quest ID: " << questId << ")";
			ChannelServer::getInstance().log(LogTypes::MalformedPacket, x.str());
		}
		return;
	}

	int32_t npcId = packet.get<int32_t>();
	if (act != QuestOpcodes::StartQuest && act != QuestOpcodes::StartNpcQuestChat) {
		if (!player->getQuests()->isQuestActive(questId)) {
			// Hacking
			out_stream_t x;
			x << "Player (ID: " << player->getId()
				<< ", Name: " << player->getName()
				<< ") tried to perform an action with a non-started quest."
				<< " (NPC ID: " << npcId
				<< ", Quest ID: " << questId << ")";
			ChannelServer::getInstance().log(LogTypes::MalformedPacket, x.str());
			return;
		}
	}
	// QuestOpcodes::RestoreLostQuestItem for some reason appears to use "NPC ID" as a different kind of identifier, maybe quantity?
	if (act != QuestOpcodes::RestoreLostQuestItem && !NpcDataProvider::getInstance().isValidNpcId(npcId)) {
		out_stream_t x;
		x << "Player (ID: " << player->getId()
			<< ", Name: " << player->getName()
			<< ") tried to do a quest action with an invalid NPC ID."
			<< " (NPC ID: " << npcId
			<< ", Quest ID: " << questId << ")";
		ChannelServer::getInstance().log(LogTypes::MalformedPacket, x.str());
		return;
	}
	switch (act) {
		case QuestOpcodes::RestoreLostQuestItem: {
			int32_t itemId = packet.get<int32_t>();
			if (ItemDataProvider::getInstance().isQuest(itemId)) {
				QuestsPacket::giveItem(player, itemId, 1);
				Inventory::addNewItem(player, itemId, 1);
			}
			else {
				out_stream_t x;
				x << "Player (ID: " << player->getId()
					<< ", Name: " << player->getName()
					<< ") tried to restore a lost quest item which isn't a quest item."
					<< " (Item ID: " << itemId
					<< ", NPC ID: " << npcId
					<< ", Quest ID: " << questId << ")";
				ChannelServer::getInstance().log(LogTypes::MalformedPacket, x.str());
			}
			break;
		}
		case QuestOpcodes::StartQuest:
			if (player->getQuests()->isQuestActive(questId)) {
				out_stream_t x;
				x << "Player (ID: " << player->getId()
					<< ", Name: " << player->getName()
					<< ") tried to start an already started quest."
					<< " (NPC ID: " << npcId
					<< ", Quest ID: " << questId << ")";
				ChannelServer::getInstance().log(LogTypes::MalformedPacket, x.str());
			}
			else {
				player->getQuests()->addQuest(questId, npcId);
			}
			break;
		case QuestOpcodes::FinishQuest:
			player->getQuests()->finishQuest(questId, npcId);
			break;
		case QuestOpcodes::StartNpcQuestChat:
		case QuestOpcodes::EndNpcQuestChat:
			NpcHandler::handleQuestNpc(player, npcId, act == QuestOpcodes::StartNpcQuestChat, questId);
			break;
	}
}