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
#include "Players.h"
#include "Player.h"
#include "PlayersPacket.h"
#include "Maps.h" 
#include "Mobs.h"
#include "Shops.h"
#include "Inventory.h"
#include "Drops.h"
#include "Levels.h"
#include "Server.h"
#include "SkillsPacket.h"
#include "BufferUtilities.h"
#include "CharUtilities.h"

hash_map <int, Player*> Players::players;

void Players::addPlayer(Player* player){
	players[player->getPlayerid()] = player;
}

void Players::deletePlayer(Player* player){
	if(players.find(player->getPlayerid()) != players.end()){
		for (hash_map<int,Player*>::iterator iter = players.begin();
			 iter != players.end(); iter++){
				 if(iter->first == player->getPlayerid()){
					 players.erase(iter);
					break;
				 }
		}
	}
}

void Players::handleMoving(Player* player, unsigned char* packet, int size){
	Pos cpos;
	int pla = packet[5]*14+1;
	cpos.x = getShort(packet+size-4);
	cpos.y = getShort(packet+size-2);
	player->setPos(cpos);
	player->setType(packet[5+14*(packet[5]-1)+12]);
	PlayersPacket::showMoving(player, Maps::info[player->getMap()].Players, packet, pla);
}

void Players::faceExperiment(Player* player, unsigned char* packet){
	int face = getInt(packet);
	PlayersPacket::faceExperiment(player, Maps::info[player->getMap()].Players, face);
}
void Players::chatHandler(Player* player, unsigned char* packet){
	char chat[91];
	int chatsize = getShort(packet);
	getString(packet+2, chatsize, chat);
	if(chat[0] == '!'){
		if(!player->isGM()) return;
		char* next_token;
		char command[90] = "";
		if(chatsize>2)
			strcpy_s(command, 90, strtok_s(chat+1, " ", &next_token));
		if(strcmp(command, "map") == 0){
			if(strlen(next_token) > 0){
				int mapid = atoi(strtok_s(NULL, " ",&next_token));
				if(Maps::info.find(mapid) != Maps::info.end())
					Maps::changeMap(player ,mapid, 0);
			}
		}
		else if(strcmp(command, "summon") == 0 || strcmp(command, "spawn") == 0){
			if(strlen(next_token) == 0) return;
			int mobid = atoi(strtok_s(NULL, " ",&next_token));
			if(Mobs::mobinfo.find(mobid) == Mobs::mobinfo.end())
				return;
			int count = 1;
			if(strlen(next_token) > 0)
				count = atoi(next_token);
			for(int i=0; i<count && i<100; i++){
				Mobs::spawnMob(player, mobid);
			}
		}
		else if(strcmp(command, "notice") == 0){
			if(strlen(next_token) == 0) return;
			PlayersPacket::showMessage(next_token, 0);
		}
		else if(strcmp(command, "shop") == 0){
			Shops::showShop(player, 9999999);
		}
		else if(strcmp(command, "shop2") == 0){
			Shops::showShop(player, 9999996);
		}
		else if(strcmp(command, "shop3") == 0){
			Shops::showShop(player, 9999995);
		}
		else if(strcmp(command, "shop4") == 0){
			Shops::showShop(player, 9999998);
		}
		else if(strcmp(command, "pos") == 0){
			char text[50];
			sprintf_s(text, 50, "X: %d Y: %d", player->getPos().x, player->getPos().y);
			PlayersPacket::showMessage(text, 0);
		}
		else if(strcmp(command, "item") == 0){
			if(strlen(next_token) == 0) return;
			int itemid = atoi(strtok_s(NULL, " ",&next_token));
			if(Drops::items.find(itemid) == Drops::items.end() && Drops::equips.find(itemid) == Drops::equips.end())
				return;
			int count = 1;
			if(strlen(next_token) > 0)
				count = atoi(next_token);
			Inventory::addNewItem(player, itemid, count);
		}
		else if(strcmp(command, "level") == 0){
			if(strlen(next_token) == 0) return;
			Levels::setLevel(player, atoi(strtok_s(NULL, " ",&next_token)));
		}
		else if(strcmp(command, "job") == 0){
			if(strlen(next_token) == 0) return;
			Levels::setJob(player, atoi(strtok_s(NULL, " ",&next_token)));
		}	
		else if(strcmp(command, "ap") == 0){
			if(strlen(next_token) == 0) return;
			player->setAp(player->getAp()+atoi(strtok_s(NULL, " ",&next_token)));
		}
		else if(strcmp(command, "sp") == 0){
			if(strlen(next_token) == 0) return;
			player->setSp(player->getSp()+atoi(strtok_s(NULL, " ",&next_token)));
		}
		else if(strcmp(command, "killnpc") == 0){
			player->setNPC(NULL);
		}
		else if(strcmp(command, "killall") == 0){
			int size=Mobs::mobs[player->getMap()].size();
			for (int j=0; j<size; j++){
				Mobs::dieMob(player, Mobs::mobs[player->getMap()][0]);
			}
		}
		else if(strcmp(command, "horntail") == 0){
			Mobs::spawnMob(player, 8810002);
			Mobs::spawnMob(player, 8810003);
			Mobs::spawnMob(player, 8810004);
			Mobs::spawnMob(player, 8810005);
			Mobs::spawnMob(player, 8810006);
			Mobs::spawnMob(player, 8810007);
            Mobs::spawnMob(player, 8810008);
            Mobs::spawnMob(player, 8810009);
        }
		else if(strcmp(command, "heal") == 0){
            player->setHP(player->getMHP());
            player->setMP(player->getMMP());
        }
		else if(strcmp(command, "kill") == 0){
			if(strcmp(next_token, "all") == 0){
				for (unsigned int x=0; x<Maps::info[player->getMap()].Players.size(); x++){
					Player* killpsa;
					killpsa = Maps::info[player->getMap()].Players[x];
					if(killpsa != player){
						killpsa->setHP(0);
					}
				}
			}
			else if(strcmp(next_token, "gm") == 0){
				for (unsigned int x=0; x<Maps::info[player->getMap()].Players.size(); x++){	
					Player* killpsa;
					killpsa = Maps::info[player->getMap()].Players[x];
					if(killpsa != player){
						if(killpsa->isGM()){	
							killpsa->setHP(0);
						}
					}
				}
			}
			else if(strcmp(next_token, "players") == 0){
				for (unsigned int x=0; x<Maps::info[player->getMap()].Players.size(); x++){	
					Player* killpsa;
					killpsa = Maps::info[player->getMap()].Players[x];
					if(killpsa != player){
						if(!killpsa->isGM()){
							killpsa->setHP(0);
						}
					}
				}
			}
			else if(strcmp(next_token, "me") == 0){
				player->setHP(0);
			}
			else {
				for (unsigned int x=0; x<Maps::info[player->getMap()].Players.size(); x++){
					Player* killpsa;
					killpsa = Maps::info[player->getMap()].Players[x];
					if(killpsa != player){
						killpsa->setHP(0);
					}
				}
			}
		}
		else if(strcmp(command, "zakum") == 0){
			Mobs::spawnMob(player, 8800000);
			Mobs::spawnMob(player, 8800003);
			Mobs::spawnMob(player, 8800004);
			Mobs::spawnMob(player, 8800005);
			Mobs::spawnMob(player, 8800006);
			Mobs::spawnMob(player, 8800007);
			Mobs::spawnMob(player, 8800008);
			Mobs::spawnMob(player, 8800009);
			Mobs::spawnMob(player, 8800010);
		}
		else if(strcmp(command, "header") == 0){
			if(strlen(next_token) > 0){
				Server::changeScrollingHeader(next_token);
			}
			else{
				Server::disableScrollingHeader();
			}
		}
		else if	(strcmp(command, "dc") == 0)	{
			player->disconnect();
		}
		return;
	}
	PlayersPacket::showChat(player, Maps::info[player->getMap()].Players, chat);

}

void Players::damagePlayer(Player* player, unsigned char* packet){
	int damage = getInt(packet+5);
	int mobid = getInt(packet+13);
	Mob* mob = NULL;
	for(unsigned int i=0; i<Mobs::mobs[player->getMap()].size(); i++)
		if(Mobs::mobs[player->getMap()][i]->getID() == mobid){
			mob = Mobs::mobs[player->getMap()][i];
			break;
		}

	// Magic Guard
	if(player->skills->getActiveSkillLevel(2001002) > 0){
		unsigned short mp = player->getMP();
        unsigned short hp = player->getHP();
        unsigned short reduc = Skills::skills[2001002][player->skills->getActiveSkillLevel(2001002)].x;
        int mpdamage = ((damage * reduc) / 100);
        int hpdamage = damage - mpdamage;
        if (mpdamage >= mp) {
            player->setMP(0);
            player->setHP(hp - (hpdamage + (mpdamage - mp)));
        }
        if (mpdamage < mp) {
            player->setMP(mp - mpdamage);
            player->setHP(hp - hpdamage);
        }
	}
	// Power Guard
	else if(player->skills->getActiveSkillLevel(1101007)>0 || player->skills->getActiveSkillLevel(1201007)>0){
		float reduc;
		if(player->skills->getActiveSkillLevel(1101007)>0)
			reduc = Skills::skills[1101007][player->skills->getActiveSkillLevel(1101007)].x;
		else
			reduc = Skills::skills[1201007][player->skills->getActiveSkillLevel(1201007)].x;
		if(damage>0){
			damage = (int) (damage-(damage*(reduc/100)));
			player->setHP(player->getHP()-damage);
		}
	}
	// Meso Guard
	else if(player->skills->getActiveSkillLevel(4211005) > 0 && player->inv->getMesos() > 0){
		unsigned short hp = player->getHP();
		// Get the rate of meso loss in %
		float mesorate = Skills::skills[4211005][player->skills->getActiveSkillLevel(4211005)].x;
		mesorate = mesorate/100;
		int mesoloss = (int) (mesorate)*(damage/2);
		// Only block damage if user has mesos
		if(player->inv->getMesos() > 0)
			player->setHP(hp-(damage/2));
		else
			player->setHP(hp-damage);
		// Do not let mesos go negative
		if(player->inv->getMesos()-mesoloss < 0){
			player->inv->setMesos(0);
		}
		else{
			player->inv->setMesos(player->inv->getMesos()-mesoloss);
		}
	}
	else{
		player->setHP(player->getHP()-damage);
	}
	if(mob != NULL)
		PlayersPacket::damagePlayer(player, Maps::info[player->getMap()].Players, damage, mob->getMobID());
}

void Players::healPlayer(Player* player, unsigned char* packet){
	player->setHP(player->getHP()+getShort(packet+4));
	player->setMP(player->getMP()+getShort(packet+6));
}

void Players::getPlayerInfo(Player* player, unsigned char* packet){
	PlayersPacket::showInfo(player, players[getInt(packet+4)]);
}

void Players::commandHandler(Player* player, unsigned char* packet){
	unsigned char type = packet[0];
	char name[20];

	int namesize = getShort(packet+1);
	getString(packet+3, namesize, name);
		
	hash_map <int, Player*>::iterator iter = Players::players.begin();
	for ( iter = Players::players.begin(); iter != Players::players.end(); iter++){
		if (_stricmp(iter->second->getName(),name) == 0){	
			if(type == 0x06){
				char chat[91];
				int chatsize = getShort(packet+3+namesize);
				getString(packet+5+namesize, chatsize, chat);

				PlayersPacket::whisperPlayer(player,iter->second,chat);
				PlayersPacket::findPlayer(player,iter->second->getName(),0,1);
			}
			else if(type == 0x05){
				PlayersPacket::findPlayer(player, iter->second->getName(), iter->second->getMap());
			}
			break;
		}
	}	
	if(iter == Players::players.end()){
		PlayersPacket::findPlayer(player,name,-1);
	}
}