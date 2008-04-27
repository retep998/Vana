#include "PlayerPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Inventory.h"
#include "Skills.h"
#include "Server.h"
#include "TimeUtilities.h"

void PlayerPacket::connectData(Player* player){
	Packet packet = Packet();
	packet.addHeader(0x4D);
	packet.addInt(0); // Channel
	packet.addBytes("0101853D4B11F4836B3DBA9A4FA1");
	packet.addShort(-1);
	packet.addInt(player->getPlayerid());
	packet.addString(player->getName(), 12);
	packet.addByte(0);
	packet.addByte(player->getGender());
	packet.addByte(player->getSkin());
	packet.addInt(player->getEyes());
	packet.addInt(player->getHair());
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(player->getLevel());
	packet.addShort(player->getJob());
	packet.addShort(player->getStr());
	packet.addShort(player->getDex());
	packet.addShort(player->getInt());
	packet.addShort(player->getLuk());
	packet.addShort(player->getHP());
	packet.addShort(player->getMHP());
	packet.addShort(player->getMP());
	packet.addShort(player->getMMP());
	packet.addShort(player->getAp());
	packet.addShort(player->getSp());
	packet.addInt(player->getExp());
	packet.addShort(player->getFame());
	packet.addInt(player->getMap());
	packet.addByte(player->getMappos());
	packet.addByte(0x14);
	packet.addInt(player->inv->getMesos());
	packet.addByte(100);
	packet.addByte(100);
	packet.addByte(100);
	packet.addByte(100);
	packet.addByte(100);
	for(int i=0; i<player->inv->getEquipNum(); i++){
		Equip* equip = player->inv->getEquip(i);
		if(equip->pos<0 && !Inventory::isCash(equip->id)){
			packet.addByte(equip->type);
			packet.addByte(1);
			packet.addInt(equip->id);
			packet.addShort(equip->scrolls);
			packet.addBytes("8005BB46E61702");
			packet.addShort(equip->slots); // slots
			packet.addShort(equip->istr); // STR
			packet.addShort(equip->idex); // DEX
			packet.addShort(equip->iint); // INT
			packet.addShort(equip->iluk); // LUK
			packet.addShort(equip->ihp); // HP
			packet.addShort(equip->imp); // MP
			packet.addShort(equip->iwatk); // W.Atk
			packet.addShort(equip->imatk); // M.Atk
			packet.addShort(equip->iwdef); // W.def
			packet.addShort(equip->imdef); // M.Def
			packet.addShort(equip->iacc); // Acc		
			packet.addShort(equip->iavo); // Avo		
			packet.addShort(equip->ihand); // Hands		
			packet.addShort(equip->ispeed); // Speed		
			packet.addShort(equip->ijump); // Jump		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);
		}
	}
	packet.addByte(0);
	for(int i=0; i<player->inv->getEquipNum(); i++){
		Equip* equip = player->inv->getEquip(i);
		if(equip->pos<0 && Inventory::isCash(equip->id)){
			packet.addByte(equip->type);
			packet.addByte(1);
			packet.addInt(equip->id);
			packet.addShort(0);
			packet.addBytes("8005BB46E61702");
			packet.addShort(equip->slots); // slots
			packet.addShort(equip->istr); // STR
			packet.addShort(equip->idex); // DEX
			packet.addShort(equip->iint); // INT
			packet.addShort(equip->iluk); // LUK
			packet.addShort(equip->ihp); // HP
			packet.addShort(equip->imp); // MP
			packet.addShort(equip->iwatk); // W.Atk
			packet.addShort(equip->imatk); // M.Atk
			packet.addShort(equip->iwdef); // W.def
			packet.addShort(equip->imdef); // M.Def
			packet.addShort(equip->iacc); // Acc		
			packet.addShort(equip->iavo); // Avo		
			packet.addShort(equip->ihand); // Hands		
			packet.addShort(equip->ispeed); // Speed		
			packet.addShort(equip->ijump); // Jump		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(equip->scrolls);
		}
	}
	packet.addByte(0);
	for(int i=0; i<player->inv->getEquipNum(); i++){
		Equip* equip = player->inv->getEquip(i);
		if(equip->pos>0){
			packet.addByte((char)equip->pos);
			packet.addByte(1);
			packet.addInt(equip->id);
			packet.addShort(0);
			packet.addBytes("8005BB46E61702");
			packet.addShort(equip->slots); // slots
			packet.addShort(equip->istr); // STR
			packet.addShort(equip->idex); // DEX
			packet.addShort(equip->iint); // INT
			packet.addShort(equip->iluk); // LUK
			packet.addShort(equip->ihp); // HP
			packet.addShort(equip->imp); // MP
			packet.addShort(equip->iwatk); // W.Atk
			packet.addShort(equip->imatk); // M.Atk
			packet.addShort(equip->iwdef); // W.def
			packet.addShort(equip->imdef); // M.Def
			packet.addShort(equip->iacc); // Acc		
			packet.addShort(equip->iavo); // Avo		
			packet.addShort(equip->ihand); // Hands		
			packet.addShort(equip->ispeed); // Speed		
			packet.addShort(equip->ijump); // Jump		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);		
			packet.addShort(0);
		}
	}
	packet.addByte(0);
	for(int j=2; j<=5; j++){
		for(int i=0; i<player->inv->getItemNum(); i++){
			Item* item = player->inv->getItem(i);
			if(item->inv == j){
				packet.addByte((char)item->pos);
				packet.addByte(2);
				packet.addInt(item->id);
				packet.addShort(0);
				packet.addBytes("8005BB46E61702");
				packet.addShort(item->amount); // slots
				packet.addInt(0);			
				if(ISSTAR(item->id)){
					packet.addInt(2);
					packet.addShort(0x54);
					packet.addByte(0);
					packet.addByte(0x34);
				}
			}
		}
		packet.addByte(0);

	}
	//Skills
	packet.addShort(player->skills->getSkillsNum());
	for(int i=0; i<player->skills->getSkillsNum(); i++){
		packet.addInt(player->skills->getSkillID(i));
		packet.addInt(player->skills->getSkillLevel(player->skills->getSkillID(i)));
	}
	//End
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addShort(0);
	for(int i=0; i<15; i++)
		packet.addBytes("FFC99A3B");
	packet.addInt64(getServerTime());
	packet.packetSend(player);
}

void PlayerPacket::newHP(Player* player, short hp){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addByte(0);
	packet.addShort(0);
	packet.addShort(4);
	packet.addByte(0);
	packet.addShort(hp); 
	packet.packetSend(player);
}

void PlayerPacket::newMP(Player* player, short mp, bool is){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addByte(is);
	packet.addShort(0);
	packet.addShort(0x10);
	packet.addByte(0);
	packet.addShort(mp);
	packet.packetSend(player);
}

void PlayerPacket::newEXP(Player* player, int exp){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(0);
	packet.addShort(0);
	packet.addShort(1);
	packet.addInt(exp);
	packet.packetSend(player);
}


void PlayerPacket::showKeys(Player* player, int keys[90]){
	Packet packet = Packet();
	packet.addHeader(0xf6);
	packet.addByte(0);
	for(int i=0; i<90; i++){
		packet.addInt(keys[i]);
		packet.addByte(0);
	}
	packet.packetSend(player);
}
void PlayerPacket::setSP(Player* player){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(0);
	packet.addByte(0);
	packet.addShort(0x80);
	packet.addByte(0);
	packet.addShort(player->getSp());
	packet.packetSend(player);
}

void PlayerPacket::setJob(Player* player){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(0);
	packet.addInt(0x20);
	packet.addShort(player->getJob());
	packet.packetSend(player);
}

void PlayerPacket::newHair(Player* player){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(0);
	packet.addInt(0x4);
	packet.addInt(player->getHair());
	packet.packetSend(player);
}

void PlayerPacket::newEyes(Player* player){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(0);
	packet.addInt(0x2);
	packet.addInt(player->getEyes());
	packet.packetSend(player);
}

void PlayerPacket::newSkin(Player* player){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(0);
	packet.addInt(0x1);
	packet.addByte(player->getSkin());
	packet.packetSend(player);
}