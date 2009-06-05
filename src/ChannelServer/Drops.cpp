/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "DropDataProvider.h"
#include "DropsPacket.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "ItemDataProvider.h"
#include "Maps.h"
#include "PacketReader.h"
#include "Pets.h"
#include "Pos.h"
#include "Quests.h"
#include "Randomizer.h"
#include "Reactors.h"
#include "Skills.h"
#include "TimeUtilities.h"
#include <limits>

// Drop class
Drop::Drop(int32_t mapid, int32_t mesos, Pos pos, int32_t owner, bool playerdrop) :
questid(0),
owner(owner),
mapid(mapid),
mesos(mesos),
// Initializing dropped time to max-value to prevent timers from
// deleting the drop in case doDrop did not get called right away
dropped(std::numeric_limits<int32_t>::max()),
playerid(0),
playerdrop(playerdrop),
tradeable(true),
pos(pos)
{
	Maps::getMap(mapid)->addDrop(this);
}

Drop::Drop(int32_t mapid, Item item, Pos pos, int32_t owner, bool playerdrop) :
questid(0),
owner(owner),
mapid(mapid),
mesos(0),
dropped(std::numeric_limits<int32_t>::max()),
playerid(0), playerdrop(playerdrop),
tradeable(true),
pos(pos),
item(item)
{
	Maps::getMap(mapid)->addDrop(this);
}

int32_t Drop::getObjectId() {
	return (mesos > 0 ? mesos : item.id);
}

int16_t Drop::getAmount() {
	return item.amount;
}

void Drop::doDrop(Pos origin) {
	setDropped(TimeUtilities::getTickCount());
	if (!isQuest()) {
		if (!isTradeable()) {
			DropsPacket::showDrop(0, this, 3, false, origin);
			this->removeDrop(false);
		}
		else
			DropsPacket::showDrop(0, this, 1, true, origin);
	}
	else if (Player *player = Players::Instance()->getPlayer(playerid)) {
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

void Drop::takeDrop(Player *player, int32_t petid) {
	Maps::getMap(mapid)->removeDrop(this->id);
	if (petid == 0)
		DropsPacket::takeDrop(player, this);
	else
		DropsPacket::takeDrop(player, this, player->getPets()->getPet(petid)->getIndex());
	delete this;
}

void Drop::removeDrop(bool showPacket) {
	Maps::getMap(this->mapid)->removeDrop(this->id);
	if (showPacket)
		DropsPacket::removeDrop(this);
	delete this;
}

// Drops namespace
void Drops::doDrops(int32_t playerid, int32_t mapid, int32_t droppingId, Pos origin) {
	DropsInfo drops = DropDataProvider::Instance()->getDrops(droppingId);
	Player *player = Players::Instance()->getPlayer(playerid);
	int16_t d = 0;
	Pos pos;

	for (size_t i = 0; i < drops.size(); i++) {
		int16_t amount = static_cast<int16_t>(Randomizer::Instance()->randInt(drops[i].maxamount - drops[i].minamount) + drops[i].minamount);
		Drop *drop = 0;

 		if (Randomizer::Instance()->randInt(99999) < drops[i].chance * ChannelServer::Instance()->getDroprate()) {
			pos.x = origin.x + ((d % 2) ? (25 * (d + 1) / 2) : -(25 * (d / 2)));
			pos.y = origin.y;

			if (!drops[i].ismesos) {
				int32_t itemid = drops[i].itemid;
				int16_t questid = drops[i].questid;

				if (questid > 0) {
					if (player == 0 || !player->getQuests()->isQuestActive(questid))
						continue;

					int32_t request = 0;
					for (size_t r = 0; r < Quests::quests[questid].rewards.size(); r++) {
						if (Quests::quests[questid].rewards[r].id == itemid) {
							request = Quests::quests[questid].rewards[r].count;
						}
					}
					if (player->getInventory()->getItemAmount(itemid) > request)
						continue;
				}

				if (GameLogicUtilities::isEquip(itemid))
					drop = new Drop(mapid, Item(itemid, true), pos, playerid);
				else
					drop = new Drop(mapid, Item(itemid, amount), pos, playerid);

				if (questid > 0) {
					drop->setPlayer(playerid);
					drop->setQuest(questid);
				}
			}
			else {
				int32_t mesos = (amount * ChannelServer::Instance()->getMesorate());
				if (player != 0 && player->getActiveBuffs()->hasMesoUp()) { // Account for Meso Up
					mesos = (mesos * Skills::skills[Jobs::Hermit::MesoUp][player->getActiveBuffs()->getActiveSkillLevel(Jobs::Hermit::MesoUp)].x) / 100;
				}
				drop = new Drop(mapid, mesos, pos, playerid);
			}
		}

		if (drop != 0) {
			drop->setTime(100);
			drop->doDrop(origin);
			d++;
		}
	}
}

void Drops::dropMesos(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int32_t amount = packet.get<int32_t>();
	if (amount < 10 || amount > 50000 || amount > player->getInventory()->getMesos()) {
		// hacking
		return;
	}
	player->getInventory()->modifyMesos(-amount, true);
	Drop *drop = new Drop(player->getMap(), amount, player->getPos(), player->getId(), true);
	drop->setTime(0);
	drop->doDrop(player->getPos());
}

void Drops::playerLoot(Player *player, PacketReader &packet) {
	packet.skipBytes(9);
	int32_t dropid = packet.get<int32_t>();

	lootItem(player, dropid);
}

void Drops::petLoot(Player *player, PacketReader &packet) {
	int32_t petid = packet.get<int32_t>();
	packet.skipBytes(13);
	int32_t dropid = packet.get<int32_t>();

	lootItem(player, dropid, petid);
}

void Drops::lootItem(Player *player, int32_t dropid, int32_t petid) {
	Drop *drop = Maps::getMap(player->getMap())->getDrop(dropid);

	if (drop == 0) {
		DropsPacket::dontTake(player);
		return;
	}
	else if (drop->getPos() - player->getPos() > 300) {
		if (player->addWarning())
			return;
	}

	if (drop->isQuest()) {
		int32_t request = 0;
		for (size_t i = 0; i < Quests::quests[drop->getQuest()].rewards.size(); i++) {
			if (Quests::quests[drop->getQuest()].rewards[i].id == drop->getObjectId()) {
				request = Quests::quests[drop->getQuest()].rewards[i].count;
			}
		}
		if (player->getInventory()->getItemAmount(drop->getObjectId()) > request || !player->getQuests()->isQuestActive(drop->getQuest())) {
			DropsPacket::takeNote(player, 0, false, 0);
			DropsPacket::dontTake(player);
			return;
		}
	}
	if (drop->isMesos()) {
		if (player->getInventory()->modifyMesos(drop->getObjectId(), true))
			DropsPacket::takeNote(player, drop->getObjectId(), true, 0);
		else
			return;
	}
	else {
		Item dropitem = drop->getItem();
		if (GameLogicUtilities::isEquip(dropitem.id) || !ItemDataProvider::Instance()->getItemInfo(dropitem.id).cons.autoconsume) {
			Item *item = new Item(dropitem);
			int16_t dropAmount = drop->getAmount();
			int16_t amount = Inventory::addItem(player, item, true);
			if (amount > 0) {
				if (dropAmount - amount > 0) {
					DropsPacket::takeNote(player, drop->getObjectId(), false, dropAmount - amount);
					drop->setItemAmount(amount);
				}
				DropsPacket::takeNote(player, 0, 0, 0);
				DropsPacket::dontTake(player);
				return;
			}
		}
		else {
			Inventory::useItem(player, dropitem.id);
		}
		DropsPacket::takeNote(player, drop->getObjectId(), false, drop->getAmount());
	}
	Reactors::checkLoot(drop);
	drop->takeDrop(player, petid);
}
