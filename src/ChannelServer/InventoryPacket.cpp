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
#include "InventoryPacket.h"
#include "Inventory.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Players.h"
#include "PlayerInventory.h"
#include "Maps.h"
#include "PlayerPacketHelper.h"
#include "SendHeader.h"

void InventoryPacket::moveItem(Player* player, char inv, short slot1, short slot2){
	Packet packet = Packet();
	packet.addHeader(SEND_MOVE_ITEM);
	packet.addByte(1);
	packet.addByte(1);
	packet.addByte(2);
	packet.addByte(inv);
	packet.addShort(slot1);
	packet.addShort(slot2);
	packet.addByte(1);
	packet.packetSend(player);
}

void InventoryPacket::updatePlayer(Player* player){
	Packet packet = Packet();
	packet.addHeader(SEND_UPDATE_CHAR_LOOK);
	packet.addInt(player->getPlayerid());
	packet.addByte(1);
	packet.addByte(player->getGender());
	packet.addByte(player->getSkin());
	packet.addInt(player->getEyes());
	packet.addByte(1);
	packet.addInt(player->getHair());
	int equips[35][2] = {0};
	for(int i=0; i<player->inv->getEquipNum(); i++){ //sort equips
		Equip* equip = player->inv->getEquip(i);
		if(equip->pos<0){
			if(equips[equip->type][0]>0){
				if(Inventory::isCash(equip->id)){
					equips[equip->type][1] = equips[equip->type][0];
					equips[equip->type][0] = equip->id;
				}
				else{
					equips[equip->type][1] = equip->id;
				}
			}
			else{
				equips[equip->type][0] = equip->id;
			}
		}
	}
	for(int i=0; i<35; i++){ //shown items
		if(equips[i][0]>0){
			packet.addByte(i);
			if(i == 11 && equips[i][1]>0) // normal weapons always here
				packet.addInt(equips[i][1]);
			else
				packet.addInt(equips[i][0]);
		}
	}
	packet.addByte(-1);
	for(int i=0; i<35; i++){ //covered items
		if(equips[i][1]>0 && i != 11){
			packet.addByte(i);
			packet.addInt(equips[i][1]);
		}
	}
	packet.addByte(-1);
	if(equips[11][1]>0) // cs weapon
		packet.addInt(equips[11][0]);
	else
		packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0); 
	packet.addByte(0);
	packet.addShort(0);
	packet.addInt(0);
	packet.sendTo<Player>(player, Maps::info[player->getMap()].Players, 0);
}

void InventoryPacket::addEquip(Player* player, Equip* equip, bool is){
	Packet packet = Packet();
	packet.addHeader(SEND_MOVE_ITEM);
	packet.addByte(is);
	packet.addByte(1);
	packet.addByte(0);
	packet.addByte(1);
	PlayerPacketHelper::addEquip(packet, equip, true);
	packet.packetSend(player);
}

void InventoryPacket::bought(Player* player){
	Packet packet = Packet();
	packet.addHeader(SEND_SHOP_BOUGHT);
	packet.addByte(0);
	packet.packetSend(player);
}

void InventoryPacket::newMesos(Player* player, int mesos, bool is){
	Packet packet = Packet();
	packet.addHeader(SEND_UPDATE_STAT);
	packet.addShort(is);
	packet.addShort(0);
	packet.addShort(4);
	packet.addInt(mesos);
	packet.packetSend(player);
}

void InventoryPacket::addNewItem(Player* player, Item* item, bool is){
	Packet packet = Packet();
	packet.addHeader(SEND_MOVE_ITEM);
	packet.addByte(is);
	packet.addByte(1);
	packet.addByte(0);
	packet.addByte(item->inv);
	packet.addShort(item->pos);
	packet.addByte(2);
	packet.addInt(item->id);
	packet.addShort(0);
	packet.addBytes("8005BB46E61702");
	packet.addShort(item->amount);
	packet.addInt(0);
	if(ISSTAR(item->id)){
		packet.addInt(2);
		packet.addShort(0x54);
		packet.addByte(0);
		packet.addByte(0x34);
	}
	packet.packetSend(player);
}
void InventoryPacket::addItem(Player* player, Item* item, bool is){
	Packet packet = Packet();
	packet.addHeader(SEND_MOVE_ITEM);
	packet.addByte(is);
	packet.addByte(1);
	packet.addByte(1);
	packet.addByte(item->inv);
	packet.addShort(item->pos);
	packet.addShort(item->amount);
	packet.packetSend(player);
}

void InventoryPacket::moveItemS(Player* player, char inv, short slot, short amount){
	Packet packet = Packet();
	packet.addHeader(SEND_MOVE_ITEM);
	packet.addByte(1);
	packet.addByte(1);
	packet.addByte(1);
	packet.addByte(inv);
	packet.addShort(slot);
	packet.addShort(amount);
	packet.packetSend(player);
}

void InventoryPacket::moveItemS2(Player* player, char inv, short slot1, short amount1, short slot2, short amount2){
	Packet packet = Packet();
	packet.addHeader(SEND_MOVE_ITEM);
	packet.addByte(1);
	packet.addByte(2);
	packet.addByte(1);
	packet.addByte(inv);
	packet.addShort(slot1);
	packet.addShort(amount1);
	packet.addByte(1);
	packet.addByte(inv);
	packet.addShort(slot2);
	packet.addShort(amount2);
	packet.packetSend(player);
}

void InventoryPacket::sitChair(Player* player, vector <Player*> players, int chairid){
	Packet packet = Packet();
	packet.addHeader(SEND_UPDATE_STAT);
	packet.addShort(1);
	packet.addInt(0);
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(SEND_SIT_CHAIR);
	packet.addInt(player->getPlayerid());
	packet.addInt(chairid);
	packet.sendTo<Player>(player, players, 0);

}

void InventoryPacket::stopChair(Player* player, vector <Player*> players){
	Packet packet = Packet();
	packet.addHeader(SEND_STOP_CHAIR);
	packet.addByte(0);
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(SEND_SIT_CHAIR);
	packet.addInt(player->getPlayerid());
	packet.addInt(0);
	packet.sendTo<Player>(player, players, 0);
}
void InventoryPacket::useScroll(Player* player, vector <Player*> players, char s){
	Packet packet = Packet();
	packet.addHeader(SEND_USE_SCROLL);
	packet.addInt(player->getPlayerid());
	packet.addInt(s);
	packet.sendTo<Player>(player, players, 1);
}

void InventoryPacket::showMegaphone(Player* player, vector <Player*> players, char* msg){
	char fullMessage[255];
	strcpy_s(fullMessage, 255, player->getName());
	strcat_s(fullMessage, 255, " : ");
	strcat_s(fullMessage, 255, msg);
	Packet packet = Packet();
	packet.addHeader(SEND_NOTICE);
	packet.addByte(2);
	packet.addShort(strlen(fullMessage));
	packet.addString(fullMessage, strlen(fullMessage));
	packet.sendTo<Player>(player, players, 1);
}

void InventoryPacket::showSuperMegaphone(Player* player, char* msg, int whisper){
	char fullMessage[255];
	strcpy_s(fullMessage, 255, player->getName());
	strcat_s(fullMessage, 255, " : ");
	strcat_s(fullMessage, 255, msg);
	Packet packet = Packet();
	packet.addHeader(SEND_NOTICE);
	packet.addByte(3);
	packet.addShort(strlen(fullMessage));
	packet.addString(fullMessage, strlen(fullMessage));
	packet.addByte(player->getChannel());
	packet.addByte(whisper);
	for(hash_map<int,Player*>::iterator iter = Players::players.begin(); //TODO: Cross channel
		iter != Players::players.end(); iter++){
			packet.packetSend(iter->second);
	}
}

void InventoryPacket::showMessenger(Player* player, char* msg, char* msg2, char* msg3, char* msg4, unsigned char* displayInfo, int displayInfo_size, int itemid){
	Packet packet = Packet();
	packet.addHeader(SEND_SHOW_MESSENGER);
	packet.addInt(itemid);
	packet.addShort(strlen(player->getName()));
	packet.addString(player->getName(), strlen(player->getName()));
	packet.addShort(strlen(msg));
	packet.addString(msg, strlen(msg));
	packet.addShort(strlen(msg2));
	packet.addString(msg2, strlen(msg2));
	packet.addShort(strlen(msg3));
	packet.addString(msg3, strlen(msg3));
	packet.addShort(strlen(msg4));
	packet.addString(msg4, strlen(msg4));
	packet.addInt(player->getChannel());
	packet.addBytesHex(displayInfo, displayInfo_size);

	for(hash_map<int,Player*>::iterator iter = Players::players.begin();
		iter != Players::players.end(); iter++){
			packet.packetSend(iter->second);
	}
}
// Use buff item
void InventoryPacket::useItem(Player* player, int itemid, int time, unsigned char types[8], vector <short> vals, bool morph){ // Test/Beta function, PoC only
	Packet packet = Packet();
	packet.addHeader(SEND_USE_SKILL);
	packet.addByte(types[0]);
	packet.addByte(types[1]);
	packet.addByte(types[2]);
	packet.addByte(types[3]);
	packet.addByte(types[4]);
	packet.addByte(types[5]);
	packet.addByte(types[6]);
	packet.addByte(types[7]);
	for(unsigned int i=0; i<vals.size(); i++){
		packet.addShort(vals[i]);
		packet.addInt(itemid*-1);
		packet.addInt(time);
	}
	packet.addShort(0);
	if(morph)
		packet.addByte(1);
	else
		packet.addByte(0);
	packet.packetSend(player);
}
void InventoryPacket::endItem(Player* player, unsigned char types[8]){
	Packet packet = Packet();
	packet.addHeader(SEND_CANCEL_SKILL);
	packet.addByte(types[0]);
	packet.addByte(types[1]);
	packet.addByte(types[2]);
	packet.addByte(types[3]);
	packet.addByte(types[4]);
	packet.addByte(types[5]);
	packet.addByte(types[6]);
	packet.addByte(types[7]);
	packet.addByte(0);
	packet.packetSend(player);
}
