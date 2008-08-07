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
#include "PlayerQuests.h"
#include "Inventory.h"
#include "Levels.h"
#include "Player.h"
#include "Quests.h"
#include "QuestsPacket.h"
#include "Randomizer.h"
#include "TimeUtilities.h"

void PlayerQuests::addQuest(int questid, int npcid) {
	QuestsPacket::acceptQuest(player, questid, npcid);
	Quest quest;
	quest.id = questid;
	quest.done = 0;
	for (size_t i = 0; i < Quests::quests[questid].requests.size(); i++) {
		if (Quests::quests[questid].requests[i].ismob) {
			QuestMob mob;
			mob.id = Quests::quests[questid].requests[i].id;
			mob.count = 0;	
			quest.mobs.push_back(mob);
		}
	}
	for (size_t i = 0; i < Quests::quests[questid].rewards.size(); i++) {
		if (!Quests::quests[questid].rewards[i].start) {
			if (Quests::quests[questid].rewards[i].isexp) {
				Levels::giveEXP(player, Quests::quests[questid].rewards[i].id, 1);
			}
			else if (Quests::quests[questid].rewards[i].isitem) {
				if (Quests::quests[questid].rewards[i].count > 0) {
					QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
					Inventory::addNewItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
				}
				else if (Quests::quests[questid].rewards[i].count < 0) {
					QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
					Inventory::takeItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
				}
				else {
					QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, -player->getInventory()->getItemAmount(Quests::quests[questid].rewards[i].id));
					Inventory::takeItem(player, Quests::quests[questid].rewards[i].id, player->getInventory()->getItemAmount(Quests::quests[questid].rewards[i].id));
				}
			}
			else if (Quests::quests[questid].rewards[i].ismesos) {
				player->getInventory()->setMesos(player->getInventory()->getMesos() + Quests::quests[questid].rewards[i].id);
				QuestsPacket::giveMesos(player, Quests::quests[questid].rewards[i].id);
			}
		}
	}
	checkDone(quest);
	quests.push_back(quest);
}

void PlayerQuests::updateQuestMob(int mobid) {
	for (size_t i = 0; i < quests.size(); i++) {
		for (size_t j = 0; j < quests[i].mobs.size(); j++) {
			if (quests[i].mobs[j].id == mobid && !quests[i].done) {
				int maxcount = 0;
				for (size_t k = 0; k < Quests::quests[quests[i].id].requests.size(); k++) {
					if (Quests::quests[quests[i].id].requests[k].id == mobid) {
						maxcount = Quests::quests[quests[i].id].requests[k].count;
					}
				}
				if (quests[i].mobs[j].count < maxcount) {
					quests[i].mobs[j].count++;
					QuestsPacket::updateQuest(player, quests[i]);
					if (quests[i].mobs[j].count == maxcount) {
						checkDone(quests[i]);
					}
				}
			}
		}
	}
}

void PlayerQuests::checkDone(Quest &quest) {
	if (Quests::quests[quest.id].requests.size() == 0) {
		quest.done = 1;
		return;
	}
	int is = 1;
	for (unsigned int i=0; i<Quests::quests[quest.id].requests.size(); i++) {
		if (Quests::quests[quest.id].requests[i].isitem) {
			if ((player->getInventory()->getItemAmount(Quests::quests[quest.id].requests[i].id) < Quests::quests[quest.id].requests[i].count && Quests::quests[quest.id].requests[i].count > 0 ) || (Quests::quests[quest.id].requests[i].count == 0 && player->getInventory()->getItemAmount(Quests::quests[quest.id].requests[i].id) != 0)) {
				is=0;
				break;
			}
		}
		else if (Quests::quests[quest.id].requests[i].ismob) {
			int killed=0;
			for (unsigned int j=0; j<quest.mobs.size(); j++) {
				if (quest.mobs[j].id == Quests::quests[quest.id].requests[i].id) {
					killed = quest.mobs[j].count;
					break;
				}
			}
			if (killed < Quests::quests[quest.id].requests[i].count) {
				is=0;
				break;
			}
		}
	}
	if (is) {
		quest.done = 1;
		QuestsPacket::doneQuest(player, quest.id);
	}
}

void PlayerQuests::finishQuest(short questid, int npcid) {
	int chance = 0;
	for (size_t i = 0; i < Quests::quests[questid].rewards.size(); i++) {
		if (Quests::quests[questid].rewards[i].start) {
			if (Quests::quests[questid].rewards[i].isexp) {
				Levels::giveEXP(player, Quests::quests[questid].rewards[i].id * ChannelServer::Instance()->getQuestExprate(), 1);
			}
			else if (Quests::quests[questid].rewards[i].isitem) {
				if (Quests::quests[questid].rewards[i].prop == 0) {
					if (Quests::quests[questid].rewards[i].count>0) {
						QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
						Inventory::addNewItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
					}
					else if (Quests::quests[questid].rewards[i].count<0) {
						QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
						Inventory::takeItem(player, Quests::quests[questid].rewards[i].id, -Quests::quests[questid].rewards[i].count);
					}
					else{
						QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, -player->getInventory()->getItemAmount(Quests::quests[questid].rewards[i].id));
						Inventory::takeItem(player, Quests::quests[questid].rewards[i].id, player->getInventory()->getItemAmount(Quests::quests[questid].rewards[i].id));
					}

				}
				else if (Quests::quests[questid].rewards[i].prop > 0) {
					chance+=Quests::quests[questid].rewards[i].prop;
				}
			}
			else if (Quests::quests[questid].rewards[i].ismesos) {
				player->getInventory()->setMesos(player->getInventory()->getMesos()+Quests::quests[questid].rewards[i].id);
				QuestsPacket::giveMesos(player, Quests::quests[questid].rewards[i].id);
			}
		}
	}
	if (chance > 0) {
		int random = Randomizer::Instance()->randInt(chance-1);
		chance = 0;
		for (size_t i = 0; i < Quests::quests[questid].rewards.size(); i++) {
			if (Quests::quests[questid].rewards[i].start) {
				if (Quests::quests[questid].rewards[i].isitem) {
					if (Quests::quests[questid].rewards[i].prop > 0) {
						if (chance>=random) {
							QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
							if (Quests::quests[questid].rewards[i].count>0)
								Inventory::addNewItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
							else	
								Inventory::takeItem(player, Quests::quests[questid].rewards[i].id, -Quests::quests[questid].rewards[i].count);
							break;
						}
						else
							chance+=Quests::quests[questid].rewards[i].prop;
					}
				}
			}
		}
	}
	for (size_t i = 0; i < quests.size(); i++) {
		if (quests[i].id == questid) {
			quests.erase(quests.begin() + i);
			break;
		}
	}
	QuestComp quest;
	quest.id = questid;
	quest.time = TimeUtilities::getServerTime();
	questscomp.push_back(quest);
	QuestsPacket::questFinish(player, questid, npcid, Quests::quests[questid].nextquest, quest.time);
}

bool PlayerQuests::isQuestActive(short questid) {
	for (size_t i = 0; i < quests.size(); i++) {
		if (quests[i].id == questid) {
			return 1;
		}
	}
	return 0;
}