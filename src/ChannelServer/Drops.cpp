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
#include "Reactors.h"
#include "DropsPacket.h"
#include "Inventory.h"
#include "PlayerInventory.h"
#include "Quests.h"
#include "Shops.h"
#include "Randomizer.h"
#include "BufferUtilities.h"
#include "Pos.h"
#include "ReadPacket.h"

hash_map <int, MobDropsInfo> Drops::dropsinfo;
hash_map <int, Mesos> Drops::mesos;
hash_map <int, EquipInfo> Drops::equips;
hash_map <int, ItemInfo> Drops::items;
hash_map <int, ConsumeInfo> Drops::consumes;

// Drop class
Drop::Drop (int mapid, int mesos, Pos pos, int ownerid) : mapid(mapid), pos(pos), ismesos(true), isequip(false), mesos(mesos), questid(0), dropped(0), playerid(0) {
	Maps::maps[mapid]->addDrop(this);
}

Drop::Drop (int mapid, Equip equip, Pos pos, int ownerid) : mapid(mapid), pos(pos), ismesos(false), isequip(true), equip(equip), questid(0), dropped(0), playerid(0) {
	Maps::maps[mapid]->addDrop(this);
}

Drop::Drop (int mapid, Item item, Pos pos, int ownerid) : mapid(mapid), pos(pos), ismesos(false), isequip(false), item(item), questid(0), dropped(0), playerid(0) {
	Maps::maps[mapid]->addDrop(this);
}

int Drop::getObjectID() {
	if (ismesos)
		return mesos;
	else if (isequip)
		return equip.id;
	else
		return item.id;
}

int Drop::getAmount() {
	if (isequip)
		return 1;
	else
		return item.amount;
}

void Drop::doDrop(Dropped dropped) {
	setDropped(clock());
	if (!isQuest())
		DropsPacket::drop(Maps::maps[mapid]->getPlayers(), this, dropped);
	else {
		if (Players::players.find(playerid) == Players::players.end())
			return;
		if (Players::players[playerid]->getMap() == this->mapid)
			DropsPacket::dropForPlayer(Players::players[playerid], this, dropped);
	}
}

void Drop::showDrop(Player *player) {
	if (isQuest() && player->getPlayerid() != playerid)
		return;
	DropsPacket::showDrop(player, this);
}

void Drop::takeDrop(Player *player) {
	Maps::maps[mapid]->removeDrop(this->id);
	DropsPacket::takeDrop(player, Maps::maps[mapid]->getPlayers(), this);
	delete this;
}

void Drop::removeDrop() {
	Maps::maps[this->mapid]->removeDrop(this->id);
	DropsPacket::removeDrop(Maps::maps[this->mapid]->getPlayers(), this);
	delete this;
}

// Drops namespace
void Drops::dropMob(Player *player, Mob *mob) {
	MobDropsInfo drops = dropsinfo[mob->getMobID()];
	int d = 0;
	for (size_t k = 0; k < drops.size(); k++) {
		if (Randomizer::Instance()->randInt(9999) < drops[k].chance * ChannelServer::Instance()->getDroprate()) {
			if (drops[k].quest > 0) {
				if (!player->quests->isQuestActive(drops[k].quest))
					continue;
				int request = 0;
				for (size_t i = 0; i < Quests::quests[drops[k].quest].rewards.size(); i++) {
					if (Quests::quests[drops[k].quest].rewards[i].id == drops[k].id) {
						request = Quests::quests[drops[k].quest].rewards[i].count;
					}
				}
				if (player->inv->getItemAmount(drops[k].id) > request)
					continue;
			}

			Pos pos;
			if (d%2) {
				pos.x = mob->getPos().x+25*((d+1)/2);
				pos.y = mob->getPos().y;
			}
			else {
				pos.x = mob->getPos().x-25*(d/2);
				pos.y = mob->getPos().y;
			}

			Drop *drop = 0;

			if (drops[k].id/1000000 == 1) {
				EquipInfo ei = equips[drops[k].id];
				Equip equip;
				equip.type =  ei.type;
				equip.slots = ei.slots;
				equip.scrolls = 0;
				if (ei.istr > 0)
					equip.istr = ei.istr + Randomizer::Instance()->randInt(2)-1;
				else
					equip.istr = 0;
				if (ei.idex > 0)
					equip.idex = ei.idex + Randomizer::Instance()->randInt(2)-1;
				else
					equip.idex = 0;
				if (ei.iint > 0)
					equip.iint = ei.iint + Randomizer::Instance()->randInt(2)-1;
				else
					equip.iint = 0;
				if (ei.iluk > 0)
					equip.iluk = ei.iluk + Randomizer::Instance()->randInt(2)-1;
				else
					equip.iluk = 0;
				if (ei.ihp > 0)
					equip.ihp = ei.ihp + Randomizer::Instance()->randInt(10)-5;
				else
					equip.ihp = 0;
				if (ei.imp > 0)
					equip.imp = ei.imp + Randomizer::Instance()->randInt(10)-5;
				else
					equip.imp = 0;
				if (ei.iwatk > 0)
					equip.iwatk = ei.iwatk + Randomizer::Instance()->randInt(10)-5;
				else
					equip.iwatk = 0;
				if (ei.imatk > 0)
					equip.imatk = ei.imatk + Randomizer::Instance()->randInt(10)-5;
				else
					equip.imatk = 0;
				if (ei.iwdef > 0)
					equip.iwdef = ei.iwdef + Randomizer::Instance()->randInt(10)-5;
				else
					equip.iwdef = 0;
				if (ei.imdef > 0)
					equip.imdef = ei.imdef + Randomizer::Instance()->randInt(10)-5;
				else
					equip.imdef = 0;
				if (ei.iacc > 0)
					equip.iacc = ei.iacc + Randomizer::Instance()->randInt(2)-1;
				else
					equip.iacc = 0;
				if (ei.iavo > 0)
					equip.iavo = ei.iavo + Randomizer::Instance()->randInt(2)-1;
				else
					equip.iavo = 0;
				equip.ihand = ei.ihand;
				if (ei.ijump > 0)
					equip.ijump = ei.ijump + Randomizer::Instance()->randInt(2)-1;
				else
					equip.ijump = 0;
				if (ei.ispeed > 0)
					equip.ispeed = ei.ispeed + Randomizer::Instance()->randInt(2)-1;	
				else
					equip.ispeed = 0;
				drop = new Drop(mob->getMapID(), equip, pos, player->getPlayerid());
			}

			else {
				Item item;
				item.id = drops[k].id;
				item.amount = 1;
				item.inv = items[drops[k].id].type;
				drop = new Drop(mob->getMapID(), item, pos, player->getPlayerid());
			}

			if (drops[k].quest > 0) {
				drop->setPlayer(player->getPlayerid());
				drop->setQuest(drops[k].quest);
			}
			Dropped dropper;
			dropper.id = mob->getID();
			dropper.pos = mob->getPos();
			drop->setTime(100);
			drop->doDrop(dropper);
			d++;
		}
	}
	int nm = mesos[mob->getMobID()].min * ChannelServer::Instance()->getMesorate();
	int xm = mesos[mob->getMobID()].max * ChannelServer::Instance()->getMesorate();
	if (xm > 0 && nm > 0) {
		int mesos = Randomizer::Instance()->randInt(xm-nm)+nm;
		// For Meso up
		if (player->skills->getActiveSkillLevel(4111001) > 0) {
			mesos = (mesos*Skills::skills[4111001][player->skills->getActiveSkillLevel(4111001)].x)/100;
		}
		Dropped dropper;
		dropper.id = mob->getID();
		dropper.pos = mob->getPos();
		Pos pos;
		if (d%2) {
			pos.x = mob->getPos().x+25*((d+1)/2);
			pos.y = mob->getPos().y;
		}
		else {
			pos.x = mob->getPos().x-25*(d/2);
			pos.y = mob->getPos().y;
		}
		Drop *drop = new Drop(player->getMap(), mesos, pos, player->getPlayerid());
		drop->setTime(100);
		drop->doDrop(dropper);
	}
}

void Drops::lootItem(Player *player, ReadPacket *packet) {
	packet->skipBytes(9);
	int itemid = packet->getInt();
	Drop* drop = Maps::maps[player->getMap()]->getDrop(itemid);
	if (drop == 0) {
		DropsPacket::dontTake(player);
		return;
	}
	if (drop->getPos() - player->getPos() > 300) {
		if (player->addWarning()) return;
	}
	if (drop->isQuest()) {
		int request = 0;
		for (size_t i = 0; i < Quests::quests[drop->getQuest()].rewards.size(); i++) {
			if (Quests::quests[drop->getQuest()].rewards[i].id == drop->getObjectID()) {
				request = Quests::quests[drop->getQuest()].rewards[i].count;
			}
		}
		if (player->inv->getItemAmount(drop->getObjectID()) > request || !player->quests->isQuestActive(drop->getQuest())) {
			DropsPacket::takeNote(player, 0, false, 0);
			DropsPacket::dontTake(player);
			return;
		}
	}
	if (drop->isMesos()) {
		player->inv->setMesos(player->inv->getMesos() + drop->getObjectID(), true);
		DropsPacket::takeNote(player, drop->getObjectID(), true, 0);
	}
	else {
		if (drop->isEquip()) {
			Equip *equip = new Equip(drop->getEquip());
			equip->pos = Inventory::findSlot(player, equip->id, 1, 1);
			if (equip->pos == 0) {
				DropsPacket::takeNote(player, 0, 0, 0);
				DropsPacket::dontTake(player);
				return;
			}
			Inventory::addEquip(player, equip, true);
		}
		else {
			Item *item = new Item(drop->getItem());
			item->pos = Inventory::findSlot(player, item->id, item->inv, item->amount);
			if (item->pos == 0) {
				DropsPacket::takeNote(player, 0, 0, 0);
				DropsPacket::dontTake(player);
				return;
			}
			Inventory::addItem(player, item, true);
		}
		DropsPacket::takeNote(player, drop->getObjectID(), false, drop->getAmount());
	}
	Reactors::checkLoot(drop);
	drop->takeDrop(player);
}

void Drops::addDrop(int id, MobDropInfo drop) {
	dropsinfo[id].push_back(drop);
}
void Drops::addEquip(int id, EquipInfo equip) {
	equips[id] = equip;
}
void Drops::addItem(int id, ItemInfo item) {
	items[id] = item;
	if (ISSTAR(id))
		Shops::rechargables.push_back(id);
}
void Drops::addConsume(int id, ConsumeInfo cons) {
	consumes[id] = cons;
}
void Drops::addMesos(int id, Mesos meso) {
	mesos[id] = meso;
}

void Drops::dropMesos(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int amount = packet->getInt();
	if (amount < 10 || amount > 50000) {
		// hacking
		return;
	}
	player->inv->setMesos(player->inv->getMesos() - amount, true);
	Drop *drop = new Drop(player->getMap(), amount, player->getPos(), player->getPlayerid());
	drop->setTime(0);
	Dropped dropper;
	dropper.id = player->getPlayerid();
	dropper.pos = player->getPos();
	drop->doDrop(dropper);
}
