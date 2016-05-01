/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "Common/DropDataProvider.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/Item.hpp"
#include "Common/ItemDataProvider.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Point.hpp"
#include "Common/QuestDataProvider.hpp"
#include "Common/Randomizer.hpp"
#include "Common/SkillConstants.hpp"
#include "Common/SkillDataProvider.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Drop.hpp"
#include "ChannelServer/DropsPacket.hpp"
#include "ChannelServer/Inventory.hpp"
#include "ChannelServer/MapDataProvider.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Party.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/ReactorHandler.hpp"
#include "ChannelServer/Skills.hpp"
#include <algorithm>

namespace Vana {
namespace ChannelServer {

auto DropHandler::doDrops(player_id_t playerId, map_id_t mapId, int32_t droppingLevel, int32_t droppingId, const Point &origin, bool explosive, bool ffa, int32_t taunt, bool isSteal) -> void {
	auto &channel = ChannelServer::getInstance();
	auto &globalDrops = channel.getDropDataProvider().getGlobalDrops();
	if (!channel.getDropDataProvider().hasDrops(droppingId) && globalDrops.size() == 0) {
		return;
	}

	// Make a copy of the data so we can modify the object with global drops
	auto drops = channel.getDropDataProvider().getDrops(droppingId);

	auto player = channel.getPlayerDataProvider().getPlayer(playerId);
	coord_t dropPosCounter = 0;
	party_id_t partyId = 0;
	Point pos;

	if (player != nullptr) {
		if (Party *party = player->getParty()) {
			partyId = party->getId();
		}
	}

	auto &config = channel.getConfig();
	int32_t dropRate = config.rates.dropRate;
	int32_t globalDropRate = config.rates.globalDropRate;
	int32_t mesoRate = config.rates.dropMeso;
	int32_t globalMesoRate = config.rates.globalDropMeso;
	if (config.rates.isGlobalDropConsistentWithRegularDropRate()) {
		globalDropRate = dropRate;
	}
	if (config.rates.isGlobalDropMesoConsistentWithRegularDropMesoRate()) {
		globalMesoRate = mesoRate;
	}

	if (droppingLevel != 0 && globalDrops.size() != 0) {
		DropInfo d;
		int8_t continent = channel.getMapDataProvider().getContinent(mapId).get(0);

		if (globalDropRate > 0) {
			for (const auto &globalDrop : globalDrops) {
				if (droppingLevel >= globalDrop.minLevel && droppingLevel <= globalDrop.maxLevel) {
					if (globalDrop.continent == 0 || (continent == globalDrop.continent)) {
						d = DropInfo{};
						d.isGlobal = true;
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
	}

	if (config.rates.dropRate == 0) {
		return;
	}

	Randomizer::shuffle(drops);
	coord_t mod = explosive ? 35 : 25;
	for (const auto &dropInfo : drops) {
		if (dropInfo.isMesos && mesoRate == 0) {
			continue;
		}
		if (dropInfo.isGlobal && dropInfo.isMesos && globalMesoRate == 0) {
			continue;
		}

		slot_qty_t amount = static_cast<slot_qty_t>(Randomizer::rand<int32_t>(dropInfo.maxAmount, dropInfo.minAmount));
		Drop *drop = nullptr;
		uint32_t chance = dropInfo.chance;

		if (isSteal) {
			chance = chance * 3 / 10;
		}
		else {
			chance = chance * taunt / 100;
			chance *= dropInfo.isGlobal ?
				globalDropRate :
				dropRate;
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
					Item{ChannelServer::getInstance().getEquipDataProvider(),
						itemId,
						Items::StatVariance::Normal,
						player != nullptr && player->hasGmBenefits()} :
					Item{itemId, amount};

				drop = new Drop{mapId, f, pos, playerId};

				if (questId > 0) {
					drop->setQuest(questId);
				}
			}
			else {
				mesos_t mesos = amount;
				if (!isSteal) {
					mesos *= dropInfo.isGlobal ?
						globalMesoRate :
						mesoRate;

					if (player != nullptr) {
						auto mesoUp = player->getActiveBuffs()->getMesoUpSource();
						if (mesoUp.is_initialized()) {
							mesos = (mesos * player->getActiveBuffs()->getBuffSkillInfo(mesoUp.get())->x) / 100;
						}
					}
				}
				drop = new Drop{mapId, mesos, pos, playerId};
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

auto DropHandler::dropMesos(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	reader.skip<tick_count_t>();
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

auto DropHandler::petLoot(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	pet_id_t petId = reader.get<pet_id_t>();
	lootItem(player, reader, petId);
}

auto DropHandler::lootItem(ref_ptr_t<Player> player, PacketReader &reader, pet_id_t petId) -> void {
	reader.unk<uint8_t>();
	reader.skip<tick_count_t>();
	Point playerPos = reader.get<Point>();
	map_object_t dropId = reader.get<map_object_t>();
	Drop *drop = player->getMap()->getDrop(dropId);

	if (drop == nullptr) {
		player->send(Packets::Drops::dontTake());
		return;
	}
	else if (drop->getPos() - player->getPos() > 300) {
		// Hacking
		return;
	}
	else if (player->isUsingGmHide()) {
		player->send(Packets::Drops::dropNotAvailableForPickup());
		player->send(Packets::Drops::dontTake());
		return;
	}

	if (drop->isQuest()) {
		if (player->getQuests()->itemDropAllowed(drop->getObjectId(), drop->getQuest()) == AllowQuestItemResult::Disallow) {
			player->send(Packets::Drops::dropNotAvailableForPickup());
			player->send(Packets::Drops::dontTake());
			return;
		}
	}
	if (drop->isMesos()) {
		int32_t playerRate = 100;
		mesos_t rawMesos = drop->getObjectId();
		auto giveMesos = [](ref_ptr_t<Player> p, mesos_t mesos) -> Result {
			if (p->getInventory()->modifyMesos(mesos, true)) {
				p->send(Packets::Drops::pickupDrop(mesos, 0, true));
			}
			else {
				p->send(Packets::Drops::dontTake());
				return Result::Failure;
			}
			return Result::Successful;
		};

		if (player->getParty() != nullptr && !drop->isPlayerDrop()) {
			// Player gets 100% unless partied and having others on the map, in which case it's 60%
			vector_t<ref_ptr_t<Player>> members = player->getParty()->getPartyMembers(player->getMapId());
			if (members.size() != 1) {
				playerRate = 60;
				mesos_t mesos = rawMesos * playerRate / 100;

				if (giveMesos(player, mesos) == Result::Failure) {
					// Can't pick up the mesos
					return;
				}

				playerRate = 40 / (members.size() - 1);
				mesos = rawMesos * playerRate / 100;
				ref_ptr_t<Player> p = nullptr;

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
				player->send(Packets::Drops::pickupDropSpecial(drop->getObjectId()));
				Inventory::useItem(player, dropItem.getId());
				player->send(Packets::Drops::dontTake());
				drop->takeDrop(player, petId);
				return;
			}
			Inventory::useItem(player, dropItem.getId());
		}
		else {
			auto item = new Item{dropItem};
			slot_qty_t dropAmount = drop->getAmount();
			if (item->hasKarma()) {
				item->setKarma(false);
			}
			slot_qty_t amount = Inventory::addItem(player, item, true);
			if (amount > 0) {
				if ((dropAmount - amount) > 0) {
					player->send(Packets::Drops::pickupDrop(drop->getObjectId(), dropAmount - amount));
					drop->setItemAmount(amount);
				}
				player->send(Packets::Drops::cantGetAnymoreItems());
				player->send(Packets::Drops::dontTake());
				return;
			}
		}
		// TODO FIXME Bug here? drop->getObjectId is going to be either a meso count or item identifier
		// pickupDrop packet calls for map_object_t and it's unclear which is correct and which isn't
		player->send(Packets::Drops::pickupDrop(drop->getObjectId(), drop->getAmount()));
	}
	ReactorHandler::checkLoot(drop);
	drop->takeDrop(player, petId);
}

}
}