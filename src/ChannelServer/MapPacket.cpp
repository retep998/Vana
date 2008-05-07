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
#include "PacketCreator.h"
#include "Player.h"
#include "Maps.h"
#include "MapPacket.h"
#include "Inventory.h"
#include "SendHeader.h"

Packet MapPacket::playerPacket(Player* player){
	Packet packet = Packet();
	packet.addHeader(SEND_SHOW_PLAYER);
	packet.addInt(player->getPlayerid());
	packet.addShort(strlen(player->getName()));
	packet.addString(player->getName(), strlen(player->getName()));
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(player->getSkill().types[0]);
	packet.addByte(player->getSkill().types[1]);
	packet.addByte(player->getSkill().types[2]);
	packet.addByte(player->getSkill().types[3]);
	packet.addByte(player->getSkill().types[4]);
	packet.addByte(player->getSkill().types[5]);
	packet.addByte(player->getSkill().types[6]);
	packet.addByte(player->getSkill().types[7]);
	if(player->getSkill().isval)
		packet.addByte(player->getSkill().val);
	packet.addByte(player->getGender());
	packet.addByte(player->getSkin());
	packet.addInt(player->getEyes());
	packet.addByte(1);
	packet.addInt(player->getHair());
	for(int i=0; i<player->inv->getEquipNum(); i++){
		Equip* equip = player->inv->getEquip(i);
		if(equip->pos<0){
			if(!Inventory::isCash(equip->id)){
				bool check=true;
				for(int j=0; j<player->inv->getEquipNum(); j++){
					Equip* equip2 = player->inv->getEquip(j);
					if(equip2->pos<0 && equip != equip2 && equip->type == equip2->type){
						check=false;
						break;
					}
				}	
				if(check){
					packet.addByte(equip->type);
					packet.addInt(equip->id);
				}
			}
			else{
				packet.addByte(equip->type);
				packet.addInt(equip->id);
			}
		}
	}
	packet.addByte(-1);
	for(int i=0; i<player->inv->getEquipNum(); i++){
		Equip* equip = player->inv->getEquip(i);
		if(equip->pos<0){
			if(!Inventory::isCash(equip->id)){
				bool check=true;
				for(int j=0; j<player->inv->getEquipNum(); j++){
					Equip* equip2 = player->inv->getEquip(j);
					if(equip2->pos<0 && equip != equip2 && equip->type == equip2->type){
						check=false;	
						break;
					}
				}	
				if(!check){
					packet.addByte(equip->type);
					packet.addInt(equip->id);
				}
			}
		}
	}
	packet.addByte(-1);
	packet.addInt(0);
	packet.addInt(0);   
	packet.addInt(0);
	packet.addInt(0); 
	packet.addInt(player->getChair());
	packet.addShort(player->getPos().x);
	packet.addShort(player->getPos().y);
	packet.addByte(player->getType());
	packet.addInt(0);
	packet.addShort(1);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	return packet;
}

void MapPacket::showPlayer(Player* player, vector <Player*> players){
	Packet packet = playerPacket(player);
	for(unsigned int i=0; i<players.size(); i++){
		packet.packetSend<Player>(players[i]);
	}
}	

void MapPacket::removePlayer(Player* player, vector <Player*> players){
	Packet packet = Packet();
	packet.addHeader(SEND_REMOVE_PLAYER);
	packet.addInt(player->getPlayerid());
	for(unsigned int i=0; i<players.size(); i++){
		if(player->getPlayerid() != players[i]->getPlayerid())
			packet.packetSend<Player>(players[i]);
	}
}	

void MapPacket::showPlayers(Player* player, vector <Player*> players){
	for(unsigned int i=0; i<players.size(); i++){
		if(player->getPlayerid() != players[i]->getPlayerid()){
			Packet packet = playerPacket(players[i]);
			packet.packetSend<Player>(player);
		}
	}
}	

void MapPacket::changeMap(Player* player){
	Packet packet = Packet();
	packet.addHeader(SEND_CHANGE_MAP);
	packet.addInt(0); // Channel
	packet.addShort(0); // 2?
	packet.addInt(player->getMap());
	packet.addByte(player->getMappos());
	packet.addShort(player->getHP());
	packet.addByte(0);
	packet.addInt(-1);
	packet.addShort(-1);
	packet.addByte(-1);
	packet.addByte(1);
	packet.packetSend<Player>(player);
}

void MapPacket::makeApple(Player* player){
	Packet packet = Packet();
	packet.addHeader(SEND_MAKE_APPLE);  
	packet.packetSend<Player>(player);
}