#include "InventoryPacket.h"
#include "Inventory.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerInventory.h"
#include "Maps.h"

void InventoryPacket::moveItem(Player* player, char inv, short slot1, short slot2){
	Packet packet = Packet();
	packet.addHeader(0x18);
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
	packet.addHeader(0x92);
	packet.addInt(player->getPlayerid());
	packet.addByte(1);
	packet.addByte(player->getGender());
	packet.addByte(player->getSkin());
	packet.addInt(player->getEyes());
	packet.addByte(1);
	packet.addInt(player->getHair());	for(int i=0; i<player->inv->getEquipNum(); i++){
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
	packet.addByte(0);
	packet.addShort(0);
	packet.addInt(0);
	packet.sendTo(player, Maps::info[player->getMap()].Players, 0);
}

void InventoryPacket::addEquip(Player* player, Equip* equip, bool is){
	Packet packet = Packet();
	packet.addHeader(0x18);
	packet.addByte(is);
	packet.addByte(1);
	packet.addByte(0);
	packet.addByte(1);
	packet.addShort(equip->pos);
	packet.addByte(1);
	packet.addInt(equip->id);
	packet.addShort(0);
	packet.addBytes("8005BB46E61702");
	packet.addShort(equip->slots);
	packet.addShort(equip->istr);
	packet.addShort(equip->idex);
	packet.addShort(equip->iint);
	packet.addShort(equip->iluk);
	packet.addShort(equip->ihp);
	packet.addShort(equip->imp);
	packet.addShort(equip->iwatk);
	packet.addShort(equip->imatk);
	packet.addShort(equip->iwdef);
	packet.addShort(equip->imdef);
	packet.addShort(equip->iacc);
	packet.addShort(equip->iavo);
	packet.addShort(equip->ihand);
	packet.addShort(equip->ispeed);
	packet.addShort(equip->ijump);
	packet.addShort(0);
	packet.addShort(0);
	packet.addShort(0);
	packet.addShort(0);
	packet.addShort(0);
	packet.addShort(0);
	packet.packetSend(player);
}

void InventoryPacket::bought(Player* player){
	Packet packet = Packet();
	packet.addHeader(0xD7);
	packet.addByte(0);
	packet.packetSend(player);
}

void InventoryPacket::newMesos(Player* player, int mesos, bool is){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(is);
	packet.addShort(0);
	packet.addShort(4);
	packet.addInt(mesos);
	packet.packetSend(player);
}

void InventoryPacket::addNewItem(Player* player, Item* item, bool is){
	Packet packet = Packet();
	packet.addHeader(0x18);
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
	packet.addHeader(0x18);
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
	packet.addHeader(0x18);
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
	packet.addHeader(0x18);
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
	packet.addHeader(0x23);
	packet.addShort(1);
	packet.addInt(0);
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(0x91);
	packet.addInt(player->getPlayerid());
	packet.addInt(chairid);
	packet.sendTo(player, players, 0);

}

void InventoryPacket::stopChair(Player* player, vector <Player*> players){
	Packet packet = Packet();
	packet.addHeader(0x66);
	packet.addByte(0);
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(0x91);
	packet.addInt(player->getPlayerid());
	packet.addInt(0);
	packet.sendTo(player, players, 0);
}
void InventoryPacket::useScroll(Player* player, vector <Player*> players, char s){
	Packet packet = Packet();
	packet.addHeader(0x7A);
	packet.addInt(player->getPlayerid());
	packet.addInt(s);
	packet.sendTo(player, players, 1);
}

