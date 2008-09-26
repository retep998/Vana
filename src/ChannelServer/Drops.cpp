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
#include "Maps.h"
#include "Reactors.h"
#include "DropsPacket.h"
#include "Inventory.h"
#include "Quests.h"
#include "Shops.h"
#include "Randomizer.h"
#include "Pos.h"
#include "Skills.h"
#include "ReadPacket.h"
#include <unordered_map>

using std::tr1::unordered_map;

unordered_map<int32_t, DropsInfo> Drops::dropdata;

// Drop class
Drop::Drop (int32_t mapid, int32_t mesos, Pos pos, int32_t owner, bool playerdrop) : mapid(mapid), pos(pos), mesos(mesos), owner(owner), questid(0), dropped(0), playerid(0), playerdrop(playerdrop) {
	Maps::maps[mapid]->addDrop(this);
}

Drop::Drop (int32_t mapid, Item item, Pos pos, int32_t owner, bool playerdrop) : mapid(mapid), pos(pos), item(item), mesos(0), owner(owner), questid(0), dropped(0), playerid(0), playerdrop(playerdrop) {
	Maps::maps[mapid]->addDrop(this);
}

int32_t Drop::getObjectID() {
	if (mesos > 0)
		return mesos;
	else
		return item.id;
}

int16_t Drop::getAmount() {
	return item.amount;
}

void Drop::doDrop(Pos origin) {
	setDropped(clock());
	if (!isQuest())
		DropsPacket::showDrop(0, this, 1, true, origin);
	else {
		Player *player = Players::Instance()->getPlayer(playerid);
		if (!player) {
			return;
		}
		if (player->getMap() == this->mapid) {
			DropsPacket::showDrop(player, this, 1, true, origin);
		}
	}
}

void Drop::showDrop(Player *player) {
	if (isQuest() && player->getId() != playerid)
		return;
	DropsPacket::showDrop(player, this, 2, false, Pos());
}

void Drop::takeDrop(Player *player) {
	Maps::maps[mapid]->removeDrop(this->id);
	DropsPacket::takeDrop(player, this);
	delete this;
}

void Drop::removeDrop(bool showPacket) {
	Maps::maps[this->mapid]->removeDrop(this->id);
	if (showPacket)
		DropsPacket::removeDrop(this);
	delete this;
}

// Drops namespace
void Drops::addDropData(int32_t id, DropInfo drop) {
	dropdata[id].push_back(drop);
}

void Drops::doDrops(Player *player, int32_t droppingID, Pos origin) {
	DropsInfo drops = dropdata[droppingID];

	int16_t d = 0;
	for (size_t k = 0; k < drops.size(); k++) {
 		if (!drops[k].ismesos && Randomizer::Instance()->randInt(9999) < drops[k].chance * ChannelServer::Instance()->getDroprate()) {
			if (drops[k].quest > 0) {
				if (!player->getQuests()->isQuestActive(drops[k].quest))
					continue;
				int32_t request = 0;
				for (size_t i = 0; i < Quests::quests[drops[k].quest].rewards.size(); i++) {
					if (Quests::quests[drops[k].quest].rewards[i].id == drops[k].id) {
						request = Quests::quests[drops[k].quest].rewards[i].count;
					}
				}
				if (player->getInventory()->getItemAmount(drops[k].id) > request)
					continue;
			}

			Pos pos;
			if (d%2) {
				pos.x = origin.x+25*((d+1)/2);
				pos.y = origin.y;
			}
			else {
				pos.x = origin.x-25*(d/2);
				pos.y = origin.y;
			}

			Drop *drop = 0;

			if (ISEQUIP(drops[k].id))
				drop = new Drop(player->getMap(), Item(drops[k].id, true), pos, player->getId());
			else
				drop = new Drop(player->getMap(), Item(drops[k].id, (int16_t) 1), pos, player->getId());

			if (drops[k].quest > 0) {
				drop->setPlayer(player->getId());
				drop->setQuest(drops[k].quest);
			}
			drop->setTime(100);
			drop->doDrop(origin);
			d++;
		}

		else if (drops[k].ismesos) {
			int32_t nm = drops[k].minmesos * ChannelServer::Instance()->getMesorate();
			int32_t xm = drops[k].maxmesos * ChannelServer::Instance()->getMesorate();
			if (xm > 0 && nm > 0) {
				int32_t mesos = Randomizer::Instance()->randInt(xm - nm) + nm;
				// For Meso up
				if (player->getActiveBuffs()->getActiveSkillLevel(4111001) > 0) {
					mesos = (mesos * Skills::skills[4111001][player->getActiveBuffs()->getActiveSkillLevel(4111001)].x) / 100;
				}
				Pos pos;
				if ((d % 2) != 0) {
					pos.x = origin.x + 25 * ((d + 1) / 2);
					pos.y = origin.y;
				}
				else {
					pos.x = origin.x - 25 * (d / 2);
					pos.y = origin.y;
				}
				Drop *drop = new Drop(player->getMap(), mesos, pos, player->getId());
				drop->setTime(100);
				drop->doDrop(origin);
			}
		}
	}
}

void Drops::dropMesos(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int32_t amount = packet->getInt();
	if (amount < 10 || amount > 50000) {
		// hacking
		return;
	}
	player->getInventory()->modifyMesos(-amount, true);
	Drop *drop = new Drop(player->getMap(), amount, player->getPos(), player->getId(), true);
	drop->setTime(0);
	drop->doDrop(player->getPos());
}

void Drops::lootItem(Player *player, ReadPacket *packet) {
	packet->skipBytes(9);
	int32_t itemid = packet->getInt();
	Drop* drop = Maps::maps[player->getMap()]->getDrop(itemid);
	bool success = true;
	if (drop == 0) {
		DropsPacket::dontTake(player);
		return;
	}
	if (drop->getPos() - player->getPos() > 300) {
		if (player->addWarning())
			return;
	}
	if (drop->isQuest()) {
		int32_t request = 0;
		for (size_t i = 0; i < Quests::quests[drop->getQuest()].rewards.size(); i++) {
			if (Quests::quests[drop->getQuest()].rewards[i].id == drop->getObjectID()) {
				request = Quests::quests[drop->getQuest()].rewards[i].count;
			}
		}
		if (player->getInventory()->getItemAmount(drop->getObjectID()) > request || !player->getQuests()->isQuestActive(drop->getQuest())) {
			DropsPacket::takeNote(player, 0, false, 0);
			DropsPacket::dontTake(player);
			return;
		}
	}
	if (drop->isMesos()) {
		success = player->getInventory()->modifyMesos(drop->getObjectID(), true);
		if (success)
			DropsPacket::takeNote(player, drop->getObjectID(), true, 0);
	}
	else {
		Item *item = new Item(drop->getItem());
		int32_t dropAmount = drop->getAmount();
		int16_t amount = Inventory::addItem(player, item, true);
		if (amount > 0) {
			if (dropAmount - amount > 0) {
				DropsPacket::takeNote(player, drop->getObjectID(), false, (int16_t) dropAmount - amount);
				drop->setItemAmount(amount);
			}
			DropsPacket::takeNote(player, 0, 0, 0);
			DropsPacket::dontTake(player);
			return;
		}
		DropsPacket::takeNote(player, drop->getObjectID(), false, drop->getAmount());
	}
	Reactors::checkLoot(drop);
	if (success)
		drop->takeDrop(player);
}
