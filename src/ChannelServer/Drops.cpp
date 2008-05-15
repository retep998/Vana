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
#include "Drops.h"
#include "Mobs.h"
#include "Maps.h"
#include "DropsPacket.h"
#include "Inventory.h"
#include "Quests.h"
#include "Randomizer.h"
#include "BufferUtilities.h"
#include <windows.h>

hash_map <int, MobDropsInfo> Drops::dropsinfo;
hash_map <int, Mesos> Drops::mesos;
hash_map <int, EquipInfo> Drops::equips;
hash_map <int, ItemInfo> Drops::items;
hash_map <int, FootholdsInfo> Drops::foots;
hash_map <int, int> Drops::objids;
hash_map <int, vector<Drop*>> Drops::drops;
hash_map <int, ConsumeInfo> Drops::consumes;

Drop* Drop::getDrop(int objid, int mapid){
	for(unsigned int i=0; i<Drops::drops[mapid].size(); i++){
		if(Drops::drops[mapid][i]->getObjID() == objid)
			return Drops::drops[mapid][i];
	}
	return NULL;
}

Pos Drops::findFloor(Pos pos, int map){
	short x = pos.x;
	short y = pos.y-100;
	bool first=1;
	short maxy;
	for(unsigned int i=0; i<Drops::foots[map].size(); i++){
		if((x>Drops::foots[map][i].x1 && x<Drops::foots[map][i].x2) || (x>Drops::foots[map][i].x2 && x<Drops::foots[map][i].x1)){
			if(first){
				maxy = (short) ( (float)( Drops::foots[map][i].y1 - Drops::foots[map][i].y2 ) / ( Drops::foots[map][i].x1 - Drops::foots[map][i].x2 ) * x - Drops::foots[map][i].x1 * (float) ( Drops::foots[map][i].y1 - Drops::foots[map][i].y2 ) / ( Drops::foots[map][i].x1 - Drops::foots[map][i].x2 ) + Drops::foots[map][i].y1 );
				if(maxy >= y)
					first=0;
			}
			else{
				short cmax = (short) ( (float)( Drops::foots[map][i].y1 - Drops::foots[map][i].y2 ) / ( Drops::foots[map][i].x1 - Drops::foots[map][i].x2 ) * x - Drops::foots[map][i].x1 * (float) ( Drops::foots[map][i].y1 - Drops::foots[map][i].y2 ) / ( Drops::foots[map][i].x1 - Drops::foots[map][i].x2 ) + Drops::foots[map][i].y1 );
				if(cmax < maxy && cmax >= y)
					maxy = cmax;
			}
		}
	}
	if(!first){
		Pos newpos;
		newpos.x = x;
		newpos.y = maxy;
		return newpos;
	}
	return pos;
}

void Drop::doDrop(Dropped dropped){
	setDropped(clock());
	setPos(Drops::findFloor(getPos(), getMap()));
	if(!isQuest())
		DropsPacket::drop(Maps::info[getMap()].Players, this, dropped);
	else{
		if(Players::players.find(playerid) == Players::players.end())
			return;
		if(Players::players[playerid]->getMap() == getMap())
			DropsPacket::dropForPlayer(Players::players[playerid], this, dropped);
	}
}

void Drop::takeDrop(Player* player){
	for(unsigned int i=0; i<Drops::drops[player->getMap()].size(); i++){
		if(Drops::drops[player->getMap()][i] == this){
			Drops::drops[player->getMap()].erase(Drops::drops[player->getMap()].begin()+i);
			break;
		}
	}
	DropsPacket::takeDrop(player, Maps::info[player->getMap()].Players, this);
}

void Drop::showDrop(Player* player){
	if(isQuest() && player->getPlayerid() != playerid)
		return;
	DropsPacket::showDrop(player, this);
}

void Drop::removeDrop(){

}

void Drops::dropMob(Player* player, Mob* mob){
	MobDropsInfo drop = dropsinfo[mob->getMobID()];
	int d=0;
	for(unsigned int k=0; k<drop.size(); k++){
		if(Randomizer::Instance()->randInt(9999) < drop[k].chance){
			if(drop[k].quest>0){
				if(!player->quests->isQuestActive(drop[k].quest))
					continue;
				int request=0;
				for(unsigned int i=0; i<Quests::quests[drop[k].quest].rewards.size(); i++){
					if(Quests::quests[drop[k].quest].rewards[i].id == drop[k].id){
						request = Quests::quests[drop[k].quest].rewards[i].count;
					}
				}
				if(player->inv->getItemAmount(drop[k].id) > request)
					continue;
			}
			Drop* drp = new Drop(player->getMap());
			if(drop[k].quest>0){
				drp->setIsQuest(1);
				drp->setPlayer(player->getPlayerid());
				drp->setQuest(drop[k].quest);
			}
			drp->setID(drop[k].id);
			drp->setAmount(1);
			drp->setOwner(player->getPlayerid());
			if(drop[k].id/1000000 == 1){
				drp->setEquip(true);
				EquipInfo ei = equips[drp->getID()];
				DropInfo dp;
				dp.scrolls = 0;
				dp.type =  ei.type;
				dp.slots = ei.slots;
				dp.scrolls = 0;
				if(ei.istr > 0)
					dp.istr = ei.istr + Randomizer::Instance()->randInt(2)-1;
				else
					dp.istr = 0;
				if(ei.idex > 0)
					dp.idex = ei.idex + Randomizer::Instance()->randInt(2)-1;
				else
					dp.idex = 0;
				if(ei.iint > 0)
					dp.iint = ei.iint + Randomizer::Instance()->randInt(2)-1;
				else
					dp.iint = 0;
				if(ei.iluk > 0)
					dp.iluk = ei.iluk + Randomizer::Instance()->randInt(2)-1;
				else
					dp.iluk = 0;
				if(ei.ihp > 0)
					dp.ihp = ei.ihp + Randomizer::Instance()->randInt(10)-5;
				else
					dp.ihp = 0;
				if(ei.imp > 0)
					dp.imp = ei.imp + Randomizer::Instance()->randInt(10)-5;
				else
					dp.imp = 0;
				if(ei.iwatk > 0)
					dp.iwatk = ei.iwatk + Randomizer::Instance()->randInt(10)-5;
				else
					dp.iwatk = 0;
				if(ei.imatk > 0)
					dp.imatk = ei.imatk + Randomizer::Instance()->randInt(10)-5;
				else
					dp.imatk = 0;
				if(ei.iwdef > 0)
					dp.iwdef = ei.iwdef + Randomizer::Instance()->randInt(10)-5;
				else
					dp.iwdef = 0;
				if(ei.imdef > 0)
					dp.imdef = ei.imdef + Randomizer::Instance()->randInt(10)-5;
				else
					dp.imdef = 0;
				if(ei.iacc > 0)
					dp.iacc = ei.iacc + Randomizer::Instance()->randInt(2)-1;
				else
					dp.iacc = 0;
				if(ei.iavo > 0)
					dp.iavo = ei.iavo + Randomizer::Instance()->randInt(2)-1;
				else
					dp.iavo = 0;
				dp.ihand = ei.ihand;
				if(ei.ijump > 0)
					dp.ijump = ei.ijump + Randomizer::Instance()->randInt(2)-1;
				else
					dp.ijump = 0;
				if(ei.ispeed > 0)
					dp.ispeed = ei.ispeed + Randomizer::Instance()->randInt(2)-1;	
				else
					dp.ispeed = 0;
				drp->setDropInfo(dp);
			}
			drp->setTime(100);
			Dropped dropper;
			dropper.id = mob->getID();
			dropper.pos = mob->getPos();
			Pos pos;
			if(d%2){
				pos.x = mob->getPos().x+25*((d+1)/2);
				pos.y = mob->getPos().y;
			}
			else{
				pos.x = mob->getPos().x-25*(d/2);
				pos.y = mob->getPos().y;
			}
			drp->setPos(pos);
			drp->setOwner(player->getPlayerid());
			drp->doDrop(dropper);
			d++;
		}
	}
	int nm = mesos[mob->getMobID()].min;
	int xm = mesos[mob->getMobID()].max;	
	if(xm > 0 && nm > 0){
		Drop* drp = new Drop(player->getMap());
		drp->setID(Randomizer::Instance()->randInt(xm-nm)+nm);
		drp->setMesos(true);
		drp->setOwner(player->getPlayerid());
		drp->setTime(100);
		Dropped dropper;
		dropper.id = mob->getID();
		dropper.pos = mob->getPos();
		Pos pos;
		if(d%2){
			pos.x = mob->getPos().x+25*((d+1)/2);
			pos.y = mob->getPos().y;
		}
		else{
			pos.x = mob->getPos().x-25*(d/2);
			pos.y = mob->getPos().y;
		}
		drp->setPos(pos);
		drp->doDrop(dropper);
	}
}

void Drops::showDrops(Player* player){
	for(unsigned int i=0; i<drops[player->getMap()].size(); i++){
		drops[player->getMap()][i]->showDrop(player);
	}
}

int getDistance(Pos a, Pos b);

void Drops::lootItem(Player* player, unsigned char*packet){
	int itemid = BufferUtilities::getInt(packet+9);
	Drop* drop = Drop::getDrop(itemid, player->getMap());
	if(drop == NULL){
		DropsPacket::dontTake(player);
		return;
	}
	if(getDistance(drop->getPos(), player->getPos()) > 300){
		player->addWarning();
	}
	if(drop->isQuest()){
		int request=0;
		for(unsigned int i=0; i<Quests::quests[drop->getQuest()].rewards.size(); i++){
			if(Quests::quests[drop->getQuest()].rewards[i].id == drop->getID()){
				request = Quests::quests[drop->getQuest()].rewards[i].count;
			}
		}
		if(player->inv->getItemAmount(drop->getID()) > request || !player->quests->isQuestActive(drop->getQuest())){
			DropsPacket::takeNote(player, 0, 0, 0);
			DropsPacket::dontTake(player);
			return;
		}
	}
	if(drop->getMesos()){
		player->inv->setMesos(player->inv->getMesos() + drop->getID(), 1);
		DropsPacket::takeNote(player, drop->getID(), 1, 0);
	}
	else{
		if(drop->getEquip()){
			Equip* equip = new Equip;
			DropInfo ei = drop->getDropInfo();
			equip->id = drop->getID();
			equip->slots = ei.slots;
			equip->scrolls = ei.scrolls;
			equip->type = ei.type;
			equip->pos = Inventory::findSlot(player, drop->getID(), 1, 1);
			if(equip->pos == 0){
				DropsPacket::takeNote(player, 0, 0, 0);
				DropsPacket::dontTake(player);
				delete equip;
				return;
			}
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
			Inventory::addEquip(player, equip, 1);
		}
		else{
			char type = Drops::items[drop->getID()].type;
			Item* newitem = new Item;
			newitem->amount = drop->getAmount();
			newitem->id = drop->getID();
			newitem->inv = type;
			newitem->pos = Inventory::findSlot(player, drop->getID() , type, drop->getAmount());
			if(newitem->pos == 0){
				DropsPacket::takeNote(player, 0, 0, 0);
				DropsPacket::dontTake(player);	
				delete newitem;
				return;
			}
			Inventory::addItem(player, newitem, 1);
		}
		DropsPacket::takeNote(player, drop->getID(), 0, drop->getAmount());
	}
	drop->takeDrop(player);
	delete drop;
}

void Drops::addDrop(int id, MobDropsInfo drops){
	dropsinfo[id] = drops;
}
void Drops::addEquip(int id, EquipInfo equip){
	equips[id] = equip;
}
void Drops::addItem(int id, ItemInfo item){
	items[id] = item;
}
void Drops::addConsume(int id, ConsumeInfo cons){
	consumes[id] = cons;
}
void Drops::addMesos(int id, Mesos meso){
	mesos[id] = meso;
}
void Drops::addFoothold(int id, FootholdsInfo foot){
	foots[id] = foot;
}

void Drops::dropMesos(Player* player, unsigned char* packet){
	int amount = BufferUtilities::getInt(packet+4);
	if(amount < 10 || amount >50000){
		// hacking
		return;
	}
	player->inv->setMesos(player->inv->getMesos()-amount, 1);
	Drop* drop = new Drop(player->getMap());
	drop->setID(amount);
	drop->setMesos(true);
	drop->setOwner(player->getPlayerid());
	drop->setTime(0);
	Dropped dropper;
	dropper.id = player->getPlayerid();
	dropper.pos = player->getPos();
	drop->setPos(player->getPos());
	drop->doDrop(dropper);
}

void Drops::checkDrops(int mapid){
	int t = clock() - 60000;
	for(unsigned int i=0; i<drops[mapid].size(); i++){
		if(drops[mapid][i]->getDropped() < t){
			DropsPacket::removeDrop(Maps::info[mapid].Players, drops[mapid][i]);
			drops[mapid].erase(drops[mapid].begin()+i);
			i--;
		}
	}
}
