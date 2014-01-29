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
#include "Pos.hpp"
#include "QuestDataProvider.hpp"
#include "Randomizer.hpp"
#include "ReactorHandler.hpp"
#include "SkillConstants.hpp"
#include "SkillDataProvider.hpp"
#include "Skills.hpp"
#include <algorithm>

auto DropHandler::doDrops(int32_t playerId, int32_t mapId, int32_t droppingLevel, int32_t droppingId, const Pos &origin, bool explosive, bool ffa, int32_t taunt, bool isSteal) -> void {
	auto &globalDrops = DropDataProvider::getInstance().getGlobalDrops();
	if (!DropDataProvider::getInstance().hasDrops(droppingId) && globalDrops.size() == 0) {
		return;
	}

	// Make a copy of the data so we can modify the object with global drops
	auto drops = DropDataProvider::getInstance().getDrops(droppingId);

	Player *player = PlayerDataProvider::getInstance().getPlayer(playerId);
	int16_t d = 0;
	int32_t partyId = 0;
	Pos pos;

	if (player != nullptr) {
		if (Party *party = player->getParty()) {
			partyId = party->getId();
		}
	}
	if (droppingLevel != 0 && globalDrops.size() != 0) {
		DropInfo d;
		int8_t continent = MapDataProvider::getInstance().getContinent(mapId);

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

	int16_t mod = 25;
	auto &config = ChannelServer::getInstance().getConfig();

	for (const auto &dropInfo : drops) {
		int16_t amount = static_cast<int16_t>(Randomizer::rand<int32_t>(dropInfo.maxAmount, dropInfo.minAmount));
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
			if (explosive) {
				mod = 35;
			}
			pos.x = origin.x + ((d % 2) ? (mod * (d + 1) / 2) : -(mod * (d / 2)));
			pos.y = origin.y;

			if (Maps::getMap(mapId)->getFhAtPosition(pos) == 0) {
				// pos = Maps::getMap(mapId)->findFloor(pos); // getFhAtPosition doesn't work correctly!
			}

			if (!dropInfo.isMesos) {
				int32_t itemId = dropInfo.itemId;
				int16_t questId = dropInfo.questId;

				if (questId > 0) {
					if (player == nullptr || player->getQuests()->itemDropAllowed(itemId, questId) == AllowQuestItemResult::Disallow) {
						continue;
					}
				}

				Item f = (GameLogicUtilities::isEquip(itemId) ? Item(itemId, true, player != nullptr && player->hasGmBenefits()) : Item(itemId, amount));
				drop = new Drop(mapId, f, pos, playerId);

				if (questId > 0) {
					drop->setPlayerId(playerId);
					drop->setQuest(questId);
				}
			}
			else {
				int32_t mesos = amount;
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
			d++;
			ReactorHandler::checkDrop(player, drop);
		}
	}
}

auto DropHandler::dropMesos(Player *player, PacketReader &packet) -> void {
	uint32_t ticks = packet.get<uint32_t>();
	int32_t amount = packet.get<int32_t>();
	if (amount < 10 || amount > 50000 || amount > player->getInventory()->getMesos()) {
		// Hacking
		return;
	}
	player->getInventory()->modifyMesos(-amount, true);
	Drop *drop = new Drop(player->getMapId(), amount, player->getPos(), player->getId(), true);
	drop->setTime(0);
	drop->doDrop(player->getPos());
}

auto DropHandler::petLoot(Player *player, PacketReader &packet) -> void {
	int64_t petId = packet.get<int64_t>();
	lootItem(player, packet, petId);
}

auto DropHandler::lootItem(Player *player, PacketReader &packet, int64_t petId) -> void {
	packet.skipBytes(5);
	const Pos &playerPos = packet.getClass<Pos>();
	int32_t dropId = packet.get<int32_t>();
	Drop *drop = player->getMap()->getDrop(dropId);

	if (drop == nullptr) {
		DropsPacket::dontTake(player);
		return;
	}
	else if (drop->getPos() - player->getPos() > 300) {
		// Hacking
		return;
	}

	if (drop->isQuest()) {
		if (player->getQuests()->itemDropAllowed(drop->getObjectId(), drop->getQuest()) == AllowQuestItemResult::Disallow) {
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
			vector_t<Player *> members = player->getParty()->getPartyMembers(player->getMapId());
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
		auto cons = ItemDataProvider::getInstance().getConsumeInfo(dropItem.getId());
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