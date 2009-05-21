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
#include "PlayerQuests.h"
#include "Inventory.h"
#include "Levels.h"
#include "Player.h"
#include "QuestsPacket.h"
#include "Randomizer.h"
#include "TimeUtilities.h"

void PlayerQuests::addQuest(int16_t questid, int32_t npcid) {
	QuestsPacket::acceptQuest(player, questid, npcid);
	Quest quest;
	quest.id = questid;
	quest.done = false;
	QuestInfo &questinfo = Quests::quests[questid];
	for (size_t i = 0; i < questinfo.requests.size(); i++) {
		if (questinfo.requests[i].ismob) {
			QuestMob mob;
			mob.id = questinfo.requests[i].id;
			mob.maxcount = questinfo.requests[i].count;
			quest.mobs.push_back(mob);
		}
	}
	for (size_t i = 0; i < questinfo.rewards.size(); i++) {
		if (!questinfo.rewards[i].start) {
			if (questinfo.rewards[i].isexp) {
				Levels::giveEXP(player, questinfo.rewards[i].id, true);
			}
			else if (questinfo.rewards[i].isitem) {
				if (questinfo.rewards[i].count > 0) {
					QuestsPacket::giveItem(player, questinfo.rewards[i].id, questinfo.rewards[i].count);
					Inventory::addNewItem(player, questinfo.rewards[i].id, questinfo.rewards[i].count);
				}
				else if (questinfo.rewards[i].count < 0) {
					QuestsPacket::giveItem(player, questinfo.rewards[i].id, questinfo.rewards[i].count);
					Inventory::takeItem(player, questinfo.rewards[i].id, questinfo.rewards[i].count);
				}
				else if (questinfo.rewards[i].id > 0) {
					QuestsPacket::giveItem(player, questinfo.rewards[i].id, -player->getInventory()->getItemAmount(questinfo.rewards[i].id));
					Inventory::takeItem(player, questinfo.rewards[i].id, player->getInventory()->getItemAmount(questinfo.rewards[i].id));
				}
			}
			else if (questinfo.rewards[i].ismesos) {
				player->getInventory()->modifyMesos(questinfo.rewards[i].id);
				QuestsPacket::giveMesos(player, questinfo.rewards[i].id);
			}
		}
	}
	checkDone(quest);
	quests[questid] = quest;
}

void PlayerQuests::updateQuestMob(int32_t mobid) {
	for (unordered_map<int32_t, Quest>::iterator iter = quests.begin(); iter != quests.end(); iter++) {
		for (size_t i = 0; i < iter->second.mobs.size(); i ++) {
			int16_t maxcount = iter->second.mobs[i].maxcount;
			if (iter->second.mobs[i].id == mobid && !iter->second.done && iter->second.mobs[i].count < maxcount) {
				iter->second.mobs[i].count ++;
				QuestsPacket::updateQuest(player, iter->second);
				if (iter->second.mobs[i].count == maxcount) {
					checkDone(iter->second);
				}
			}
		}
	}
}

void PlayerQuests::checkDone(Quest &quest) {
	QuestInfo &questinfo = Quests::quests[quest.id];
	quest.done = true;
	if (questinfo.requests.size() == 0) {
		return;
	}
	for (uint32_t i = 0; i < questinfo.requests.size(); i++) {
		if (questinfo.requests[i].isitem) {
			if ((player->getInventory()->getItemAmount(questinfo.requests[i].id) < questinfo.requests[i].count && questinfo.requests[i].count > 0) || (questinfo.requests[i].count == 0 && player->getInventory()->getItemAmount(questinfo.requests[i].id) != 0)) {
				quest.done = false;
				break;
			}
		}
		else if (questinfo.requests[i].ismob) {
			int32_t killed = 0;
			for (uint32_t j = 0; j < quest.mobs.size(); j++) {
				if (quest.mobs[j].id == questinfo.requests[i].id) {
					killed = quest.mobs[j].count;
					break;
				}
			}
			if (killed < questinfo.requests[i].count) {
				quest.done = false;
				break;
			}
		}
	}
	if (quest.done) {
		QuestsPacket::doneQuest(player, quest.id);
	}
}

void PlayerQuests::finishQuest(int16_t questid, int32_t npcid) {
	QuestInfo &questinfo = Quests::quests[questid];
	int32_t chance = 0;
	for (size_t i = 0; i < questinfo.rewards.size(); i++) {
		if (questinfo.rewards[i].start) {
			if (questinfo.rewards[i].isexp) {
				Levels::giveEXP(player, questinfo.rewards[i].id * ChannelServer::Instance()->getQuestExprate(), true);
			}
			else if (questinfo.rewards[i].isitem) {
				if (questinfo.rewards[i].prop == 0) {
					if (questinfo.rewards[i].count > 0) {
						QuestsPacket::giveItem(player, questinfo.rewards[i].id, questinfo.rewards[i].count);
						Inventory::addNewItem(player, questinfo.rewards[i].id, questinfo.rewards[i].count);
					}
					else if (questinfo.rewards[i].count < 0) {
						QuestsPacket::giveItem(player, questinfo.rewards[i].id, questinfo.rewards[i].count);
						Inventory::takeItem(player, questinfo.rewards[i].id, -questinfo.rewards[i].count);
					}
					else {
						QuestsPacket::giveItem(player, questinfo.rewards[i].id, -player->getInventory()->getItemAmount(questinfo.rewards[i].id));
						Inventory::takeItem(player, questinfo.rewards[i].id, player->getInventory()->getItemAmount(questinfo.rewards[i].id));
					}
				}
				else if (questinfo.rewards[i].prop > 0) {
					chance += questinfo.rewards[i].prop;
				}
			}
			else if (questinfo.rewards[i].ismesos && questinfo.rewards[i].id > 0) {
				player->getInventory()->modifyMesos(questinfo.rewards[i].id);
				QuestsPacket::giveMesos(player, questinfo.rewards[i].id);
			}
		}
	}
	if (chance > 0) {
		int32_t random = Randomizer::Instance()->randInt(chance - 1);
		chance = 0;
		for (size_t i = 0; i < questinfo.rewards.size(); i++) {
			if (questinfo.rewards[i].start) {
				if (questinfo.rewards[i].isitem) {
					if (questinfo.rewards[i].prop > 0) {
						if (chance >= random) {
							QuestsPacket::giveItem(player, questinfo.rewards[i].id, questinfo.rewards[i].count);
							if (questinfo.rewards[i].count > 0)
								Inventory::addNewItem(player, questinfo.rewards[i].id, questinfo.rewards[i].count);
							else
								Inventory::takeItem(player, questinfo.rewards[i].id, -questinfo.rewards[i].count);
							break;
						}
						else
							chance += questinfo.rewards[i].prop;
					}
				}
			}
		}
	}
	quests.erase(questid);
	QuestComp complete(questid, TimeUtilities::getServerTime());
	completed.push_back(complete);
	QuestsPacket::questFinish(player, questid, npcid, questinfo.nextquest, complete.time);
}

bool PlayerQuests::isQuestActive(int16_t questid) {
	return quests.find(questid) != quests.end();
}
