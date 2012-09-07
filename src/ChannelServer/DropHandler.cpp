/*
Copyright (C) 2008-2012 Vana Development Team

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

void DropHandler::doDrops(int32_t playerId, int32_t mapId, int32_t droppingLevel, int32_t droppingId, const Pos &origin, bool explosive, bool ffa, int32_t taunt, bool isSteal) {
	GlobalDrops *gDrops = DropDataProvider::Instance()->getGlobalDrops();
	if (!DropDataProvider::Instance()->hasDrops(droppingId) && gDrops == nullptr) {
		return;
	}
	DropsInfo drops = DropDataProvider::Instance()->getDrops(droppingId); // Make a copy of the data so we can modify the object with global drops
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerId);
	int16_t d = 0;
	int32_t partyId = 0;
	Pos pos;

	if (player != nullptr) {
		if (Party *party = player->getParty()) {
			partyId = party->getId();
		}
	}
	if (droppingLevel != 0 && gDrops != nullptr) {
		// Check for global drops, add to the vector if needed
		DropInfo d;
		int8_t continent = MapDataProvider::Instance()->getContinent(mapId);
		for (GlobalDrops::iterator i = gDrops->begin(); i != gDrops->end(); i++) {
			if (droppingLevel >= i->minLevel && droppingLevel <= i->maxLevel) {
				if (i->continent == 0 || (continent == i->continent)) {
					d = DropInfo();
					d.chance = i->chance;
					d.isMesos = i->isMesos;
					d.itemId = i->itemId;
					d.minAmount = i->minAmount;
					d.maxAmount = i->maxAmount;
					d.questId = i->questId;
					drops.push_back(d);
				}
			}
		}
	}
	std::random_shuffle(drops.begin(), drops.end());
	int16_t mod = 25;
	for (DropsInfo::iterator i = drops.begin(); i != drops.end(); ++i) {
		int16_t amount = static_cast<int16_t>(Randomizer::Instance()->randInt(i->maxAmount, i->minAmount));
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

			if (Maps::getMap(mapId)->getFhAtPosition(pos) == 0) {
				// pos = Maps::getMap(mapId)->findFloor(pos); // getFhAtPosition doesn't work correctly!
			}

			if (!i->isMesos) {
				int32_t itemId = i->itemId;
				int16_t questId = i->questId;

				if (questId > 0) {
					if (player == nullptr || !player->getQuests()->isQuestActive(questId)) {
						continue;
					}
					int16_t request = QuestDataProvider::Instance()->getItemRequest(questId, itemId);
					int16_t amount = player->getInventory()->getItemAmount(itemId);
					if (amount >= request) {
						continue;
					}
				}

				Item f = (GameLogicUtilities::isEquip(itemId) ? Item(itemId, true) : Item(itemId, amount));
				drop = new Drop(mapId, f, pos, playerId);

				if (questId > 0) {
					drop->setPlayerId(playerId);
					drop->setQuest(questId);
				}
			}
			else {
				int32_t mesos = amount;
				if (!isSteal) {
					mesos *= ChannelServer::Instance()->getMobMesoRate();
					if (player != nullptr && player->getActiveBuffs()->hasMesoUp()) {
						// Account for Meso Up
						mesos = (mesos * player->getActiveBuffs()->getActiveSkillInfo(Skills::Hermit::MesoUp)->x) / 100;
					}
				}
				drop = new Drop(mapId, mesos, pos, playerId);
			}
		}

		if (drop != nullptr) {
			if (explosive) {
				drop->setType(Drop::Explosive);
			}
			else if (ffa) {
				drop->setType(Drop::FreeForAll);
			}
			else if (partyId > 0) {
				drop->setType(Drop::Party);
				drop->setOwner(partyId);
			}
			drop->setTime(100);
			drop->doDrop(origin);
			d++;
			ReactorHandler::checkDrop(player, drop);
		}
	}
}

void DropHandler::dropMesos(Player *player, PacketReader &packet) {
	uint32_t ticks = packet.get<uint32_t>();
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
	int64_t petId = packet.get<int64_t>();
	lootItem(player, packet, petId);
}

void DropHandler::lootItem(Player *player, PacketReader &packet, int64_t petId) {
	packet.skipBytes(5);
	const Pos &playerPos = packet.getPos();
	int32_t dropId = packet.get<int32_t>();
	Drop *drop = Maps::getMap(player->getMap())->getDrop(dropId);

	if (drop == nullptr) {
		DropsPacket::dontTake(player);
		return;
	}
	else if (drop->getPos() - player->getPos() > 300) {
		// Hacking
		return;
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
		int32_t playerRate = 100;
		int32_t rawMesos = drop->getObjectId();
		auto giveMesos = [](Player *p, int32_t mesos) -> bool {
			if (p->getInventory()->modifyMesos(mesos, true)) {
				DropsPacket::pickupDrop(p, mesos, 0, true);
			}
			else {
				DropsPacket::dontTake(p);
				return false;
			}
			return true;
		};

		if (player->getParty() != nullptr && !drop->isplayerDrop()) {
			// Player gets 100% unless partied and having others on the map, in which case it's 60%
			vector<Player *> members = player->getParty()->getPartyMembers(player->getMap());
			if (members.size() != 1) {
				playerRate = 60;
				int32_t mesos = rawMesos * playerRate / 100;

				if (!giveMesos(player, mesos)) {
					// Can't pick up the mesos
					return;
				}

				playerRate = 40 / (members.size() - 1);
				mesos = rawMesos * playerRate / 100;
				Player *p = nullptr;

				for (uint8_t j = 0; j < members.size(); ++j) {
					p = members[j];
					if (p != player) {
						giveMesos(p, mesos);
					}
				}
			}
		}
		if (playerRate == 100) {
			if (!giveMesos(player, rawMesos)) {
				return;
			}
		}
	}
	else {
		Item dropItem = drop->getItem();
		ConsumeInfo *cons = ItemDataProvider::Instance()->getConsumeInfo(dropItem.getId());
		if (cons != nullptr && cons->autoConsume) {
			if (GameLogicUtilities::isMonsterCard(drop->getObjectId())) {
				DropsPacket::pickupDropSpecial(player, drop->getObjectId());
				Inventory::useItem(player, dropItem.getId());
				DropsPacket::dontTake(player);
				drop->takeDrop(player, petId);
				return;
			}
			Inventory::useItem(player, dropItem.getId());
		}
		else {
			Item *item = new Item(dropItem);
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
	drop->takeDrop(player, petId);
}