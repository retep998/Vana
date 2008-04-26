#include "Quests.h"
#include "Player.h"
#include "QuestsPacket.h"
#include "Levels.h"
#include "Maps.h"
#include "Inventory.h"
#include "NPCs.h"
#include "Server.h"

hash_map <int, QuestInfo> Quests::quests;

void Quests::giveItem(Player* player, int itemid, int amount){
	//Temp
	QuestsPacket::giveItem(player, itemid, amount);
	if(amount>0)
		Inventory::addNewItem(player, itemid, amount);
	else	
		Inventory::takeItem(player, itemid, -amount);
}

void Quests::giveMesos(Player* player, int amount){
	player->inv->setMesos(player->inv->getMesos()+amount);
	QuestsPacket::giveMesos(player, amount);
}


void Quests::getQuest(Player *player, unsigned char *packet){
	short questid = getShort(packet+1);
	int npcid = getInt(packet+3);
	if(packet[0] == 0){	
		QuestsPacket::giveItem(player, getInt(packet+7), npcid);
		Inventory::addNewItem(player, getInt(packet+7), npcid);
	}
	else if(packet[0] == 1){
		player->quests->addQuest(questid, npcid);
	}
	else if(packet[0] == 2){
		player->quests->finishQuest(questid, npcid);
	}
	else if(packet[0] == 4){
		NPCs::handleQuestNPC(player, npcid, 1);
	}
	else if(packet[0] == 5){
		NPCs::handleQuestNPC(player, npcid, 0);
	}
}

void PlayerQuests::updateQuestMob(int mobid){
	for(unsigned int i=0; i<quests.size(); i++){
		for(unsigned int j=0; j<quests[i].mobs.size(); j++){
			if(quests[i].mobs[j].id == mobid && !quests[i].done){
				int maxcount=0;
				for(unsigned int k=0; k<Quests::quests[quests[i].id].requests.size(); k++){
					if(Quests::quests[quests[i].id].requests[k].id == mobid){
						maxcount = Quests::quests[quests[i].id].requests[k].count;
					}
				}
				if(quests[i].mobs[j].count<maxcount){
					quests[i].mobs[j].count++;
					QuestsPacket::updateQuest(player, quests[i]);
					if(quests[i].mobs[j].count == maxcount){
						checkDone(quests[i]);
					}
				}
			}
		}
	}
}
void PlayerQuests::checkDone(Quest &quest){
	if(Quests::quests[quest.id].requests.size() == 0){
		quest.done = 1;
		return;
	}
	int is=1;
	for(unsigned int i=0; i<Quests::quests[quest.id].requests.size(); i++){
		if(Quests::quests[quest.id].requests[i].isitem){
			if((player->inv->getItemAmount(Quests::quests[quest.id].requests[i].id) < Quests::quests[quest.id].requests[i].count && Quests::quests[quest.id].requests[i].count > 0 ) || (Quests::quests[quest.id].requests[i].count == 0 && player->inv->getItemAmount(Quests::quests[quest.id].requests[i].id) != 0)){
				is=0;
				break;
			}
		}
		else if(Quests::quests[quest.id].requests[i].ismob){
			int killed=0;
			for(unsigned int j=0; j<quest.mobs.size(); j++){
				if(quest.mobs[j].id == Quests::quests[quest.id].requests[i].id){
					killed = quest.mobs[j].count;
					break;
				}
			}
			if(killed < Quests::quests[quest.id].requests[i].count){
				is=0;
				break;
			}
		}
	}
	if(is){
		quest.done = 1;
		QuestsPacket::doneQuest(player, quest.id);
	}

}

void PlayerQuests::finishQuest(short questid, int npcid){
	int chance=0;
	for(unsigned int i=0; i<Quests::quests[questid].rewards.size(); i++){
		if(Quests::quests[questid].rewards[i].start){
			if(Quests::quests[questid].rewards[i].isexp){
				Levels::giveEXP(player, Quests::quests[questid].rewards[i].id, 1);
			}
			else if(Quests::quests[questid].rewards[i].isitem){
				if(Quests::quests[questid].rewards[i].prop == 0){
					if(Quests::quests[questid].rewards[i].count>0){
						QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
						Inventory::addNewItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
					}
					else if(Quests::quests[questid].rewards[i].count<0){
						QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
						Inventory::takeItem(player, Quests::quests[questid].rewards[i].id, -Quests::quests[questid].rewards[i].count);
					}
					else{
						QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, -player->inv->getItemAmount(Quests::quests[questid].rewards[i].id));
						Inventory::takeItem(player, Quests::quests[questid].rewards[i].id, player->inv->getItemAmount(Quests::quests[questid].rewards[i].id));
					}

				}
				else if(Quests::quests[questid].rewards[i].prop > 0){
					chance+=Quests::quests[questid].rewards[i].prop;
				}
			}
			else if(Quests::quests[questid].rewards[i].ismesos){
				player->inv->setMesos(player->inv->getMesos()+Quests::quests[questid].rewards[i].id);
				QuestsPacket::giveMesos(player, Quests::quests[questid].rewards[i].id);
			}
		}
	}
	if(chance>0){
		int random = rand()%chance;
		chance=0;
		for(unsigned int i=0; i<Quests::quests[questid].rewards.size(); i++){
			if(Quests::quests[questid].rewards[i].start){
				if(Quests::quests[questid].rewards[i].isitem){
					if(Quests::quests[questid].rewards[i].prop > 0){
						if(chance>=random){
							QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
							if(Quests::quests[questid].rewards[i].count>0)
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
	for(unsigned int i=0; i<quests.size(); i++){
		if(quests[i].id == questid){
			quests.erase(quests.begin() + i);
			break;
		}
	}
	QuestComp quest;
	quest.id = questid;
	quest.time = Server::getServerTime();
	questscomp.push_back(quest);
	QuestsPacket::questFinish(player, Maps::info[player->getMap()].Players , questid, npcid, Quests::quests[questid].nextquest, quest.time);
}

void PlayerQuests::addQuest(int questid, int npcid){
	QuestsPacket::acceptQuest(player, questid, npcid);
	Quest quest;
	quest.id = questid;
	quest.done = 0;
	for(unsigned int i=0; i<Quests::quests[questid].requests.size(); i++){
		if(Quests::quests[questid].requests[i].ismob){
			QuestMob mob;
			mob.id = Quests::quests[questid].requests[i].id;
			mob.count = 0;	
			quest.mobs.push_back(mob);
		}
	}
	for(unsigned int i=0; i<Quests::quests[questid].rewards.size(); i++){
		if(!Quests::quests[questid].rewards[i].start){
			if(Quests::quests[questid].rewards[i].isexp){
				Levels::giveEXP(player, Quests::quests[questid].rewards[i].id, 1);
			}
			else if(Quests::quests[questid].rewards[i].isitem){
				if(Quests::quests[questid].rewards[i].count>0){
					QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
					Inventory::addNewItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
				}
				else if(Quests::quests[questid].rewards[i].count<0){
					QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
					Inventory::takeItem(player, Quests::quests[questid].rewards[i].id, Quests::quests[questid].rewards[i].count);
				}
				else{
					QuestsPacket::giveItem(player, Quests::quests[questid].rewards[i].id, -player->inv->getItemAmount(Quests::quests[questid].rewards[i].id));
					Inventory::takeItem(player, Quests::quests[questid].rewards[i].id, player->inv->getItemAmount(Quests::quests[questid].rewards[i].id));
				}
			}
			else if(Quests::quests[questid].rewards[i].ismesos){
				player->inv->setMesos(player->inv->getMesos()+Quests::quests[questid].rewards[i].id);
				QuestsPacket::giveMesos(player, Quests::quests[questid].rewards[i].id);
			}
		}
	}
	checkDone(quest);
	quests.push_back(quest);
}