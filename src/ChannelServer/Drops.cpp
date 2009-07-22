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
#include "Party.h"
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
partydrop(false),
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
playerid(0),
playerdrop(playerdrop),
tradeable(true),
partydrop(false),
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
void Drops::doDrops(int32_t playerid, int32_t mapid, int32_t droppingLevel, int32_t droppingId, Pos origin, bool explosive, bool ffa, int32_t taunt, bool isSteal) {
	DropsInfo drops = DropDataProvider::Instance()->getDrops(droppingId);
	Player *player = Players::Instance()->getPlayer(playerid);
	int16_t d = 0;
	int32_t partyid = 0;
	Pos pos;

	if (player != 0) {
		if (Party *party = player->getParty()) {
			partyid = party->getId();
		}
	}
	if (droppingLevel != 0) { // Check for global drops, add to the vector if needed
		GlobalDrops gdrops = DropDataProvider::Instance()->getGlobalDrops();
		DropInfo d;
		for (size_t i = 0; i < gdrops.size(); i++) {
			if (droppingLevel >= gdrops[i].minamount && droppingLevel <= gdrops[i].maxamount) {
				d = DropInfo();
				d.chance = gdrops[i].chance;
				d.ismesos = gdrops[i].ismesos;
				d.itemid = gdrops[i].itemid;
				d.minamount = gdrops[i].minamount;
				d.maxamount = gdrops[i].maxamount;
				d.questid = gdrops[i].questid;
				drops.push_back(d);
			}
		}
	}
	for (size_t i = 0; i < drops.size(); i++) {
		int16_t amount = static_cast<int16_t>(Randomizer::Instance()->randInt(drops[i].maxamount - drops[i].minamount) + drops[i].minamount);
		Drop *drop = 0;
		uint32_t chance = drops[i].chance;
		if (isSteal) {
			chance = chance * 3 / 10;
		}
		else {
			chance = chance * taunt / 100;
			chance *= ChannelServer::Instance()->getDroprate();
		}
		if (Randomizer::Instance()->randInt(999999) < chance) {
			pos.x = origin.x + ((d % 2) ? (25 * (d + 1) / 2) : -(25 * (d / 2)));
			pos.y = origin.y;

			if (!drops[i].ismesos) {
				int32_t itemid = drops[i].itemid;
				int16_t questid = drops[i].questid;

				if (questid > 0) {
					if (player == 0 || !player->getQuests()->isQuestActive(questid))
						continue;

					int16_t request = Quests::quests[questid].getRequest(QuestRequestTypes::Item)[itemid];
					if (player->getInventory()->getItemAmount(itemid) >= request)
						continue;
				}

				if (GameLogicUtilities::isEquip(itemid))
					drop = new Drop(mapid, Item(itemid, true), pos, (partyid > 0 ? partyid : playerid));
				else
					drop = new Drop(mapid, Item(itemid, amount), pos, (partyid > 0 ? partyid : playerid));

				if (questid > 0) {
					drop->setPlayer(playerid);
					drop->setQuest(questid);
				}
			}
			else {
				int32_t mesos = amount;
				if (!isSteal) {
					mesos *= ChannelServer::Instance()->getMesorate();
					if (player != 0 && player->getActiveBuffs()->hasMesoUp()) { // Account for Meso Up
						mesos = (mesos * Skills::skills[Jobs::Hermit::MesoUp][player->getActiveBuffs()->getActiveSkillLevel(Jobs::Hermit::MesoUp)].x) / 100;
					}
				}
				drop = new Drop(mapid, mesos, pos, (partyid > 0 ? partyid : playerid));
			}
		}

		if (drop != 0) {
			if (partyid > 0) {
				drop->setPartyDrop(true);
			}
			drop->setTime(100);
			drop->doDrop(origin);
			d++;
			Reactors::checkDrop(player, drop);
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
		if (!player->getQuests()->isQuestActive(drop->getQuest())) {
			DropsPacket::takeNote(player, 0, false, 0);
			DropsPacket::dontTake(player);
			return;
		}

		int16_t request = Quests::quests[drop->getQuest()].getRequest(QuestRequestTypes::Item)[drop->getObjectId()];
		if (player->getInventory()->getItemAmount(drop->getObjectId()) >= request) {
			DropsPacket::takeNote(player, 0, false, 0);
			DropsPacket::dontTake(player);
			return;
		}
	}
	if (drop->isMesos()) {
		int32_t playerrate = 100;
		if (player->getParty() != 0 && !drop->isplayerDrop()) {
			// Player gets 100% unless partied and having others on the map, in which case it's 60%
			playerrate = 60;
			vector<Player *> members;
			for (int8_t i = 0; i < player->getParty()->getMembersCount(); i++) {
				if (Player *test = player->getParty()->getMemberByIndex(i)) {
					if (test != player && test->getMap() == player->getMap()) {
						members.push_back(test);
					}
				}
			}
			if (members.size() == 0) {
				playerrate = 100;
			}
			else {
				int32_t memberrate = 40 / members.size();
				for (uint8_t j = 0; j < members.size(); j++) {
					if (members[j]->getInventory()->modifyMesos(drop->getObjectId() * memberrate / 100, true)) {
						DropsPacket::takeNote(members[j], drop->getObjectId(), true, 0);
					}
				}
			}
		}
		if (player->getInventory()->modifyMesos(drop->getObjectId() * playerrate / 100, true))
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
			if (GameLogicUtilities::isMonsterCard(drop->getObjectId())) {
				if (player->getMonsterBook()->isFull(drop->getObjectId())) {
					drop->takeDrop(player, petid);
				}
				else {
					drop->removeDrop();
				}
				Inventory::useItem(player, dropitem.id);
				DropsPacket::dontTake(player);
				return;
			}
			Inventory::useItem(player, dropitem.id);
		}
		DropsPacket::takeNote(player, drop->getObjectId(), false, drop->getAmount());
	}
	Reactors::checkLoot(drop);
	drop->takeDrop(player, petid);
}