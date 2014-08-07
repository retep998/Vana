/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "DropHandler.hpp"
#include "ChannelServer.hpp"
#include "Drop.hpp"
#include "DropDataProvider.hpp"
#include "DropsPacket.hpp"
#include "GameLogicUtilities.hpp"
#include "Inventory.hpp"
#include "ItemDataProvider.hpp"
#include "MapDataProvider.hpp"
#include "Maps.hpp"
#include "PacketReader.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "Point.hpp"
#include "QuestDataProvider.hpp"
#include "Randomizer.hpp"
#include "ReactorHandler.hpp"
#include "SkillConstants.hpp"
#include "SkillDataProvider.hpp"
#include "Skills.hpp"
#include <algorithm>

auto DropHandler::doDrops(player_id_t playerId, map_id_t mapId, int32_t droppingLevel, int32_t droppingId, const Point &origin, bool explosive, bool ffa, int32_t taunt, bool isSteal) -> void {
	auto &globalDrops = ChannelServer::getInstance().getDropDataProvider().getGlobalDrops();
	if (!ChannelServer::getInstance().getDropDataProvider().hasDrops(droppingId) && globalDrops.size() == 0) {
		return;
	}

	// Make a copy of the data so we can modify the object with global drops
	auto drops = ChannelServer::getInstance().getDropDataProvider().getDrops(droppingId);

	Player *player = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerId);
	coord_t dropPosCounter = 0;
	party_id_t partyId = 0;
	Point pos;

	if (player != nullptr) {
		if (Party *party = player->getParty()) {
			partyId = party->getId();
		}
	}
	if (droppingLevel != 0 && globalDrops.size() != 0) {
		DropInfo d;
		int8_t continent = ChannelServer::getInstance().getMapDataProvider().getContinent(mapId);

		for (const auto &globalDrop : globalDrops) {
			if (droppingLevel >= globalDrop.minLevel && droppingLevel <= globalDrop.maxLevel) {
				if (globalDrop.continent == 0 || (continent == globalDrop.continent)) {
					d = DropInfo();
					d.chance = globalDrop.chance;
					d.isMesos = globalDrop.isMesos;
					d.itemId = globalDrop.itemId;
					d.minAmount = globalDrop.minAmount;
					d.maxAmount = globalDrop.maxAmount;
					d.questId = globalDrop.questId;
					drops.push_back(d);
				}
			}
		}
	}

	Randomizer::shuffle(drops);

	coord_t mod = explosive ? 35 : 25;
	auto &config = ChannelServer::getInstance().getConfig();

	for (const auto &dropInfo : drops) {
		slot_qty_t amount = static_cast<slot_qty_t>(Randomizer::rand<int32_t>(dropInfo.maxAmount, dropInfo.minAmount));
		Drop *drop = nullptr;
		uint32_t chance = dropInfo.chance;

		if (isSteal) {
			chance = chance * 3 / 10;
		}
		else {
			chance = chance * taunt / 100;
			chance *= config.rates.dropRate;
		}

		if (Randomizer::rand<uint32_t>(999999) < chance) {
			pos.x = origin.x + ((dropPosCounter % 2) ?
				(mod * (dropPosCounter + 1) / 2) :
				-(mod * (dropPosCounter / 2)));
			pos.y = origin.y;

			/*
			// getFootholdAtPosition doesn't work correctly
			if (Maps::getMap(mapId)->getFootholdAtPosition(pos) == 0) {
				pos = Maps::getMap(mapId)->findFloor(pos);
			}
			*/

			if (!dropInfo.isMesos) {
				item_id_t itemId = dropInfo.itemId;
				quest_id_t questId = dropInfo.questId;

				if (questId > 0) {
					if (player == nullptr || player->getQuests()->itemDropAllowed(itemId, questId) == AllowQuestItemResult::Disallow) {
						continue;
					}
				}

				Item f = GameLogicUtilities::isEquip(itemId) ?
					Item(ChannelServer::getInstance().getEquipDataProvider(),
						itemId,
						Items::StatVariance::Normal,
						player != nullptr && player->hasGmBenefits()) :
					Item(itemId, amount);

				drop = new Drop(mapId, f, pos, playerId);

				if (questId > 0) {
					drop->setQuest(questId);
				}
			}
			else {
				mesos_t mesos = amount;
				if (!isSteal) {
					mesos *= config.rates.mobMesoRate;
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
			dropPosCounter++;
			ReactorHandler::checkDrop(player, drop);
		}
	}
}

auto DropHandler::dropMesos(Player *player, PacketReader &reader) -> void {
	tick_count_t ticks = reader.get<tick_count_t>();
	int32_t amount = reader.get<int32_t>();
	if (amount < 10 || amount > 50000 || amount > player->getInventory()->getMesos()) {
		// Hacking
		return;
	}
	player->getInventory()->modifyMesos(-amount, true);
	Drop *drop = new Drop(player->getMapId(), amount, player->getPos(), player->getId(), true);
	drop->setTime(0);
	drop->doDrop(player->getPos());
}

auto DropHandler::petLoot(Player *player, PacketReader &reader) -> void {
	pet_id_t petId = reader.get<pet_id_t>();
	lootItem(player, reader, petId);
}

auto DropHandler::lootItem(Player *player, PacketReader &reader, pet_id_t petId) -> void {
	reader.skipBytes(5);
	Point playerPos = reader.get<Point>();
	map_object_t dropId = reader.get<map_object_t>();
	Drop *drop = player->getMap()->getDrop(dropId);

	if (drop == nullptr) {
		player->send(DropsPacket::dontTake());
		return;
	}
	else if (drop->getPos() - player->getPos() > 300) {
		// Hacking
		return;
	}

	if (drop->isQuest()) {
		if (player->getQuests()->itemDropAllowed(drop->getObjectId(), drop->getQuest()) == AllowQuestItemResult::Disallow) {
			player->send(DropsPacket::dropNotAvailableForPickup());
			player->send(DropsPacket::dontTake());
			return;
		}
	}
	if (drop->isMesos()) {
		int32_t playerRate = 100;
		mesos_t rawMesos = drop->getObjectId();
		auto giveMesos = [](Player *p, mesos_t mesos) -> Result {
			if (p->getInventory()->modifyMesos(mesos, true)) {
				p->send(DropsPacket::pickupDrop(mesos, 0, true));
			}
			else {
				p->send(DropsPacket::dontTake());
				return Result::Failure;
			}
			return Result::Successful;
		};

		if (player->getParty() != nullptr && !drop->isPlayerDrop()) {
			// Player gets 100% unless partied and having others on the map, in which case it's 60%
			vector_t<Player *> members = player->getParty()->getPartyMembers(player->getMapId());
			if (members.size() != 1) {
				playerRate = 60;
				mesos_t mesos = rawMesos * playerRate / 100;

				if (giveMesos(player, mesos) == Result::Failure) {
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
			if (giveMesos(player, rawMesos) == Result::Failure) {
				return;
			}
		}
	}
	else {
		Item dropItem = drop->getItem();
		auto cons = ChannelServer::getInstance().getItemDataProvider().getConsumeInfo(dropItem.getId());
		if (cons != nullptr && cons->autoConsume) {
			if (GameLogicUtilities::isMonsterCard(drop->getObjectId())) {
				player->send(DropsPacket::pickupDropSpecial(drop->getObjectId()));
				Inventory::useItem(player, dropItem.getId());
				player->send(DropsPacket::dontTake());
				drop->takeDrop(player, petId);
				return;
			}
			Inventory::useItem(player, dropItem.getId());
		}
		else {
			Item *item = new Item(dropItem);
			slot_qty_t dropAmount = drop->getAmount();
			slot_qty_t amount = Inventory::addItem(player, item, true);
			if (amount > 0) {
				if ((dropAmount - amount) > 0) {
					player->send(DropsPacket::pickupDrop(drop->getObjectId(), dropAmount - amount));
					drop->setItemAmount(amount);
				}
				player->send(DropsPacket::cantGetAnymoreItems());
				player->send(DropsPacket::dontTake());
				return;
			}
		}
		// TODO FIXME Bug here? drop->getObjectId is going to be either a meso count or item identifier
		// pickupDrop packet calls for map_object_t and it's unclear which is correct and which isn't
		player->send(DropsPacket::pickupDrop(drop->getObjectId(), drop->getAmount()));
	}
	ReactorHandler::checkLoot(drop);
	drop->takeDrop(player, petId);
}