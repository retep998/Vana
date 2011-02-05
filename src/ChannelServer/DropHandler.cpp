/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "DropHandler.h"
#include "ChannelServer.h"
#include "Drop.h"
#include "DropDataProvider.h"
#include "DropsPacket.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "ItemDataProvider.h"
#include "MapDataProvider.h"
#include "Maps.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Pos.h"
#include "QuestDataProvider.h"
#include "Randomizer.h"
#include "ReactorHandler.h"
#include "SkillConstants.h"
#include "SkillDataProvider.h"
#include "Skills.h"
#include <algorithm>

void DropHandler::doDrops(int32_t playerid, int32_t mapid, int32_t droppingLevel, int32_t droppingId, const Pos &origin, bool explosive, bool ffa, int32_t taunt, bool isSteal) {
	GlobalDrops *gdrops = DropDataProvider::Instance()->getGlobalDrops();
	if (!DropDataProvider::Instance()->hasDrops(droppingId) && gdrops == nullptr) {
		return;
	}

	DropsInfo drops = DropDataProvider::Instance()->getDrops(droppingId); // Make a copy of the data so we can modify the object with global drops
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	int16_t d = 0;
	int32_t partyid = 0;
	Pos pos;

	if (player != nullptr) {
		if (Party *party = player->getParty()) {
			partyid = party->getId();
		}
	}
	if (droppingLevel != 0 && gdrops != nullptr) {
		// Check for global drops, add to the vector if needed
		DropInfo d;
		int8_t continent = MapDataProvider::Instance()->getContinent(mapid);
		for (GlobalDrops::iterator i = gdrops->begin(); i != gdrops->end(); i++) {
			if (droppingLevel >= i->minlevel && droppingLevel <= i->maxlevel) {
				if (i->continent == 0 || (continent == i->continent)) {
					d = DropInfo();
					d.chance = i->chance;
					d.ismesos = i->ismesos;
					d.itemid = i->itemid;
					d.minamount = i->minamount;
					d.maxamount = i->maxamount;
					d.questid = i->questid;
					drops.push_back(d);
				}
			}
		}
	}

	std::random_shuffle(drops.begin(), drops.end());
	int16_t mod = 25;
	for (DropsInfo::iterator i = drops.begin(); i != drops.end(); i++) {
		int16_t amount = static_cast<int16_t>(Randomizer::Instance()->randInt(i->maxamount, i->minamount));
		Drop *drop = nullptr;
		uint32_t chance = i->chance;

		if (isSteal) {
			chance = chance * 3 / 10;
		}
		else {
			chance = chance * taunt / 100;
			chance *= ChannelServer::Instance()->getDropRate();
		}

		if (Randomizer::Instance()->randInt(999999) < chance) {
			if (explosive) {
				mod = 35;
			}
			pos.x = origin.x + ((d % 2) ? (mod * (d + 1) / 2) : -(mod * (d / 2)));
			pos.y = origin.y;

			if (Maps::getMap(mapid)->getFhAtPosition(pos) == 0) {
//				pos = Maps::getMap(mapid)->findFloor(pos); // getFhAtPosition doesn't work correctly!
			}

			if (!i->ismesos) {
				int32_t itemid = i->itemid;
				int16_t questid = i->questid;

				if (questid > 0) {
					if (player == nullptr || !player->getQuests()->isQuestActive(questid)) {
						continue;
					}
					int16_t request = QuestDataProvider::Instance()->getItemRequest(questid, itemid);
					int16_t amount = player->getInventory()->getItemAmount(itemid);
					if (amount >= request) {
						continue;
					}
				}

				Item f = (GameLogicUtilities::isEquip(itemid) ? Item(itemid, true) : Item(itemid, amount));
				drop = new Drop(mapid, f, pos, playerid);

				if (questid > 0) {
					drop->setPlayerId(playerid);
					drop->setQuest(questid);
				}
			}
			else {
				int32_t mesos = amount;
				if (!isSteal) {
					mesos *= ChannelServer::Instance()->getMesoRate();
					if (player != nullptr && player->getActiveBuffs()->hasMesoUp()) {
						// Account for Meso Up
						mesos = (mesos * player->getActiveBuffs()->getActiveSkillInfo(Jobs::Hermit::MesoUp)->x) / 100;
					}
				}
				drop = new Drop(mapid, mesos, pos, playerid);
			}
		}

		if (drop != nullptr) {
			if (explosive) {
				drop->setType(Drop::Explosive);
			}
			else if (ffa) {
				drop->setType(Drop::FreeForAll);
			}
			else if (partyid > 0) {
				drop->setType(Drop::Party);
				drop->setOwner(partyid);
			}
			drop->setTime(100);
			drop->doDrop(origin);
			d++;
			ReactorHandler::checkDrop(player, drop);
		}
	}
}

void DropHandler::dropMesos(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int32_t amount = packet.get<int32_t>();
	if (amount < 10 || amount > 50000 || amount > player->getInventory()->getMesos()) {
		// Hacking
		return;
	}
	player->getInventory()->modifyMesos(-amount, true);
	Drop *drop = new Drop(player->getMap(), amount, player->getPos(), player->getId(), true);
	drop->setTime(0);
	drop->doDrop(player->getPos());
}

void DropHandler::petLoot(Player *player, PacketReader &packet) {
	int32_t petid = (int32_t)packet.get<int64_t>();

	lootItem(player, packet, petid);
}

void DropHandler::lootItem(Player *player, PacketReader &packet, int32_t petid) {
	packet.skipBytes(5);
	Pos playerPos = packet.getPos();
	int32_t dropid = packet.get<int32_t>();
	Drop *drop = Maps::getMap(player->getMap())->getDrop(dropid);

	if (drop == nullptr) {
		DropsPacket::dontTake(player);
		return;
	}
	else if (playerPos - player->getPos() > 100) {
		// Hacking
		return;
	}
	else if (drop->getPos() - player->getPos() > 300) {
		if (player->addWarning()) {
			return;
		}
	}

	if (drop->isQuest()) {
		if (!player->getQuests()->isQuestActive(drop->getQuest())) {
			DropsPacket::dropNotAvailableForPickup(player);
			DropsPacket::dontTake(player);
			return;
		}

		int16_t request = QuestDataProvider::Instance()->getItemRequest(drop->getQuest(), drop->getObjectId());
		if (player->getInventory()->getItemAmount(drop->getObjectId()) >= request) {
			DropsPacket::dropNotAvailableForPickup(player);
			DropsPacket::dontTake(player);
			return;
		}
	}
	if (drop->isMesos()) {
		int32_t playerrate = 100;
		int32_t mesos = drop->getObjectId();
		if (player->getParty() != nullptr && !drop->isPlayerDrop()) {
			// Player gets 100% unless partied and having others on the map, in which case it's 60%
			vector<Player *> members = player->getParty()->getPartyMembers(player->getMap());
			if (members.size() != 1) {
				playerrate = 60;
				mesos = mesos * playerrate / 100;

				if (player->getInventory()->modifyMesos(mesos, true)) {
					DropsPacket::pickupDrop(player, mesos, 0, true);
				}
				else {
					DropsPacket::dontTake(player);
					return;
				}

				playerrate = 40 / (members.size() - 1);
				mesos = drop->getObjectId() * playerrate / 100;
				Player *p = nullptr;

				for (uint8_t j = 0; j < members.size(); j++) {
					p = members[j];
					if (p != player) {
						if (p->getInventory()->modifyMesos(mesos, true)) {
							DropsPacket::pickupDrop(p, mesos, 0, true);
						}
						else {
							DropsPacket::dontTake(p);
						}
					}
				}
			}
		}
		if (playerrate == 100) {
			if (player->getInventory()->modifyMesos(mesos, true)) {
				DropsPacket::pickupDrop(player, drop->getObjectId(), 0, true);
			}
			else {
				DropsPacket::dontTake(player);
				return;
			}
		}
	}
	else {
		Item dropitem = drop->getItem();
		ConsumeInfo *cons = ItemDataProvider::Instance()->getConsumeInfo(dropitem.getId());
		if (cons != nullptr && cons->autoconsume) {
			if (GameLogicUtilities::isMonsterCard(drop->getObjectId())) {
				DropsPacket::pickupDropSpecial(player, drop->getObjectId());
				Inventory::useItem(player, dropitem.getId());
				DropsPacket::dontTake(player);
				drop->takeDrop(player, petid);
				return;
			}
			Inventory::useItem(player, dropitem.getId());
		}
		else {
			Item *item = new Item(dropitem);
			int16_t dropAmount = drop->getAmount();
			int16_t amount = Inventory::addItem(player, item, true);
			if (amount > 0) {
				if ((dropAmount - amount) > 0) {
					DropsPacket::pickupDrop(player, drop->getObjectId(), dropAmount - amount);
					drop->setItemAmount(amount);
				}
				DropsPacket::cantGetAnymoreItems(player);
				DropsPacket::dontTake(player);
				return;
			}
		}
		DropsPacket::pickupDrop(player, drop->getObjectId(), drop->getAmount());
	}
	ReactorHandler::checkLoot(drop);
	drop->takeDrop(player, petid);
}