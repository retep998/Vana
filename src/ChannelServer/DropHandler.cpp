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
#include "DropHandler.h"
#include "ChannelServer.h"
#include "Drop.h"
#include "DropDataProvider.h"
#include "DropsPacket.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "ItemDataProvider.h"
#include "Maps.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "Players.h"
#include "Pos.h"
#include "QuestDataProvider.h"
#include "Randomizer.h"
#include "Reactors.h"
#include "SkillDataProvider.h"
#include "Skills.h"
#include <algorithm>

void DropHandler::doDrops(int32_t playerid, int32_t mapid, int32_t droppingLevel, int32_t droppingId, const Pos &origin, bool explosive, bool ffa, int32_t taunt, bool isSteal) {
	GlobalDrops *gdrops = DropDataProvider::Instance()->getGlobalDrops();
	if (!DropDataProvider::Instance()->hasDrops(droppingId) && gdrops == 0) {
		return;
	}
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
	if (droppingLevel != 0 && gdrops != 0) { // Check for global drops, add to the vector if needed
		DropInfo d;
		for (GlobalDrops::iterator i = gdrops->begin(); i != gdrops->end(); i++) {
			if (droppingLevel >= i->minlevel && droppingLevel <= i->maxlevel) {
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
	std::random_shuffle(drops.begin(), drops.end());
	for (DropsInfo::iterator i = drops.begin(); i != drops.end(); i++) {
		int16_t amount = static_cast<int16_t>(Randomizer::Instance()->randInt(i->maxamount - i->minamount) + i->minamount);
		Drop *drop = 0;
		uint32_t chance = i->chance;
		if (isSteal) {
			chance = chance * 3 / 10;
		}
		else {
			chance = chance * taunt / 100;
			chance *= ChannelServer::Instance()->getDroprate();
		}
		if (Randomizer::Instance()->randInt(999999) < chance) {
			if (explosive) {
				pos.x = origin.x + ((d % 2) ? (35 * (d + 1) / 2) : -(35 * (d / 2)));
			}
			else {
				pos.x = origin.x + ((d % 2) ? (25 * (d + 1) / 2) : -(25 * (d / 2)));
			}
			pos.y = origin.y;

//			if (Maps::getMap(mapid)->getFhAtPosition(pos) == 0) {
//				Need something to keep drops inside the map here		
//			}

			if (!i->ismesos) {
				int32_t itemid = i->itemid;
				int16_t questid = i->questid;

				if (questid > 0) {
					if (player == 0 || !player->getQuests()->isQuestActive(questid))
						continue;

					int16_t request = QuestDataProvider::Instance()->getItemRequest(questid, itemid);
					if (player->getInventory()->getItemAmount(itemid) >= request)
						continue;
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
					mesos *= ChannelServer::Instance()->getMesorate();
					if (player != 0 && player->getActiveBuffs()->hasMesoUp()) { // Account for Meso Up
						mesos = (mesos * SkillDataProvider::Instance()->getSkill(Jobs::Hermit::MesoUp, player->getActiveBuffs()->getActiveSkillLevel(Jobs::Hermit::MesoUp))->x) / 100;
					}
				}
				drop = new Drop(mapid, mesos, pos, playerid);
			}
		}

		if (drop != 0) {
			if (explosive) {
				drop->setType(3);
			}
			else if (ffa) {
				drop->setType(2);
			}
			else if (partyid > 0) {
				drop->setType(1);
				drop->setOwner(partyid);
			}
			drop->setTime(100);
			drop->doDrop(origin);
			d++;
			Reactors::checkDrop(player, drop);
		}
	}
}

void DropHandler::dropMesos(Player *player, PacketReader &packet) {
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

void DropHandler::playerLoot(Player *player, PacketReader &packet) {
	packet.skipBytes(9);
	int32_t dropid = packet.get<int32_t>();

	lootItem(player, dropid);
}

void DropHandler::petLoot(Player *player, PacketReader &packet) {
	int32_t petid = packet.get<int32_t>();
	packet.skipBytes(13);
	int32_t dropid = packet.get<int32_t>();

	lootItem(player, dropid, petid);
}

void DropHandler::lootItem(Player *player, int32_t dropid, int32_t petid) {
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

		int16_t request = QuestDataProvider::Instance()->getItemRequest(drop->getQuest(), drop->getObjectId());
		if (player->getInventory()->getItemAmount(drop->getObjectId()) >= request) {
			DropsPacket::takeNote(player, 0, false, 0);
			DropsPacket::dontTake(player);
			return;
		}
	}
	if (drop->isMesos()) {
		int32_t playerrate = 100;
		int32_t mesos = drop->getObjectId();
		if (player->getParty() != 0 && !drop->isPlayerDrop()) {
			// Player gets 100% unless partied and having others on the map, in which case it's 60%
			vector<Player *> members = player->getParty()->getPartyMembers(player->getMap());
			if (members.size() != 1) {
				playerrate = 60;
				mesos = mesos * playerrate / 100;

				if (player->getInventory()->modifyMesos(mesos, true)) {
					DropsPacket::takeNote(player, mesos, true, 0);
				}
				else {
					DropsPacket::dontTake(player);
					return;
				}

				playerrate = 40 / (members.size() - 1);
				mesos = drop->getObjectId() * playerrate / 100;
				Player *p = 0;

				for (uint8_t j = 0; j < members.size(); j++) {
					p = members[j];
					if (p != player) {
						if (p->getInventory()->modifyMesos(mesos, true)) {
							DropsPacket::takeNote(p, mesos, true, 0);
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
				DropsPacket::takeNote(player, drop->getObjectId(), true, 0);
			}
			else {
				DropsPacket::dontTake(player);
				return;
			}
		}
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