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
#include "InventoryHandler.hpp"
#include "ChannelServer.hpp"
#include "CurseDataProvider.hpp"
#include "Drop.hpp"
#include "EquipDataProvider.hpp"
#include "GameLogicUtilities.hpp"
#include "InterHeader.hpp"
#include "Inventory.hpp"
#include "InventoryPacket.hpp"
#include "ItemConstants.hpp"
#include "ItemDataProvider.hpp"
#include "MapleTvs.hpp"
#include "Maps.hpp"
#include "MobDataProvider.hpp"
#include "NpcDataProvider.hpp"
#include "PacketReader.hpp"
#include "PacketWrapper.hpp"
#include "Pet.hpp"
#include "PetHandler.hpp"
#include "PetsPacket.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "Randomizer.hpp"
#include "ReactorHandler.hpp"
#include "ScriptDataProvider.hpp"
#include "ShopDataProvider.hpp"
#include "StoragePacket.hpp"
#include "ValidCharDataProvider.hpp"

auto InventoryHandler::itemMove(Player *player, PacketReader &reader) -> void {
	uint32_t ticks = reader.get<uint32_t>();
	int8_t inv = reader.get<int8_t>();
	int16_t slot1 = reader.get<int16_t>();
	int16_t slot2 = reader.get<int16_t>();
	bool dropped = (slot2 == 0);
	bool equippedSlot1 = (slot1 < 0);
	bool equippedSlot2 = (slot2 < 0);
	int16_t strippedSlot1 = GameLogicUtilities::stripCashSlot(slot1);
	int16_t strippedSlot2 = GameLogicUtilities::stripCashSlot(slot2);
	auto testSlot = [&slot1, &slot2, &strippedSlot1, &strippedSlot2](int16_t testSlot) -> bool {
		return (slot1 < 0 && strippedSlot1 == testSlot) || (slot2 < 0 && strippedSlot2 == testSlot);
	};

	if (dropped) {
		Item *item1 = player->getInventory()->getItem(inv, slot1);
		if (item1 == nullptr) {
			return;
		}
		InventoryHandler::dropItem(player, reader, item1, slot1, inv);
	}
	else {
		player->getInventory()->swapItems(inv, slot1, slot2);
	}

	if (equippedSlot1 || equippedSlot2) {
		auto testPetSlot = [&player, &testSlot](int16_t equipSlot, int32_t petIndex) {
			if (testSlot(equipSlot)) {
				if (Pet *pet = player->getPets()->getSummoned(petIndex)) {
					player->sendMap(PetsPacket::changeName(player->getId(), pet));
				}
			}
		};
		// Check if any label ring changed, so we can update the look of the apropos pet
		testPetSlot(EquipSlots::PetLabelRing1, 0);
		testPetSlot(EquipSlots::PetLabelRing2, 1);
		testPetSlot(EquipSlots::PetLabelRing3, 2);

		player->sendMap(InventoryPacket::updatePlayer(player));
	}
}

auto InventoryHandler::dropItem(Player *player, PacketReader &reader, Item *item, int16_t slot, int8_t inv) -> void {
	int16_t amount = reader.get<int16_t>();
	if (!GameLogicUtilities::isStackable(item->getId())) {
		amount = item->getAmount();
	}
	else if (amount <= 0 || amount > item->getAmount()) {
		// Hacking
		return;
	}
	if (GameLogicUtilities::isGmEquip(item->getId()) || item->hasLock()) {
		// We don't allow these to be dropped or traded
		return;
	}

	Item droppedItem(item);
	droppedItem.setAmount(amount);
	if (item->getAmount() == amount) {
		vector_t<InventoryPacketOperation> ops;
		ops.emplace_back(InventoryPacket::OperationTypes::ModifySlot, item, slot);
		player->send(InventoryPacket::inventoryOperation(true, ops));

		player->getInventory()->deleteItem(inv, slot);
	}
	else {
		item->decAmount(amount);
		player->getInventory()->changeItemAmount(item->getId(), -amount);

		vector_t<InventoryPacketOperation> ops;
		ops.emplace_back(InventoryPacket::OperationTypes::ModifyQuantity, item, slot);
		player->send(InventoryPacket::inventoryOperation(true, ops));
	}

	auto itemInfo = ItemDataProvider::getInstance().getItemInfo(droppedItem.getId());
	bool isTradeable = !(droppedItem.hasTradeBlock() || itemInfo->quest || itemInfo->noTrade);
	Drop *drop = new Drop(player->getMapId(), droppedItem, player->getPos(), player->getId(), true);
	drop->setTime(0);
	drop->setTradeable(isTradeable);
	drop->doDrop(player->getPos());

	if (isTradeable) {
		// Drop is deleted otherwise, avoid like plague
		ReactorHandler::checkDrop(player, drop);
	}
}

auto InventoryHandler::useItem(Player *player, PacketReader &reader) -> void {
	uint32_t ticks = reader.get<uint32_t>();
	int16_t slot = reader.get<int16_t>();
	int32_t itemId = reader.get<int32_t>();
	const int16_t amountUsed = 1;
	if (player->getStats()->isDead() || player->getInventory()->getItemAmountBySlot(Inventories::UseInventory, slot) < amountUsed) {
		// Hacking
		return;
	}

	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (item == nullptr || item->getId() != itemId) {
		// Hacking
		return;
	}

	if (!player->hasGmBenefits()) {
		Inventory::takeItemSlot(player, Inventories::UseInventory, slot, amountUsed);
	}

	Inventory::useItem(player, itemId);
}

auto InventoryHandler::cancelItem(Player *player, PacketReader &reader) -> void {
	int32_t itemId = reader.get<int32_t>();
	Buffs::endBuff(player, itemId);
}

auto InventoryHandler::useSkillbook(Player *player, PacketReader &reader) -> void {
	uint32_t ticks = reader.get<uint32_t>();
	int16_t slot = reader.get<int16_t>();
	int32_t itemId = reader.get<int32_t>();

	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (item == nullptr || item->getId() != itemId) {
		// Hacking
		return;
	}
	
	auto skillbookItems = ItemDataProvider::getInstance().getItemSkills(itemId);
	if (skillbookItems == nullptr) {
		// Hacking
		return;
	}

	int32_t skillId = 0;
	uint8_t newMaxLevel = 0;
	bool use = false;
	bool succeed = false;

	for (const auto &s : *skillbookItems) {
		skillId = s.skillId;
		newMaxLevel = s.maxLevel;
		if (GameLogicUtilities::itemSkillMatchesJob(skillId, player->getStats()->getJob())) {
			if (player->getSkills()->getSkillLevel(skillId) >= s.reqLevel) {
				if (player->getSkills()->getMaxSkillLevel(skillId) < newMaxLevel) {
					if (Randomizer::rand<int8_t>(99) < s.chance) {
						player->getSkills()->setMaxSkillLevel(skillId, newMaxLevel);
						succeed = true;
					}

					Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);
					break;
				}
			}
		}
	}

	if (skillId != 0) {
		player->sendMap(InventoryPacket::useSkillbook(player->getId(), skillId, newMaxLevel, true, succeed));
	}
}

auto InventoryHandler::useChair(Player *player, PacketReader &reader) -> void {
	int32_t chairId = reader.get<int32_t>();
	player->setChair(chairId);
	player->sendMap(InventoryPacket::sitChair(player->getId(), chairId));
}

auto InventoryHandler::handleChair(Player *player, PacketReader &reader) -> void {
	int16_t chair = reader.get<int16_t>();
	Map *map = player->getMap();
	if (chair == -1) {
		if (player->getChair() != 0) {
			player->setChair(0);
		}
		else {
			map->playerSeated(player->getMapChair(), nullptr);
			player->setMapChair(0);
		}
		map->send(InventoryPacket::stopChair(player->getId(), false), player);
	}
	else {
		// Map chair
		if (map->seatOccupied(chair)) {
			map->send(InventoryPacket::stopChair(player->getId(), true), player);
		}
		else {
			map->playerSeated(chair, player);
			player->setMapChair(chair);
			player->send(InventoryPacket::sitMapChair(chair));
		}
	}
}

auto InventoryHandler::useSummonBag(Player *player, PacketReader &reader) -> void {
	uint32_t ticks = reader.get<uint32_t>();
	int16_t slot = reader.get<int16_t>();
	int32_t itemId = reader.get<int32_t>();

	auto itemInfo = ItemDataProvider::getInstance().getItemSummons(itemId);
	if (itemInfo == nullptr) {
		// Most likely hacking
		return;
	}

	Item *inventoryItem = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (inventoryItem == nullptr || inventoryItem->getId() != itemId) {
		// Hacking
		return;
	}

	Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);

	for (const auto &bag : *itemInfo) {
		if (Randomizer::rand<uint32_t>(99) < bag.chance) {
			if (MobDataProvider::getInstance().mobExists(bag.mobId)) {
				player->getMap()->spawnMob(bag.mobId, player->getPos());
			}
		}
	}
}

auto InventoryHandler::useReturnScroll(Player *player, PacketReader &reader) -> void {
	uint32_t ticks = reader.get<uint32_t>();
	int16_t slot = reader.get<int16_t>();
	int32_t itemId = reader.get<int32_t>();

	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (item == nullptr || item->getId() != itemId) {
		// Hacking
		return;
	}
	auto info = ItemDataProvider::getInstance().getConsumeInfo(itemId);
	if (info == nullptr) {
		// Probably hacking
		return;
	}

	Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);

	int32_t map = info->moveTo;
	player->setMap(map == Maps::NoMap ? player->getMap()->getReturnMap() : map);
}

auto InventoryHandler::useScroll(Player *player, PacketReader &reader) -> void {
	uint32_t ticks = reader.get<uint32_t>();
	int16_t slot = reader.get<int16_t>();
	int16_t equipSlot = reader.get<int16_t>();
	bool whiteScroll = (reader.get<int16_t>() == 2);
	bool legendarySpirit = reader.get<bool>();

	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	Item *equip = player->getInventory()->getItem(Inventories::EquipInventory, equipSlot);
	if (item == nullptr || equip == nullptr) {
		// Most likely hacking
		return;
	}

	int32_t itemId = item->getId();
	int8_t succeed = -1;
	bool cursed = false;
	ItemDataProvider::getInstance().scrollItem(itemId, equip, whiteScroll, player->hasGmBenefits(), succeed, cursed);

	if (succeed != -1) {
		if (whiteScroll) {
			Inventory::takeItem(player, Items::WhiteScroll, 1);
		}

		Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);
		player->sendMap(InventoryPacket::useScroll(player->getId(), succeed, cursed, legendarySpirit));

		if (!cursed) {
			player->getStats()->setEquip(equipSlot, equip);

			vector_t<InventoryPacketOperation> ops;
			ops.emplace_back(InventoryPacket::OperationTypes::AddItem, equip, equipSlot);
			player->send(InventoryPacket::inventoryOperation(true, ops));
		}
		else {
			vector_t<InventoryPacketOperation> ops;
			ops.emplace_back(InventoryPacket::OperationTypes::ModifySlot, equip, equipSlot);
			player->send(InventoryPacket::inventoryOperation(true, ops));

			player->getInventory()->deleteItem(Inventories::EquipInventory, equipSlot);
		}

		player->sendMap(InventoryPacket::updatePlayer(player));
	}
	else {
		if (legendarySpirit) {
			player->sendMap(InventoryPacket::useScroll(player->getId(), succeed, cursed, legendarySpirit));
		}
		player->send(InventoryPacket::blankUpdate());
	}
}

auto InventoryHandler::useCashItem(Player *player, PacketReader &reader) -> void {
	reader.get<int8_t>(); // Type
	reader.skipBytes(1);
	int32_t itemId = reader.get<int32_t>();

	if (player->getInventory()->getItemAmount(itemId) == 0) {
		// Hacking
		return;
	}

	auto itemInfo = ItemDataProvider::getInstance().getItemInfo(itemId);
	bool used = false;
	if (GameLogicUtilities::getItemType(itemId) == Items::Types::WeatherCash) {
		string_t message = reader.getString();
		uint32_t ticks = reader.get<uint32_t>();
		if (message.length() <= 35) {
			Map *map = player->getMap();
			message = player->getName() + " 's message : " + message;
			used = map->createWeather(player, false, Items::WeatherTime, itemId, message);
		}
	}
	else if (GameLogicUtilities::getItemType(itemId) == Items::Types::CashPetFood) {
		Pet *pet = player->getPets()->getSummoned(0);
		if (pet != nullptr) {
			if (pet->getFullness() < Stats::MaxFullness) {
				player->send(PetsPacket::showAnimation(player->getId(), pet, 1));
				pet->modifyFullness(Stats::MaxFullness, false);
				pet->addCloseness(100); // All cash pet food gives 100 closeness
				used = true;
			}
		}
	}
	else {
		switch (itemId) {
			case Items::TeleportRock:
			case Items::TeleportCoke:
			case Items::VipRock: // Only occurs when you actually try to move somewhere
				used = handleRockTeleport(player, itemId, reader);
				break;
			case Items::FirstJobSpReset:
			case Items::SecondJobSpReset:
			case Items::ThirdJobSpReset:
			case Items::FourthJobSpReset: {
				int32_t toSkill = reader.get<int32_t>();
				int32_t fromSkill = reader.get<int32_t>();
				if (!player->getSkills()->addSkillLevel(fromSkill, -1, true)) {
					// Hacking
					return;
				}
				if (!player->getSkills()->addSkillLevel(toSkill, 1, true)) {
					// Hacking
					return;
				}
				used = true;
				break;
			}
			case Items::ApReset: {
				int32_t toStat = reader.get<int32_t>();
				int32_t fromStat = reader.get<int32_t>();
				player->getStats()->addStat(toStat, 1, true);
				player->getStats()->addStat(fromStat, -1, true);
				used = true;
				break;
			}
			case Items::Megaphone: {
				string_t msg = player->getMedalName() + " : " + reader.getString();
				// In global, this sends to everyone on the current channel, not the map
				PlayerDataProvider::getInstance().send(InventoryPacket::showMegaphone(msg));
				used = true;
				break;
			}
			case Items::SuperMegaphone: {
				string_t msg = player->getMedalName() + " : " + reader.getString();
				bool whisper = reader.get<bool>();
				auto &basePacket = InventoryPacket::showSuperMegaphone(msg, whisper);
				ChannelServer::getInstance().sendWorld(
					Packets::prepend(basePacket, [](PacketBuilder &builder) {
						builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
					}));
				used = true;
				break;
			}
			case Items::DiabloMessenger:
			case Items::Cloud9Messenger:
			case Items::LoveholicMessenger: {
				string_t msg1 = reader.getString();
				string_t msg2 = reader.getString();
				string_t msg3 = reader.getString();
				string_t msg4 = reader.getString();

				auto &basePacket = InventoryPacket::showMessenger(player->getName(), msg1, msg2, msg3, msg4, reader.getBuffer(), reader.getBufferLength(), itemId);
				ChannelServer::getInstance().sendWorld(
					Packets::prepend(basePacket, [](PacketBuilder &builder) {
						builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
					}));
				used = true;
				break;
			}
			case Items::ItemMegaphone: {
				string_t msg = player->getMedalName() + " : " + reader.getString();
				bool whisper = reader.get<bool>();
				Item *item = nullptr;
				if (reader.get<bool>()) {
					int8_t inv = static_cast<int8_t>(reader.get<int32_t>());
					int16_t slot = static_cast<int16_t>(reader.get<int32_t>());
					item = player->getInventory()->getItem(inv, slot);
					if (item == nullptr) {
						// Hacking
						return;
					}
				}
	
				auto &basePacket = InventoryPacket::showItemMegaphone(msg, whisper, item);
				ChannelServer::getInstance().sendWorld(
					Packets::prepend(basePacket, [](PacketBuilder &builder) {
						builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
					}));
				used = true;
				break;
			}
			case Items::ArtMegaphone: {
				int8_t lines = reader.get<int8_t>();
				if (lines < 1 || lines > 3) {
					// Hacking
					return;
				}
				string_t text[3];
				for (int8_t i = 0; i < lines; i++) {
					text[i] = player->getMedalName() + " : " + reader.getString();
				}

				bool whisper = reader.get<bool>();
				auto &basePacket = InventoryPacket::showTripleMegaphone(lines, text[0], text[1], text[2], whisper);
				ChannelServer::getInstance().sendWorld(
					Packets::prepend(basePacket, [](PacketBuilder &builder) {
						builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
					}));

				used = true;
				break;
			}
			case Items::PetNameTag: {
				const string_t &name = reader.getString();
				if (ValidCharDataProvider::getInstance().isForbiddenName(name) || CurseDataProvider::getInstance().isCurseWord(name)) {
					// Don't think it's hacking, but it should be forbidden
					return;
				}
				else {
					PetHandler::changeName(player, name);
					used = true;
				}
				break;
			}
			case Items::ItemNameTag: {
				int16_t slot = reader.get<int16_t>();
				if (slot != 0) {
					Item *item = player->getInventory()->getItem(Inventories::EquipInventory, slot);
					if (item == nullptr) {
						// Hacking or failure, dunno
						return;
					}
					item->setName(player->getName());

					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(InventoryPacket::OperationTypes::AddItem, item, slot);
					player->send(InventoryPacket::inventoryOperation(true, ops));
					used = true;
				}
				break;
			}
			case Items::ScissorsOfKarma:
			case Items::ItemLock: {
				int8_t inv = static_cast<int8_t>(reader.get<int32_t>());
				int16_t slot = static_cast<int16_t>(reader.get<int32_t>());
				if (slot != 0) {
					Item *item = player->getInventory()->getItem(inv, slot);
					if (item == nullptr) {
						// Hacking or failure, dunno
						return;
					}
					switch (itemId) {
						case Items::ItemLock:
							if (item->hasLock()) {
								// Hacking
								return;
							}
							item->setLock(true);
							break;
						case Items::ScissorsOfKarma:
							if (!itemInfo->karmaScissors) {
								// Hacking
								return;
							}
							if (item->hasTradeBlock() || item->hasKarma()) {
								// Hacking
								return;
							}
							item->setKarma(true);
							break;
					}

					vector_t<InventoryPacketOperation> ops;
					ops.emplace_back(InventoryPacket::OperationTypes::AddItem, item, slot);
					player->send(InventoryPacket::inventoryOperation(true, ops));
					used = true;
				}
				break;
			}
			case Items::MapleTvMessenger:
			case Items::Megassenger: {
				bool hasReceiver = (reader.get<int8_t>() == 3);
				bool showWhisper = (itemId == Items::Megassenger ? reader.get<bool>() : false);
				Player *receiver = PlayerDataProvider::getInstance().getPlayer(reader.getString());
				int32_t time = 15;

				if ((hasReceiver && receiver != nullptr) || (!hasReceiver && receiver == nullptr)) {
					string_t msg1 = reader.getString();
					string_t msg2 = reader.getString();
					string_t msg3 = reader.getString();
					string_t msg4 = reader.getString();
					string_t msg5 = reader.getString();
					uint32_t ticks = reader.get<uint32_t>();

					MapleTvs::getInstance().addMessage(player, receiver, msg1, msg2, msg3, msg4, msg5, itemId - (itemId == Items::Megassenger ? 3 : 0), time);

					if (itemId == Items::Megassenger) {
						auto &basePacket = InventoryPacket::showSuperMegaphone(player->getMedalName() + " : " + msg1 + msg2 + msg3 + msg4 + msg5, showWhisper);
						ChannelServer::getInstance().sendWorld(
							Packets::prepend(basePacket, [](PacketBuilder &builder) {
								builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
							}));

					}
					used = true;
				}
				break;
			}
			case Items::MapleTvStarMessenger:
			case Items::StarMegassenger: {
				int32_t time = 30;
				bool showWhisper = (itemId == Items::StarMegassenger ? reader.get<bool>() : false);
				string_t msg1 = reader.getString();
				string_t msg2 = reader.getString();
				string_t msg3 = reader.getString();
				string_t msg4 = reader.getString();
				string_t msg5 = reader.getString();
				uint32_t ticks = reader.get<uint32_t>();

				MapleTvs::getInstance().addMessage(player, nullptr, msg1, msg2, msg3, msg4, msg5, itemId - (itemId == Items::StarMegassenger ? 3 : 0), time);

				if (itemId == Items::StarMegassenger) {
					auto &basePacket = InventoryPacket::showSuperMegaphone(player->getMedalName() + " : " + msg1 + msg2 + msg3 + msg4 + msg5, showWhisper);
					ChannelServer::getInstance().sendWorld(
						Packets::prepend(basePacket, [](PacketBuilder &builder) {
							builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
						}));
				}
				used = true;
				break;
			}
			case Items::MapleTvHeartMessenger:
			case Items::HeartMegassenger: {
				bool showWhisper = (itemId == Items::HeartMegassenger ? reader.get<bool>() : false);
				string_t name = reader.getString();
				Player *receiver = PlayerDataProvider::getInstance().getPlayer(name);
				int32_t time = 60;

				if (receiver != nullptr) {
					string_t msg1 = reader.getString();
					string_t msg2 = reader.getString();
					string_t msg3 = reader.getString();
					string_t msg4 = reader.getString();
					string_t msg5 = reader.getString();
					uint32_t ticks = reader.get<uint32_t>();

					MapleTvs::getInstance().addMessage(player, receiver, msg1, msg2, msg3, msg4, msg5, itemId - (itemId == Items::HeartMegassenger ? 3 : 0), time);

					if (itemId == Items::HeartMegassenger) {
						auto &basePacket = InventoryPacket::showSuperMegaphone(player->getMedalName() + " : " + msg1 + msg2 + msg3 + msg4 + msg5, showWhisper);
						ChannelServer::getInstance().sendWorld(
							Packets::prepend(basePacket, [](PacketBuilder &builder) {
								builder.add<header_t>(IMSG_TO_ALL_PLAYERS);
							}));
					}
					used = true;
				}
				break;
			}
			case Items::BronzeSackOfMesos:
			case Items::SilverSackOfMesos:
			case Items::GoldSackOfMesos: {
				int32_t mesos = itemInfo->mesos;
				if (!player->getInventory()->modifyMesos(mesos)) {
					player->send(InventoryPacket::sendMesobagFailed());
				}
				else {
					player->send(InventoryPacket::sendMesobagSucceed(mesos));
					used = true;
				}
				break;
			}
			case Items::Chalkboard:
			case Items::Chalkboard2: {
				string_t msg = reader.getString();
				player->setChalkboard(msg);
				player->sendMap(InventoryPacket::sendChalkboardUpdate(player->getId(), msg));
				break;
			}
			case Items::FungusScroll:
			case Items::OinkerDelight:
			case Items::ZetaNightmare:
				Inventory::useItem(player, itemId);
				used = true;
				break;
			case Items::ViciousHammer: {
				int8_t inv = static_cast<int8_t>(reader.get<int32_t>());
				int16_t slot = static_cast<int16_t>(reader.get<int32_t>());
				Item *item = player->getInventory()->getItem(inv, slot);
				if (item == nullptr || item->getHammers() == Items::MaxHammers || EquipDataProvider::getInstance().getSlots(item->getId()) == 0) {
					// Hacking, probably
					return;
				}
				item->incHammers();
				item->incSlots();
				player->send(InventoryPacket::sendHammerSlots(item->getHammers()));
				player->getInventory()->setHammerSlot(slot);
				used = true;
				break;
			}
			case Items::CongratulatorySong:
				player->sendMap(InventoryPacket::playCashSong(itemId, player->getName()));
				used = true;
				break;
		}
	}
	if (used) {
		Inventory::takeItem(player, itemId, 1);
	}
	else {
		player->send(InventoryPacket::blankUpdate());
	}
}

auto InventoryHandler::useItemEffect(Player *player, PacketReader &reader) -> void {
	int32_t itemId = reader.get<int32_t>();
	if (player->getInventory()->getItemAmount(itemId) == 0) {
		// Hacking
		return;
	}
	player->setItemEffect(itemId);
	player->sendMap(InventoryPacket::useItemEffect(player->getId(), itemId));
}

auto InventoryHandler::handleRockFunctions(Player *player, PacketReader &reader) -> void {
	int8_t mode = reader.get<int8_t>();
	int8_t type = reader.get<int8_t>();

	enum Modes : int8_t {
		Remove = 0x00,
		Add = 0x01
	};

	if (mode == Remove) {
		int32_t map = reader.get<int32_t>();
		player->getInventory()->delRockMap(map, type);
	}
	else if (mode == Add) {
		int32_t map = player->getMapId();
		Map *m = Maps::getMap(map);
		if (m->canVip() && m->getContinent() != 0) {
			player->getInventory()->addRockMap(map, type);
		}
		else {
			// Hacking, the client doesn't allow this to occur
			player->send(InventoryPacket::sendRockError(InventoryPacket::RockErrors::CannotSaveMap, type));
		}
	}
}

auto InventoryHandler::handleRockTeleport(Player *player, int32_t itemId, PacketReader &reader) -> bool {
	if (itemId == Items::SpecialTeleportRock) {
		reader.skipBytes(5);
	}
	int8_t type = InventoryPacket::RockTypes::Regular;
	switch (itemId) {
		case Items::VipRock: type = InventoryPacket::RockTypes::Vip; break;
		case Items::SpecialTeleportRock: type = reader.get<int8_t>(); break;
	}
	bool used = false;
	int8_t mode = reader.get<int8_t>();
	int32_t targetMapId = -1;

	enum Modes : int8_t {
		PresetMap = 0x00,
		Ign = 0x01
	};
	if (mode == PresetMap) {
		targetMapId = reader.get<int32_t>();
		if (!player->getInventory()->ensureRockDestination(targetMapId)) {
			// Hacking
			return false;
		}
	}
	else if (mode == Ign) {
		const string_t &targetName = reader.getString();
		Player *target = PlayerDataProvider::getInstance().getPlayer(targetName);
		if (target != nullptr && target != player) {
			targetMapId = target->getMapId();
		}
		else if (target == nullptr) {
			player->send(InventoryPacket::sendRockError(InventoryPacket::RockErrors::DifficultToLocate, type));
		}
		else if (target == player) {
			// Hacking
			return false;
		}
	}
	if (targetMapId != -1) {
		Map *destination = Maps::getMap(targetMapId);
		Map *origin = player->getMap();
		if (!destination->canVip()) {
			player->send(InventoryPacket::sendRockError(InventoryPacket::RockErrors::CannotGo, type));
		}
		else if (!origin->canVip()) {
			player->send(InventoryPacket::sendRockError(InventoryPacket::RockErrors::CannotGo, type));
		}
		else if (player->getMapId() == targetMapId) {
			player->send(InventoryPacket::sendRockError(InventoryPacket::RockErrors::AlreadyThere, type));
		}
		else if (type == 0 && destination->getContinent()!= origin->getContinent()) {
			player->send(InventoryPacket::sendRockError(InventoryPacket::RockErrors::CannotGo, type));
		}
		else if (player->getStats()->getLevel() < 7 && origin->getContinent() == 0 && destination->getContinent() != 0) {
			player->send(InventoryPacket::sendRockError(InventoryPacket::RockErrors::NoobsCannotLeaveMapleIsland, type));
		}
		else {
			player->setMap(targetMapId);
			used = true;
		}
	}
	if (itemId == Items::SpecialTeleportRock) {
		if (used) {
			Inventory::takeItem(player, itemId, 1);
		}
		else {
			player->send(InventoryPacket::blankUpdate());
		}
	}
	return used;
}

auto InventoryHandler::handleHammerTime(Player *player) -> void {
	if (!player->getInventory()->isHammering()) {
		// Hacking
		return;
	}
	int16_t hammerSlot = player->getInventory()->getHammerSlot();
	Item *item = player->getInventory()->getItem(Inventories::EquipInventory, hammerSlot);
	player->send(InventoryPacket::sendHammerUpdate());
	player->send(InventoryPacket::sendHulkSmash(hammerSlot, item));
	player->getInventory()->setHammerSlot(-1);
}

auto InventoryHandler::handleRewardItem(Player *player, PacketReader &reader) -> void {
	int16_t slot = reader.get<int16_t>();
	int32_t itemId = reader.get<int32_t>();
	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (item == nullptr || item->getId() != itemId) {
		// Hacking or hacking failure
		player->send(InventoryPacket::blankUpdate()); // We don't want stuck players, do we?
		return;
	}

	auto rewards = ItemDataProvider::getInstance().getItemRewards(itemId);
	if (rewards == nullptr) {
		// Hacking or no information in the database
		player->send(InventoryPacket::blankUpdate()); // We don't want stuck players, do we?
		return;
	}

	auto reward = Randomizer::select(rewards);
	Inventory::takeItem(player, itemId, 1);
	Item *rewardItem = new Item(reward->rewardId, reward->quantity);
	Inventory::addItem(player, rewardItem, true);
	player->sendMap(InventoryPacket::sendRewardItemAnimation(player->getId(), itemId, reward->effect));
}

auto InventoryHandler::handleScriptItem(Player *player, PacketReader &reader) -> void {
	if (player->getNpc() != nullptr || player->getShop() != 0 || player->getTradeId() != 0) {
		// Hacking
		player->send(InventoryPacket::blankUpdate());
		return;
	}

	uint32_t ticks = reader.get<uint32_t>();
	int16_t slot = reader.get<int16_t>();
	int32_t itemId = reader.get<int32_t>();

	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (item == nullptr || item->getId() != itemId) {
		// Hacking or hacking failure
		player->send(InventoryPacket::blankUpdate());
		return;
	}

	const string_t &scriptName = ScriptDataProvider::getInstance().getScript(itemId, ScriptTypes::Item);
	if (scriptName == "") {
		// Hacking or no script for item found
		player->send(InventoryPacket::blankUpdate());
		return;
	}

	int32_t npcId = ItemDataProvider::getInstance().getItemInfo(itemId)->npc;

	// Let's run the NPC
	Npc *npc = new Npc(npcId, player, scriptName);
	if (!npc->checkEnd()) {
		// NPC is running/script found
		// Delete the item used
		Inventory::takeItem(player, itemId, 1);
		npc->run();
	}
	else {
		// NPC didn't run/no script found
		player->send(InventoryPacket::blankUpdate());
	}
}