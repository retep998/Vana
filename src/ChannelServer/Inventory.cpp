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
#include "Inventory.h"
#include "Player.h"
#include "InventoryPacket.h"
#include "Drops.h"
#include "Shops.h"
#include "Quests.h"
#include "Maps.h"
#include "Mobs.h"
#include "BufferUtilities.h"

#include "Timer.h"

class ItemTimer: public Timer::TimerHandler {
public:
	static ItemTimer * Instance() {
		if (singleton == 0)
			singleton = new ItemTimer;
		return singleton;
	}
	void setItemTimer(Player* player, int item, int time){
		ITimer timer;
		timer.id = Timer::Instance()->setTimer(time, this);;
		timer.player = player;
		timer.item = item;
		timer.time = time;
		timers.push_back(timer);
	}
	void stop(Player* player, int item){
		for(unsigned int i=0; i<timers.size(); i++){
			if(player == timers[i].player && timers[i].item == item){
				Timer::Instance()->cancelTimer(timers[i].id);
				break;
			}
		}
	}
	void stop(Player* player){
		for(unsigned int i=timers.size(); i>0; i--){ // fix missing timer cancels
			if(player == timers[i-1].player){
				Timer::Instance()->cancelTimer(timers[i-1].id);
			}
		}
	}
private:
	static ItemTimer *singleton;
	ItemTimer() {};
	ItemTimer(const ItemTimer&);
	ItemTimer& operator=(const ItemTimer&);

	struct ITimer {
		int id;
		Player* player;
		int item;
		int time;
	};
	static vector <ITimer> timers;
	void handle(Timer* timer, int id){
		int item;
		Player* player;
		for(unsigned int i=0; i<timers.size(); i++){
			if(timers[i].id == id){
				player = timers[i].player;
				item = timers[i].item;
				break;
			}
		}
		Inventory::endItem(player, item);
	}
	void remove (int id){
		for(unsigned int i=0; i<timers.size(); i++){
			if(timers[i].id == id){	
				timers.erase(timers.begin()+i);	
				return;
			}
		}
	}
};

vector <ItemTimer::ITimer> ItemTimer::timers;
ItemTimer * ItemTimer::singleton = 0;

void Inventory::stopTimerPlayer(Player* player){
	ItemTimer::Instance()->stop(player);
}

void Inventory::itemMove(Player* player, unsigned char* packet){
	char inv = packet[4];
	short slot1 = BufferUtilities::getShort(packet+5);
	short slot2 = BufferUtilities::getShort(packet+7);
	if(inv == 1){
		if(slot2 == 0){
			InventoryPacket::moveItem(player, inv, slot1, slot2);
			Equip* equip = NULL;
			int num;
			for(int i=0; i<player->inv->getEquipNum(); i++){
				if(player->inv->getEquipPos(i) == slot1){
					equip = player->inv->getEquip(i);	
					num = i;
					break;
				}
			}
			if(equip==NULL)
				return;
			Drop* drop = new Drop(player->getMap());
			drop->setID(equip->id);
			drop->setOwner(player->getPlayerid());
			drop->setTime(0);
			drop->setEquip(true);
			Dropped dropper;
			dropper.id = player->getPlayerid();
			dropper.pos = player->getPos();
			drop->setPos(player->getPos());
			DropInfo dropi;
			dropi.type = equip->type;
			dropi.scrolls = equip->scrolls;
			dropi.slots = equip->slots;
			dropi.istr = equip->istr;
			dropi.idex = equip->idex;
			dropi.iint = equip->iint;
			dropi.iluk = equip->iluk;
			dropi.ihp = equip->ihp;
			dropi.imp = equip->imp;
			dropi.iwatk = equip->iwatk;
			dropi.imatk = equip->imatk;
			dropi.iwdef = equip->iwdef;
			dropi.imdef = equip->imdef;
			dropi.iacc = equip->iacc;
			dropi.iavo = equip->iavo;
			dropi.ihand = equip->ihand;
			dropi.ijump = equip->ijump;
			dropi.ispeed = equip->ispeed;
			drop->setDropInfo(dropi);
			drop->doDrop(dropper);
			player->inv->deleteEquip(num);
			return;
		}
		for(int i=0; i<player->inv->getEquipNum(); i++){

			if(player->inv->getEquipPos(i) == slot1){
				for(int j=0; j<player->inv->getEquipNum(); j++){
					if(player->inv->getEquipPos(j) == slot2){
						player->inv->setEquipPos(j, slot1);
						break;
					}
				}		
				player->inv->setEquipPos(i, slot2);	
				break;
			}
		}	
		InventoryPacket::moveItem(player, inv, slot1, slot2);
		InventoryPacket::updatePlayer(player);
	}
	else {
		if(slot2 == 0){
			short amount = BufferUtilities::getShort(packet+9);
			Item* item = NULL;
			int num;
			for(int i=0; i<player->inv->getItemNum(); i++){
				if(player->inv->getItemPos(i) == slot1 && player->inv->getItem(i)->inv == inv){
					item = player->inv->getItem(i);	
					num = i;
					break;
				}
			}
			if(item==NULL)
				return;
			if (ISSTAR(item->id)) amount = item->amount;
			if(item->amount - amount == 0){
				item->amount = 0;
				InventoryPacket::moveItem(player, inv, slot1, slot2);
			}
			else{
				item->amount -= amount;
				InventoryPacket::moveItemS(player, inv, slot1, item->amount);
			}
			Drop* drop = new Drop(player->getMap());
			drop->setID(item->id);
			drop->setOwner(player->getPlayerid());
			drop->setTime(0);
			drop->setAmount(amount);
			Dropped dropper;
			dropper.id = player->getPlayerid();
			dropper.pos = player->getPos();
			drop->setPos(player->getPos());
			drop->doDrop(dropper);
			if(item->amount == 0)
				player->inv->deleteItem(num);
			return;
		}
		int stack=0;
		int amount1=0;
		int amount2=0;
		for(int i=0; i<player->inv->getItemNum(); i++){
			if(player->inv->getItemPos(i) == slot1 && player->inv->getItem(i)->inv == inv){
				for(int j=0; j<player->inv->getItemNum(); j++){
					if(player->inv->getItemPos(j) == slot2 && player->inv->getItem(j)->inv == inv){
						if(player->inv->getItem(i)->id == player->inv->getItem(j)->id){
							if(player->inv->getItem(j)->amount + player->inv->getItem(i)->amount <= Drops::items[player->inv->getItem(i)->id].maxslot){
								stack=1;
								Item* newitem = player->inv->getItem(j);
								newitem->amount += player->inv->getItem(i)->amount;
								amount1 = newitem->amount;
								player->inv->setItem(newitem, j);
								player->inv->deleteItem(i);
							}
							else{
								stack=2;
								Item* newitem1 = player->inv->getItem(j);
								Item* newitem2 = player->inv->getItem(i);
								int amountt = player->inv->getItem(j)->amount;
								newitem1->amount = Drops::items[player->inv->getItem(j)->id].maxslot;
								newitem2->amount -= newitem1->amount - amountt;
								amount2 = newitem1->amount;
								amount1 = newitem2->amount;
								player->inv->setItem(newitem1, j);
								player->inv->setItem(newitem2, i);
							}
						}
						else
							player->inv->setItemPos(j, slot1);
						break;
					}
				}	
				if(stack==0)
					player->inv->setItemPos(i, slot2);	
				break;
			}
		}			
		if(stack==0)
			InventoryPacket::moveItem(player, inv, slot1, slot2);
		else if(stack==1){
			InventoryPacket::moveItemS(player, inv, slot2, amount1);
			InventoryPacket::moveItem(player, inv, slot1, 0);
		}
		else if(stack==2)
			InventoryPacket::moveItemS2(player, inv, slot1, amount1, slot2, amount2);

	}
}

int Inventory::findSlot(Player* player, int itemid ,char inv, short amount){
	if(inv==1){
		int pos = 1;
		for(unsigned int i=1; i<=100; i++){
			int check=0;
			for(int j=0; j<player->inv->getEquipNum(); j++){
				if(player->inv->getEquipPos(j) == i){
					check = 1;
					if(i==100)
						return 0;
					break;
				}
			}
			if(!check){
				pos = i;
				break;
			}
		}
		return pos;
	}
	else{
		int pos = 1;
		for(unsigned int i=1; i<=100; i++){
			int check=0;
			for(int j=0; j<player->inv->getItemNum(); j++){
				if(((player->inv->getItemPos(j) == i) && !(player->inv->getItem(j)->id == itemid && player->inv->getItem(j)->amount + amount <= Drops::items[itemid].maxslot)) && Drops::items[player->inv->getItem(j)->id].type == inv){
					check = 1;
					if(i==100)
						return 0;
					break;
				}
			}
			if(!check){
				pos = i;
				break;
			}
		}		
		return pos;
	}
	return 0;
}

Equip* Inventory::setEquipStats(Player* player, int equipid){
	EquipInfo ei = Drops::equips[equipid];
	Equip* equip = new Equip;
	equip->id = equipid;
	equip->slots = ei.slots;
	equip->scrolls = 0;
	equip->type = ei.type;
	equip->pos = findSlot(player, equipid, 1, 1);
	equip->istr = ei.istr;
	equip->idex = ei.idex;
	equip->iint = ei.iint;
	equip->iluk = ei.iluk;
	equip->ihp = ei.ihp;
	equip->imp = ei.imp;
	equip->iwatk = ei.iwatk;
	equip->imatk = ei.imatk;
	equip->iwdef = ei.iwdef;
	equip->imdef = ei.imdef;
	equip->iacc = ei.iacc;
	equip->iavo = ei.iavo;
	equip->ihand = ei.ihand;
	equip->ijump = ei.ijump;
	equip->ispeed = ei.ispeed;	
	return equip;
}

void Inventory::addEquip(Player* player, Equip* equip, bool is){
	InventoryPacket::addEquip(player, equip, is);
	player->inv->addEquip(equip);
}

void Inventory::addItem(Player* player, Item* item, bool is){
	if (!ISSTAR(item->id)){
		for(int i=0; i<player->inv->getItemNum(); i++){
			if(player->inv->getItem(i)->id == item->id && Drops::items[player->inv->getItem(i)->id].type == item->inv && player->inv->getItem(i)->amount < Drops::items[item->id].maxslot){
				Item* olditem = player->inv->getItem(i);
				if(item->amount + olditem->amount > Drops::items[item->id].maxslot){
					int amount = Drops::items[item->id].maxslot-olditem->amount;
					item->amount-=amount;
					olditem->amount = Drops::items[item->id].maxslot;
					InventoryPacket::addItem(player, olditem, is);
				}
				else{
					item->amount += olditem->amount;
					item->pos = olditem->pos;
					player->inv->deleteItem(i);
					player->inv->addItem(item);
					InventoryPacket::addItem(player, item, is);
					return;
				}
			}
		}
	}
	InventoryPacket::addNewItem(player, item, is);
	player->inv->addItem(item);
}

void Inventory::useShop(Player* player, unsigned char* packet){
	char type = packet[0];
	if(type == 0){
		int item = BufferUtilities::getInt(packet+3);
		short howmany = BufferUtilities::getShort(packet+7);
		if(Shops::getPrice(player, item) == 0){
			// hacking
			return;
		}
		bool isequip=false;
		if(Drops::equips.find(item) != Drops::equips.end())
			isequip = 1;
		if(isequip){
			if(player->inv->getMesos() < Shops::getPrice(player, item)){
				// hacking
				return;
			}
		}
		else{
			if(player->inv->getMesos() < Shops::getPrice(player, item)*howmany){
				// hacking
				return;
			}
		}
		if(isequip){
			Equip* equip = setEquipStats(player, item);
			addEquip(player, equip);
			player->inv->setMesos(player->inv->getMesos() - Shops::getPrice(player, item));
			InventoryPacket::bought(player);
		}
		else{
			if(Drops::items.find(item) == Drops::items.end())
				return;
			char type = Drops::items[item].type;
			Item* newitem = new Item;
			newitem->amount = howmany;
			newitem->id = item;
			newitem->inv = type;
			newitem->pos = findSlot(player, item , type, howmany);
			addItem(player, newitem);
			player->inv->setMesos(player->inv->getMesos() - Shops::getPrice(player, item)*howmany);
			InventoryPacket::bought(player);
		}
	}
	else if(type == 1){
		short slot = BufferUtilities::getShort(packet+1);
		int item = BufferUtilities::getInt(packet+3);
		short amount = BufferUtilities::getShort(packet+7);
		char inv = item/1000000;
		if(inv == 1){
			bool check=false;
			for(int i=0; i<player->inv->getEquipNum(); i++){
				if(player->inv->getEquip(i)->pos == slot && player->inv->getEquip(i)->id == item){
					InventoryPacket::moveItem(player, 1, slot, 0);
					player->inv->deleteEquip(i);
					check = true;
					break;
				}
			}
			if(!check){
				// hacking
				return;
			}
			player->inv->setMesos(player->inv->getMesos() + Drops::equips[item].price*amount);
		}
		else{
			if(player->inv->getItemBySlot(item, slot, inv) < amount){
				// hacking
				return;
			}
			takeItemSlot(player, slot, Drops::items[item].type, amount);
			player->inv->setMesos(player->inv->getMesos() + Drops::items[item].price*amount);
		}
		InventoryPacket::bought(player);
	}
}

void Inventory::addNewItem(Player* player, int item, int howmany){
	bool isequip=false;
	if(Drops::equips.find(item) != Drops::equips.end())
		isequip = 1;
	if(isequip){
		Equip* equip = setEquipStats(player, item);
		addEquip(player, equip);
	}
	else{
		if(Drops::items.find(item) == Drops::items.end())
			return;
		char type = Drops::items[item].type;
		Item* newitem = new Item;
		newitem->amount = howmany;
		newitem->id = item;
		newitem->inv = type;
		newitem->pos = findSlot(player, item , type, howmany);
		addItem(player, newitem);
	}
}

void Inventory::takeItem(Player* player, int item, int howmany){
	for(int i=0; i<player->inv->getItemNum(); i++)
		if(player->inv->getItem(i)->id == item){
			if(player->inv->getItem(i)->amount>=howmany){
				Item* item = player->inv->getItem(i);
				item->amount-=howmany;
				howmany=0;
				if(item->amount == 0){
					InventoryPacket::moveItem(player, item->inv, item->pos, 0);
					player->inv->deleteItem(i);
				}
				else
					InventoryPacket::moveItemS(player, item->inv, item->pos, item->amount);
				break;
			}
			else{
				Item* item = player->inv->getItem(i);
				howmany-=item->amount;
				item->amount=0;
				InventoryPacket::moveItem(player, item->inv, item->pos, 0);
				player->inv->deleteItem(i);
			}
		}
}

void Inventory::takeItemSlot(Player* player, short slot, char inv, short amount){
	for(int i=0; i<player->inv->getItemNum(); i++)
		if(player->inv->getItem(i)->pos == slot && player->inv->getItem(i)->inv == inv){
			Item* item = player->inv->getItem(i);
			item->amount-=amount;
			if(item->amount == 0){
				InventoryPacket::moveItem(player, item->inv, item->pos, 0);
				player->inv->deleteItem(i);
			}
			else
				InventoryPacket::moveItemS(player, item->inv, item->pos, item->amount);
			break;
		}
}

void Inventory::useItem(Player *player, unsigned char *packet){
	short slot = BufferUtilities::getShort(packet+4);
	int itemid = BufferUtilities::getInt(packet+6);
	if(player->inv->getItemBySlot(itemid, slot, 2) == 0){
		// hacking
		return;
	}
	takeItemSlot(player, slot, 2, 1);
	// Alchemist
	int alchemist = 0;
	if(player->skills->getSkillLevel(4110000)>0){
		alchemist = Skills::skills[4110000][player->skills->getSkillLevel(4110000)].x;
	}
	if(Drops::consumes[itemid].hp>0){
		player->setHP(player->getHP()+Drops::consumes[itemid].hp + ((Drops::consumes[itemid].hp * alchemist)/100));
	}
	if(Drops::consumes[itemid].mp>0){
		player->setMP(player->getMP()+Drops::consumes[itemid].mp + ((Drops::consumes[itemid].mp * alchemist)/100));
	}
	else
		player->setMP(player->getMP(), 1);
	if(Drops::consumes[itemid].hpr>0){
		player->setHP(player->getHP() + ((Drops::consumes[itemid].hpr*player->getMHP()/100) + ((Drops::consumes[itemid].hpr * alchemist)/100)));
	}
	if(Drops::consumes[itemid].mpr>0){
		player->setMP(player->getMP() + ((Drops::consumes[itemid].mpr*player->getMMP()/100) + ((Drops::consumes[itemid].mpr * alchemist)/100)));
	}
	// Item buffs
	if(Drops::consumes[itemid].time>0){
		vector <short> vals;
		unsigned char types[8];
		// Set all types to 0 initially
		types[0] = 0;
		types[1] = 0;
		types[2] = 0;
		types[3] = 0;
		types[4] = 0;
		types[5] = 0;
		types[6] = 0;
		types[7] = 0;

		if(Drops::consumes[itemid].watk>0){
			types[0] += 0x01;
			vals.push_back(Drops::consumes[itemid].watk);
		}
		if(Drops::consumes[itemid].wdef>0){
			types[0] += 0x02;
			vals.push_back(Drops::consumes[itemid].wdef);
		}
		if(Drops::consumes[itemid].matk>0){
			types[0] += 0x04;
			vals.push_back(Drops::consumes[itemid].matk);
		}
		if(Drops::consumes[itemid].mdef>0){
			types[0] += 0x08;
			vals.push_back(Drops::consumes[itemid].mdef);
		}
		if(Drops::consumes[itemid].acc>0){
			types[0] += 0x10;
			vals.push_back(Drops::consumes[itemid].acc);
		}
		if(Drops::consumes[itemid].avo>0){
			types[0] += 0x20;
			vals.push_back(Drops::consumes[itemid].avo);
		}
		if(Drops::consumes[itemid].speed>0){
			types[0] += 0x80;
			vals.push_back(Drops::consumes[itemid].speed);
		}
		if(Drops::consumes[itemid].jump>0){
			types[1] = 0x01;
			vals.push_back(Drops::consumes[itemid].jump);
		}
		bool isMorph = false;
		if(Drops::consumes[itemid].morph>0){
			types[5] = 0x02;
			vals.push_back(Drops::consumes[itemid].morph);
			isMorph = true;
		}
		InventoryPacket::useItem(player, itemid, Drops::consumes[itemid].time*1000, types, vals, isMorph);
		ItemTimer::Instance()->stop(player, itemid);
		ItemTimer::Instance()->setItemTimer(player, itemid, Drops::consumes[itemid].time*1000);
	}
}
// Cancel item buffs
void Inventory::cancelItem(Player *player, unsigned char* packet){
	int itemid = BufferUtilities::getInt(packet)*-1;
	ItemTimer::Instance()->stop(player, itemid);
	Inventory::endItem(player, itemid);
}
// End item buffs
void Inventory::endItem(Player *player, int itemid){
	unsigned char types[8];
	// Set all types to 0 initially
	types[0] = 0;
	types[1] = 0;
	types[2] = 0;
	types[3] = 0;
	types[4] = 0;
	types[5] = 0;
	types[6] = 0;
	types[7] = 0;

	if(Drops::consumes[itemid].watk>0){
		types[0] += 0x01;
	}
	if(Drops::consumes[itemid].wdef>0){
		types[0] += 0x02;
	}
	if(Drops::consumes[itemid].matk>0){
		types[0] += 0x04;
	}
	if(Drops::consumes[itemid].mdef>0){
		types[0] += 0x08;
	}
	if(Drops::consumes[itemid].acc>0){
		types[0] += 0x10;
	}
	if(Drops::consumes[itemid].avo>0){
		types[0] += 0x20;
	}
	if(Drops::consumes[itemid].speed>0){
		types[0] += 0x80;
	}
	if(Drops::consumes[itemid].jump>0){
		types[1] = 0x01;
	}
	if(Drops::consumes[itemid].morph>0){
		types[5] = 0x02;
	}
	InventoryPacket::endItem(player, types);
}

void Inventory::useChair(Player* player, unsigned char* packet){
	int chairid = BufferUtilities::getInt(packet);
	player->setChair(chairid);
	InventoryPacket::sitChair(player, Maps::info[player->getMap()].Players, chairid);
}

void Inventory::stopChair(Player* player, unsigned char* packet){
	player->setChair(0);
	InventoryPacket::stopChair(player, Maps::info[player->getMap()].Players);
}

int Inventory::isCash(int itemid){
	if(Drops::equips.find(itemid) != Drops::equips.end() && Drops::equips[itemid].cash)
		return 1;
	return 0;
}

void Inventory::useSummonBag(Player* player, unsigned char* packet){
	short slot = BufferUtilities::getShort(packet+4);
	int itemid = BufferUtilities::getInt(packet+6);
	if(player->inv->getItemBySlot(itemid, slot, 2) == 0){
		// hacking
		return;
	}
	if(Drops::items.find(itemid) == Drops::items.end())
		return;
	takeItemSlot(player, slot, 2, 1);
	for(unsigned int i=0; i<Drops::consumes[itemid].mobs.size(); i++){
		if(rand()%100<Drops::consumes[itemid].mobs[i].chance){
			Mobs::spawnMob(player, Drops::consumes[itemid].mobs[i].mobid);
		}
	}
}
void Inventory::useReturnScroll(Player* player, unsigned char* packet){
	short slot = BufferUtilities::getShort(packet+4);
	int itemid = BufferUtilities::getInt(packet+6);
	if(player->inv->getItemBySlot(itemid, slot, 2) == 0){
		// hacking
		return;
	}
	if(Drops::items.find(itemid) == Drops::items.end())
		return;
	takeItemSlot(player, slot, 2, 1);
	int map = Drops::consumes[itemid].moveTo;
	if(map == 999999999)
		Maps::changeMap(player, Maps::info[player->getMap()].rm, 0);
	else
		Maps::changeMap(player, map, 0);
}
void Inventory::useScroll(Player* player, unsigned char* packet){
	short slot = BufferUtilities::getShort(packet+4);
	short eslot = BufferUtilities::getShort(packet+6);
	int itemid=0;
	Equip* equip = NULL;
	for(int i=0; i<player->inv->getItemNum(); i++){
		if(player->inv->getItem(i)->pos == slot && player->inv->getItem(i)->inv == 2)	{
			itemid = player->inv->getItem(i)->id;
			break;
		}
	}
	for(int i=0; i<player->inv->getEquipNum(); i++){
		if(player->inv->getEquip(i)->pos == eslot)	{
			equip = player->inv->getEquip(i);
			break;
		}
	}
	bool cursed=false;
	if(itemid == 0 || equip == NULL)
		return;
	if(equip->slots > 0){
		if(Drops::items.find(itemid) == Drops::items.end())
			return;
		takeItemSlot(player, slot, 2, 1);
		equip->slots--;
		if(rand()%100<Drops::consumes[itemid].success){
			InventoryPacket::useScroll(player, Maps::info[player->getMap()].Players ,true);
			equip->istr+=Drops::consumes[itemid].istr;
			equip->idex+=Drops::consumes[itemid].idex;
			equip->iint+=Drops::consumes[itemid].iint;
			equip->iluk+=Drops::consumes[itemid].iluk;
			equip->ihp+=Drops::consumes[itemid].ihp;
			equip->imp+=Drops::consumes[itemid].imp;
			equip->iwatk+=Drops::consumes[itemid].iwatk;
			equip->imatk+=Drops::consumes[itemid].imatk;
			equip->iwdef+=Drops::consumes[itemid].iwdef;
			equip->imdef+=Drops::consumes[itemid].imdef;
			equip->iacc+=Drops::consumes[itemid].iacc;
			equip->iavo+=Drops::consumes[itemid].iavo;
			equip->ihand+=Drops::consumes[itemid].ihand;
			equip->ijump+=Drops::consumes[itemid].ijump;
			equip->ispeed+=Drops::consumes[itemid].ispeed;
			equip->scrolls++;
		}
		else{
			if(rand()%100<Drops::consumes[itemid].cursed){
				cursed = true;
				for(int i=0; i<player->inv->getEquipNum(); i++){
					if(player->inv->getEquip(i)->pos == eslot){
						InventoryPacket::moveItem(player, 1, eslot, 0);
						player->inv->deleteEquip(i);	
						break;
					}
				}
			}
			InventoryPacket::useScroll(player, Maps::info[player->getMap()].Players , false);
		}
	}
	if(!cursed) 
		InventoryPacket::addEquip(player, equip, 1);
	InventoryPacket::updatePlayer(player); 
}

void Inventory::useMegaphone(Player* player, unsigned char *packet){
	char type = packet[0];
	int itemid = BufferUtilities::getInt(packet+2);
	char msg[200];
	int msgLength = BufferUtilities::getShort(packet+6);
	BufferUtilities::getString(packet+8, msgLength, msg);
	if(itemid == 5071000){ // Megaphone
		InventoryPacket::showMegaphone(player, Maps::info[player->getMap()].Players, msg);
	}
	else if(itemid == 5072000){ // Super Megaphone
		int whisper = packet[8+msgLength];
		InventoryPacket::showSuperMegaphone(player, msg, whisper);
	}
	else if (itemid/10000 == 539) { // Messenger
		char msgs[3][15];
		packet = packet + 8 + msgLength;
		for (int i = 0; i < 3; i++) {
			msgLength = BufferUtilities::getShort(packet);
			BufferUtilities::getString(packet+2, msgLength, msgs[i]);
			packet = packet + 2 + msgLength;
		}
		int offset = 0;
		int ffcount = 0;
		while (ffcount != 2) {
			if (packet[offset] == 0xff)
				ffcount++;
			offset++;
		}
		InventoryPacket::showMessenger(player, msg, msgs[0], msgs[1], msgs[2], packet, offset+16, itemid);
	}
	Inventory::takeItem(player, itemid, 1);
}
