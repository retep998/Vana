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
#include "InventoryHandler.h"
#include "ChannelServer.h"
#include "Drop.h"
#include "EquipDataProvider.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "ItemConstants.h"
#include "ItemDataProvider.h"
#include "MapleTvs.h"
#include "Maps.h"
#include "MobDataProvider.h"
#include "NpcDataProvider.h"
#include "PacketReader.h"
#include "Pet.h"
#include "PetHandler.h"
#include "PetsPacket.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Randomizer.h"
#include "ReactorHandler.h"
#include "ScriptDataProvider.h"
#include "ShopDataProvider.h"
#include "StoragePacket.h"

void InventoryHandler::itemMove(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}

	int8_t inv = packet.get<int8_t>();
	int16_t slot1 = packet.get<int16_t>();
	int16_t slot2 = packet.get<int16_t>();
	if (slot2 == 0) {
		// Dropping an item
		int16_t amount = packet.get<int16_t>();
		Item *item = player->getInventory()->getItem(inv, slot1);
		if (item == nullptr) {
			return;
		}

		if (!GameLogicUtilities::isStackable(item->getId())) {
			amount = item->getAmount();
		}
		else if (amount <= 0 || amount > item->getAmount()) {
			// Hacking
			return;
		}
		else if (ItemDataProvider::Instance()->isCash(item->getId())) {
			// Hacks or modded client.
			return;
		}

		Item droppeditem(item);
		droppeditem.setAmount(amount);
		if (item->getAmount() == amount) {
			InventoryPacket::moveItem(player, inv, slot1, slot2);
			player->getInventory()->deleteItem(inv, slot1);
		}
		else {
			item->decAmount(amount);
			player->getInventory()->changeItemAmount(item->getId(), -amount);
			InventoryPacket::updateItemAmounts(player, inv, slot1, item->getAmount(), 0, 0);
		}
		Drop *drop = new Drop(player->getMap(), droppeditem, player->getPos(), player->getId(), true);
		drop->setTime(0);

		bool istradeable = ItemDataProvider::Instance()->isTradeable(droppeditem.getId());
		drop->setTradeable(istradeable);

		drop->doDrop(player->getPos());
		if (istradeable) {
			// Drop is deleted otherwise, avoid like plague
			ReactorHandler::checkDrop(player, drop);
		}
	}
	else {
		// Change item slot (swapping)
		Item *item1 = player->getInventory()->getItem(inv, slot1);
		Item *item2 = player->getInventory()->getItem(inv, slot2);

		if (item1 == nullptr) {
			// Hacking
			return;
		}

		if (item2 != nullptr && GameLogicUtilities::isStackable(item1->getId()) && item1->getId() == item2->getId()) {
			if (item1->getAmount() + item2->getAmount() <= ItemDataProvider::Instance()->getMaxSlot(item1->getId())) {
				item2->incAmount(item1->getAmount());
				player->getInventory()->deleteItem(inv, slot1, false);
				InventoryPacket::updateItemAmounts(player, inv, slot2, item2->getAmount(), 0, 0);
				InventoryPacket::moveItem(player, inv, slot1, 0);
			}
			else {
				item1->decAmount(ItemDataProvider::Instance()->getMaxSlot(item1->getId()) - item2->getAmount());
				item2->setAmount(ItemDataProvider::Instance()->getMaxSlot(item2->getId()));
				InventoryPacket::updateItemAmounts(player, inv, slot1, item1->getAmount(), slot2, item2->getAmount());
			}
		}
		else {
			if (slot2 < 0) {
				bool iscash = ItemDataProvider::Instance()->isCash(item1->getId());
				uint8_t desiredslot = -(slot2 + (iscash ? 100 : 0));
				if (!EquipDataProvider::Instance()->validSlot(item1->getId(), desiredslot)) {
					// Hacking
					return;
				}
				Item *remove = nullptr;
				int16_t oldslot = 0;
				bool weapon = -slot2 == EquipSlots::Weapon;
				bool shield = -slot2 == EquipSlots::Shield;
				bool top = -slot2 == EquipSlots::Top;
				bool bottom = -slot2 == EquipSlots::Bottom;

				if (weapon && GameLogicUtilities::is2hWeapon(item1->getId()) && player->getInventory()->getEquippedId(EquipSlots::Shield) != 0) {
					oldslot = -EquipSlots::Shield;
				}
				else if (shield && GameLogicUtilities::is2hWeapon(player->getInventory()->getEquippedId(EquipSlots::Weapon))) {
					oldslot = -EquipSlots::Weapon;
				}
				else if (top && GameLogicUtilities::isOverall(item1->getId()) && player->getInventory()->getEquippedId(EquipSlots::Bottom) != 0) {
					oldslot = -EquipSlots::Bottom;
				}
				else if (bottom && GameLogicUtilities::isOverall(player->getInventory()->getEquippedId(EquipSlots::Top))) {
					oldslot = -EquipSlots::Top;
				}
				if (oldslot != 0) {
					remove = player->getInventory()->getItem(inv, oldslot);
					bool onlyswap = true;
					if ((player->getInventory()->getEquippedId(EquipSlots::Shield) != 0) && (player->getInventory()->getEquippedId(EquipSlots::Weapon) != 0)) {
						onlyswap = false;
					}
					else if ((player->getInventory()->getEquippedId(EquipSlots::Top) != 0) && (player->getInventory()->getEquippedId(EquipSlots::Bottom) != 0)) {
						onlyswap = false;
					}
					if (onlyswap) {
						int16_t swapslot = 0;
						if (weapon) {
							swapslot = -EquipSlots::Shield;
							player->getActiveBuffs()->stopBooster();
							player->getActiveBuffs()->stopCharge();
						}
						else if (shield) {
							swapslot = -EquipSlots::Weapon;
							player->getActiveBuffs()->stopBooster();
							player->getActiveBuffs()->stopCharge();
						}
						else if (top) {
							swapslot = -EquipSlots::Bottom;
						}
						else if (bottom) {
							swapslot = -EquipSlots::Top;
						}
						player->getInventory()->setItem(inv, swapslot, nullptr);
						player->getInventory()->setItem(inv, slot1, remove);
						player->getInventory()->setItem(inv, slot2, item1);

						InventoryPacket::moveItem(player, inv, slot1, slot2);
						InventoryPacket::moveItem(player, inv, swapslot, slot1);
						InventoryPacket::updatePlayer(player);
						return;
					}
					else {
						if (player->getInventory()->getOpenSlotsNum(inv) == 0) {
							InventoryPacket::blankUpdate(player);
							return;
						}
						int16_t freeslot = 0;
						for (int16_t s = 1; s <= player->getInventory()->getMaxSlots(inv); s++) {
							Item *olditem = player->getInventory()->getItem(inv, s);
							if (olditem == nullptr) {
								freeslot = s;
								break;
							}
						}
						player->getInventory()->setItem(inv, freeslot, remove);
						player->getInventory()->setItem(inv, oldslot, nullptr);
						InventoryPacket::moveItem(player, inv, oldslot, freeslot);
					}
				}
			}
			else if (slot1 < 0 && item2 != nullptr && !ItemDataProvider::Instance()->isCash(item2->getId())) {
				// Client tries to switch a cash item with a regular item
				return;
			}

			player->getInventory()->setItem(inv, slot1, item2);
			player->getInventory()->setItem(inv, slot2, item1);
			if (item1->getPetId() > 0) {
				player->getPets()->getPet(item1->getPetId())->setInventorySlot((int8_t) slot2);
			}
			if (item2 != nullptr && item2->getPetId() > 0) {
				player->getPets()->getPet(item2->getPetId())->setInventorySlot((int8_t) slot1);
			}
			InventoryPacket::moveItem(player, inv, slot1, slot2);
		}
	}
	if ((slot1 < 0 && -slot1 == EquipSlots::Weapon) || (slot2 < 0 && -slot2 == EquipSlots::Weapon)) {
		player->getActiveBuffs()->stopBooster();
		player->getActiveBuffs()->stopCharge();
		player->getActiveBuffs()->stopBulletSkills();
	}
	// Check if the label ring changed, so we can update the look of the pet.
	if ((slot1 < 0 && -slot1 - 100 == EquipSlots::PetLabelRing1) || (slot2 < 0 && -slot2 - 100 == EquipSlots::PetLabelRing1)) {
		if (Pet *pet = player->getPets()->getSummoned(0)) {
			PetsPacket::changeName(player, pet);
		}
	}
	if ((slot1 < 0 && -slot1 - 100 == EquipSlots::PetLabelRing2) || (slot2 < 0 && -slot2 - 100 == EquipSlots::PetLabelRing2)) {
		if (Pet *pet = player->getPets()->getSummoned(1)) {
			PetsPacket::changeName(player, pet);
		}
	}
	if ((slot1 < 0 && -slot1 - 100 == EquipSlots::PetLabelRing3) || (slot2 < 0 && -slot2 - 100 == EquipSlots::PetLabelRing3)) {
		if (Pet *pet = player->getPets()->getSummoned(2)) {
			PetsPacket::changeName(player, pet);
		}
	}
	if (slot1 < 0 || slot2 < 0) {
		InventoryPacket::updatePlayer(player);
	}
}

void InventoryHandler::useItem(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int16_t slot = packet.get<int16_t>();
	int32_t itemid = packet.get<int32_t>();
	if (player->getStats()->getHp() == 0 || player->getInventory()->getItemAmountBySlot(Inventories::UseInventory, slot) == 0) {
		// Hacking
		return;
	}

	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (item == nullptr || item->getId() != itemid) {
		// Hacking
		return;
	}

	Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);
	Inventory::useItem(player, itemid);
}

void InventoryHandler::cancelItem(Player *player, PacketReader &packet) {
	int32_t itemid = packet.get<int32_t>();
	Buffs::endBuff(player, itemid);
}

void InventoryHandler::useSkillbook(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int16_t slot = packet.get<int16_t>();
	int32_t itemid = packet.get<int32_t>();

	Item *it = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (it == nullptr || it->getId() != itemid) {
		// Hacking
		return;
	}
	if (!ItemDataProvider::Instance()->skillItemExists(itemid)) {
		// Hacking
		return;
	}

	int32_t skillid = 0;
	uint8_t newMaxLevel = 0;
	bool use = false;
	bool succeed = false;

	vector<Skillbook> *item = ItemDataProvider::Instance()->getItemSkills(itemid);
	Skillbook s;

	for (size_t i = 0; i < item->size(); i++) {
		s = (*item)[i];
		skillid = s.skillid;
		newMaxLevel = s.maxlevel;
		if (GameLogicUtilities::itemSkillMatchesJob(skillid, player->getStats()->getJob())) {
			// Make sure the skill is for the person's job
			if (player->getSkills()->getSkillLevel(skillid) >= s.reqlevel) {
				// I know the multiple levels of if aren't necessary, but they're large/verbose comparisons
				if (player->getSkills()->getMaxSkillLevel(skillid) < newMaxLevel) {
					// Don't want to break up this vertical spacing
					if (Randomizer::Instance()->randShort(99) < s.chance) {
						player->getSkills()->setMaxSkillLevel(skillid, newMaxLevel);
						succeed = true;
					}
					Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);
					break;
				}
			}
		}
	}

	if (skillid != 0) {
		InventoryPacket::useSkillbook(player, skillid, newMaxLevel, true, succeed);
	}
}

void InventoryHandler::useChair(Player *player, PacketReader &packet) {
	int32_t chairid = packet.get<int32_t>();
	player->setChair(chairid);
	InventoryPacket::sitChair(player, chairid);
}

void InventoryHandler::handleChair(Player *player, PacketReader &packet) {
	int16_t chair = packet.get<int16_t>();
	Map *map = Maps::getMap(player->getMap());
	if (chair == -1) {
		if (player->getChair() != 0) {
			player->setChair(0);
		}
		else {
			map->playerSeated(player->getMapChair(), nullptr);
			player->setMapChair(0);
		}
		InventoryPacket::stopChair(player);
	}
	else { // Map chair
		if (map->seatOccupied(chair)) {
			InventoryPacket::stopChair(player, false);
		}
		else {
			map->playerSeated(chair, player);
			player->setMapChair(chair);
			InventoryPacket::sitMapChair(player, chair);
		}
	}
}

void InventoryHandler::useSummonBag(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int16_t slot = packet.get<int16_t>();
	int32_t itemid = packet.get<int32_t>();

	if (!ItemDataProvider::Instance()->summonBagExists(itemid)) {
		// Most likely hacking
		return;
	}

	Item *inventoryitem = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (inventoryitem == nullptr || inventoryitem->getId() != itemid) {
		// Hacking
		return;
	}

	Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);

	vector<SummonBag> *item = ItemDataProvider::Instance()->getItemSummons(itemid);
	SummonBag s;
	for (size_t i = 0; i < item->size(); i++) {
		s = (*item)[i];
		if (Randomizer::Instance()->randInt(99) < s.chance) {
			if (MobDataProvider::Instance()->mobExists(s.mobid)) {
				Maps::getMap(player->getMap())->spawnMob(s.mobid, player->getPos());
			}
			else {
				std::stringstream x;
				x << "[SUMMON BAG] Summon bag tries to summon mob without info. ID: " << itemid << ", MobID: " << s.mobid;
				ChannelServer::Instance()->log(LogTypes::Warning, x.str());
			}
		}
	}
}

void InventoryHandler::useReturnScroll(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int16_t slot = packet.get<int16_t>();
	int32_t itemid = packet.get<int32_t>();

	Item *it = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (it == nullptr|| it->getId() != itemid) {
		// Hacking
		return;
	}
	ConsumeInfo *info = ItemDataProvider::Instance()->getConsumeInfo(itemid);
	if (info == nullptr) {
		// Probably hacking
		return;
	}
	Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);
	int32_t map = info->moveTo;
	player->setMap(map == Maps::NoMap ? Maps::getMap(player->getMap())->getReturnMap() : map);
}

void InventoryHandler::useScroll(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int16_t slot = packet.get<int16_t>();
	int16_t eslot = packet.get<int16_t>();
	bool wscroll = (packet.get<int16_t>() == 2);
	bool legendarySpirit = packet.getBool();

	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	Item *equip = player->getInventory()->getItem(Inventories::EquipInventory, eslot);
	if (item == nullptr || equip == nullptr) {
		// Most likely hacking
		return;
	}

	int32_t itemid = item->getId();
	int8_t succeed = -1;
	bool cursed = false;
	ItemDataProvider::Instance()->scrollItem(itemid, equip, succeed, cursed, wscroll);

	if (succeed != -1) {
		if (wscroll) {
			Inventory::takeItem(player, Items::WhiteScroll, 1);
		}
		Inventory::takeItemSlot(player, Inventories::UseInventory, slot, 1);
		InventoryPacket::useScroll(player, succeed, cursed, legendarySpirit);
		if (!cursed) {
			player->getStats()->setEquip(eslot, equip);
			InventoryPacket::addNewItem(player, Inventories::EquipInventory, eslot, equip, true);
		}
		else {
			InventoryPacket::moveItem(player, Inventories::EquipInventory, eslot, 0);
			player->getInventory()->deleteItem(Inventories::EquipInventory, eslot);
		}
		InventoryPacket::updatePlayer(player);
	}
	else {
		if (legendarySpirit) {
			InventoryPacket::useScroll(player, succeed, cursed, legendarySpirit);
		}
		InventoryPacket::blankUpdate(player);
	}
}

void InventoryHandler::useCashItem(Player *player, PacketReader &packet) {
	packet.get<int8_t>(); // Type
	packet.skipBytes(1);
	int32_t itemid = packet.get<int32_t>();

	if (!player->getInventory()->getItemAmount(itemid)) {
		// Hacking
		return;
	}

	bool used = false;
	if (GameLogicUtilities::getItemType(itemid) == Items::Types::WeatherCash) {
		string message = packet.getString();
		if (!player->updateTickCount(packet.get<int32_t>())) {
			// Tickcount was the same or less than 100 of the difference.
			return;
		}
		if (message.length() <= 35) {
			Map *map = Maps::getMap(player->getMap());
			message = player->getName() + " 's message : " + message;
			used = map->createWeather(player, false, Items::WeatherTime, itemid, message);
		}
	}
	else if (GameLogicUtilities::getItemType(itemid) == Items::Types::CashPetFood) {
		// Pet food.
		Pet *pet = player->getPets()->getSummoned(0);
		if (pet != nullptr) {
			if (pet->getFullness() < Stats::MaxFullness) {
				PetsPacket::showAnimation(player, pet, 1);
				pet->modifyFullness(Stats::MaxFullness, false);
				pet->addCloseness(100); // All cash pet food gives 100 closeness.
				used = true;
			}
		}
	}
	else {
		switch (itemid) {
			case Items::TeleportRock:
			case Items::TeleportCoke:
			case Items::VipRock: // Only occurs when you actually try to move somewhere
				used = handleRockTeleport(player, itemid, packet);
				break;
			case Items::FirstJobSpReset:
			case Items::SecondJobSpReset:
			case Items::ThirdJobSpReset:
			case Items::FourthJobSpReset: {
				int32_t toskill = packet.get<int32_t>();
				int32_t fromskill = packet.get<int32_t>();
				if (!player->getSkills()->addSkillLevel(fromskill, -1, true)) {
					// Hacking
					return;
				}
				if (!player->getSkills()->addSkillLevel(toskill, 1, true)) {
					// Hacking
					return;
				}
				used = true;
				break;
			}
			case Items::ApReset: {
				int32_t tostat = packet.get<int32_t>();
				int32_t fromstat = packet.get<int32_t>();
				player->getStats()->addStat(tostat, 1, true);
				player->getStats()->addStat(fromstat, -1, true);
				used = true;
				break;
			}
			case Items::Megaphone: {
				string msg = player->getMedalName() + " : " + packet.getString();
				InventoryPacket::showMegaphone(player, msg);
				used = true;
				break;
			}
			case Items::SuperMegaphone: {
				string msg = player->getMedalName() + " : " + packet.getString();
				bool whisper = packet.getBool();
				InventoryPacket::showSuperMegaphone(player, msg, whisper);
				used = true;
				break;
			}
			case Items::DiabloMessenger:
			case Items::Cloud9Messenger:
			case Items::LoveholicMessenger: {
				string msg = packet.getString();
				string msg2 = packet.getString();
				string msg3 = packet.getString();
				string msg4 = packet.getString();

				InventoryPacket::showMessenger(player, msg, msg2, msg3, msg4, packet.getBuffer(), packet.getBufferLength(), itemid);
				used = true;
				break;
			}
			case Items::ItemMegaphone: {
				string msg = player->getMedalName() + " : " + packet.getString();
				bool whisper = packet.getBool();
				Item *item = nullptr;
				if (packet.getBool()) {
					int8_t inv = (int8_t) packet.get<int32_t>();
					int16_t slot = (int16_t) packet.get<int32_t>();
					item = player->getInventory()->getItem(inv, slot);
					if (item == nullptr) {
						// Hacking
						return;
					}
				}
				InventoryPacket::showItemMegaphone(player, msg, whisper, item);
				used = true;
				break;
			}
			case Items::ArtMegaphone: {
				int8_t lines = packet.get<int8_t>();
				if (lines < 1 || lines > 3) {
					// Hacking
					return;
				}
				string text[3];
				for (int8_t i = 0; i < lines; i++) {
					text[i] = player->getMedalName() + " : " + packet.getString();
				}
				bool whisper = packet.getBool();
				InventoryPacket::showTripleMegaphone(player, lines, text[0], text[1], text[2], whisper);
				used = true;
				break;
			}
			case Items::PetNameTag: {
				string name = packet.getString();
				PetHandler::changeName(player, name);
				used = true;
				break;
			}
			case Items::ItemNameTag: {
				int16_t slot = packet.get<int16_t>();
				if (slot != 0) {
					Item *item = player->getInventory()->getItem(Inventories::EquipInventory, slot);
					if (item == nullptr) {
						// Hacking or failure, dunno
						return;
					}
					item->setName(player->getName());
					InventoryPacket::addNewItem(player, Inventories::EquipInventory, slot, item, true);
					used = true;
				}
				break;
			}
			case Items::ItemLock: 
			case Items::ScissorsOfKarma: {
				int8_t inventory = (int8_t) packet.get<int32_t>();
				int16_t slot = (int16_t) packet.get<int32_t>();
				if (slot != 0) {
					Item *item = player->getInventory()->getItem(inventory, slot);
					if (item == nullptr || (itemid == Items::ItemLock && item->hasLock()) || item->hasTradeBlock() || (itemid == Items::ScissorsOfKarma && item->hasKarma())) {
						// Hacking or failure, dunno
						return;
					}
					ItemInfo *info = ItemDataProvider::Instance()->getItemInfo(item->getId());
					if (itemid == Items::ScissorsOfKarma && info->karmascissors) {
						// Hacking
						return;
					}

					switch (itemid) {
						case Items::ItemLock: item->setLock(true); break;
						case Items::ScissorsOfKarma: item->setKarma(true); break;
					}

					InventoryPacket::addNewItem(player, inventory, slot, item, true);
					used = true;
				}
				break;
			}
			case Items::MapleTvMessenger: 
			case Items::Megassenger: {
				bool hasreceiver = (packet.get<int8_t>() == 3);
				bool show_whisper = (itemid == Items::Megassenger ? packet.getBool() : false);
				Player *receiver = PlayerDataProvider::Instance()->getPlayer(packet.getString());
				int32_t time = 15;

				if ((hasreceiver && receiver != nullptr) || (!hasreceiver && receiver == nullptr)) {
					string msg = packet.getString();
					string msg2 = packet.getString();
					string msg3 = packet.getString();
					string msg4 = packet.getString();
					string msg5 = packet.getString();
					if (!player->updateTickCount(packet.get<int32_t>())) {
						// Tickcount was the same or less than 100 of the difference.
						return;
					}
					MapleTvs::Instance()->addMessage(player, receiver, msg, msg2, msg3, msg4, msg5, itemid - (itemid == Items::Megassenger ? 3 : 0), time);

					if (itemid == Items::Megassenger) {
						InventoryPacket::showSuperMegaphone(player, player->getMedalName() + " : " + msg + msg2 + msg3 + msg4 + msg5, show_whisper);
					}
					used = true;
				}
				break;
			}
			case Items::MapleTvStarMessenger: 
			case Items::StarMegassenger: {
				int32_t time = 30;
				bool show_whisper = (itemid == Items::StarMegassenger ? packet.getBool() : false);
				string msg = packet.getString();
				string msg2 = packet.getString();
				string msg3 = packet.getString();
				string msg4 = packet.getString();
				string msg5 = packet.getString();
				if (!player->updateTickCount(packet.get<int32_t>())) {
					// Tickcount was the same or less than 100 of the difference.
					return;
				}
				MapleTvs::Instance()->addMessage(player, nullptr, msg, msg2, msg3, msg4, msg5, itemid - (itemid == Items::StarMegassenger ? 3 : 0), time);

				if (itemid == Items::StarMegassenger) {
					InventoryPacket::showSuperMegaphone(player, player->getMedalName() + " : " + msg + msg2 + msg3 + msg4 + msg5, show_whisper);
				}
				used = true;
				break;
			}
			case Items::MapleTvHeartMessenger:
			case Items::HeartMegassenger: {
				bool show_whisper = (itemid == Items::HeartMegassenger ? packet.getBool() : false);
				string name = packet.getString();
				Player *receiver = PlayerDataProvider::Instance()->getPlayer(name);
				int32_t time = 60;

				if (receiver != nullptr) {
					string msg = packet.getString();
					string msg2 = packet.getString();
					string msg3 = packet.getString();
					string msg4 = packet.getString();
					string msg5 = packet.getString();
					if (!player->updateTickCount(packet.get<int32_t>())) {
						// Tickcount was the same or less than 100 of the difference.
						return;
					}
					MapleTvs::Instance()->addMessage(player, receiver, msg, msg2, msg3, msg4, msg5, itemid - (itemid == Items::HeartMegassenger ? 3 : 0), time);
					if (itemid == Items::HeartMegassenger) {
						InventoryPacket::showSuperMegaphone(player, player->getMedalName() + " : " + msg + msg2 + msg3 + msg4 + msg5, show_whisper);
					}
					used = true;
				}
				break;
			}
			case Items::BronzeSackOfMesos:
			case Items::SilverSackOfMesos:
			case Items::GoldSackOfMesos: {
				int32_t mesos = ItemDataProvider::Instance()->getMesoBonus(itemid);
				if (!player->getInventory()->modifyMesos(mesos)) {
					InventoryPacket::sendMesobagFailed(player);
				}
				else {
					InventoryPacket::sendMesobagSucceed(player, mesos);
					used = true;
				}
				break;
			}
			case Items::Chalkboard:
			case Items::Chalkboard2: {
				string msg = packet.getString();
				player->setChalkboard(msg);
				InventoryPacket::sendChalkboardUpdate(player, msg);
				break;
			}
			case Items::FungusScroll:
			case Items::OinkerDelight:
			case Items::ZetaNightmare:
				Inventory::useItem(player, itemid);
				used = true;
				break;
			case Items::ViciousHammer: {
				int8_t inventory = (int8_t) packet.get<int32_t>(); // How pointless...
				int16_t slot = (int16_t) packet.get<int32_t>();
				Item *f = player->getInventory()->getItem(inventory, slot);
				if (f == nullptr || f->getHammers() == Items::MaxHammers) {
					// Hacking, probably
					return;
				}
				f->incHammers();
				f->incSlots();
				InventoryPacket::sendHammerSlots(player, f->getHammers());
				player->getInventory()->setHammerSlot(slot);
				used = true;
				break;
			}
			case Items::CongratulatorySong:
				if (!player->updateTickCount(packet.get<int32_t>())) {
					// Tickcount was the same or less than 100 of the difference.
					return;
				}
				used = Maps::getMap(player->getMap())->playJukebox(player, itemid, 60 * 5); // 5 minutes is enough!
				break;
			case Items::KoreanKite:
			case Items::HeartBalloon:
			case Items::GraduationBanner:
			case Items::AdmissionBanner: {
				string message = packet.getString();
				if (!player->updateTickCount(packet.get<int32_t>())) {
					// Tickcount was the same or less than 100 of the difference.
					return;
				}

				used = Maps::getMap(player->getMap())->createKite(player, itemid, message);
				if (!used) {
					InventoryPacket::sendCannotFlyHere(player);
				}

				break;
			}
			default: {
				packet.reset();
				std::stringstream x;
				x << "Unknown cash item! ItemID: " << itemid << "; Player ID: " << player->getId() << "; Packet: " << packet;
				ChannelServer::Instance()->log(LogTypes::Info, x.str());
				break;
			}
		}
	}
	if (used) {
		Inventory::takeItem(player, itemid, 1);
	}
	else {
		InventoryPacket::blankUpdate(player);
	}
}

void InventoryHandler::useItemEffect(Player *player, PacketReader &packet) {
	int32_t itemid = packet.get<int32_t>();
	if (player->getInventory()->getItemAmount(itemid) == 0) {
		// Hacking
		return;
	}
	player->setItemEffect(itemid);
	InventoryPacket::useItemEffect(player, itemid);
}

void InventoryHandler::handleRockFunctions(Player *player, PacketReader &packet) {
	int8_t mode = packet.get<int8_t>();
	int8_t type = packet.get<int8_t>();

	enum Modes {
		Remove = 0x00,
		Add = 0x01
	};

	if (mode == Remove) {
		int32_t map = packet.get<int32_t>();
		player->getInventory()->delRockMap(map, type);
	}
	else if (mode == Add) {
		int32_t map = player->getMap();
		Map *m = Maps::getMap(map);
		if (m->canVip() && m->getContinent() != 0) {
			player->getInventory()->addRockMap(map, type);
		}
		else {
			// Hacking, the client doesn't allow this to occur
			InventoryPacket::sendRockError(player, InventoryPacket::RockErrors::CannotSaveMap, type);
		}
	}
}

bool InventoryHandler::handleRockTeleport(Player *player, int32_t itemid, PacketReader &packet) {
	if (itemid == Items::SpecialTeleportRock) {
		packet.skipBytes(5);
	}

	int8_t type = InventoryPacket::RockTypes::Regular;
	switch (itemid) {
		case Items::VipRock: type = InventoryPacket::RockTypes::Vip; break;
		case Items::SpecialTeleportRock: type = packet.get<int8_t>(); break;
	}
	bool used = false;
	int8_t mode = packet.get<int8_t>();
	int32_t targetmapid = -1;

	enum Modes {
		PresetMap = 0x00,
		Ign = 0x01
	};

	if (mode == PresetMap) {
		targetmapid = packet.get<int32_t>();
		if (!player->getInventory()->ensureRockDestination(targetmapid)) {
			// Hacking
			return false;
		}
	}
	else if (mode == Ign) {
		string tname = packet.getString();
		Player *target = PlayerDataProvider::Instance()->getPlayer(tname);
		if (target != nullptr && target != player) {
			targetmapid = target->getMap();
		}
		else if (target == nullptr) {
			InventoryPacket::sendRockError(player, InventoryPacket::RockErrors::DifficultToLocate, type);
		}
		else if (target == player) {
			// Hacking
			return false;
		}
	}
	if (targetmapid != -1) {
		Map *destination = Maps::getMap(targetmapid);
		Map *origin = Maps::getMap(player->getMap());
		if (!destination->canVip()) {
			InventoryPacket::sendRockError(player, InventoryPacket::RockErrors::CannotGo, type);
		}
		else if (!origin->canVip()) {
			InventoryPacket::sendRockError(player, InventoryPacket::RockErrors::CannotGo, type);
		}
		else if (player->getMap() == targetmapid) {
			InventoryPacket::sendRockError(player, InventoryPacket::RockErrors::AlreadyThere, type);
		}
		else if (type == InventoryPacket::RockTypes::Regular && destination->getContinent() != origin->getContinent()) {
			InventoryPacket::sendRockError(player, InventoryPacket::RockErrors::CannotGo, type);
		}
		else if (player->getStats()->getLevel() < 7 && origin->getContinent() == 0 && destination->getContinent() != 0) {
			InventoryPacket::sendRockError(player, InventoryPacket::RockErrors::NoobsCannotLeaveMapleIsland, type);
		}
		else {
			player->setMap(targetmapid);
			used = true;
		}
	}
	if (itemid == Items::SpecialTeleportRock) {
		if (used) {
			Inventory::takeItem(player, itemid, 1);
		}
		else {
			InventoryPacket::blankUpdate(player);
		}
	}
	return used;
}

void InventoryHandler::handleHammerTime(Player *player) {
	if (!player->getInventory()->isHammering()) {
		// Hacking
		return;
	}
	int16_t hammerslot = player->getInventory()->getHammerSlot();
	Item *item = player->getInventory()->getItem(Inventories::EquipInventory, hammerslot);
	InventoryPacket::sendHammerUpdate(player);
	InventoryPacket::sendHulkSmash(player, hammerslot, item);
	player->getInventory()->setHammerSlot(-1);
}

void InventoryHandler::handleRewardItem(Player *player, PacketReader &packet) {
	int16_t slot = packet.get<int16_t>();
	int32_t itemid = packet.get<int32_t>();
	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (item == nullptr || item->getId() != itemid) {
		// Hacking or hacking failure
		InventoryPacket::blankUpdate(player); // We don't want stuck players, do we?
		return;
	}

	ItemRewardInfo *reward = ItemDataProvider::Instance()->getRandomReward(itemid);
	if (reward == nullptr) {
		// Hacking or no information in the database
		InventoryPacket::blankUpdate(player); // We don't want stuck players, do we?
		return;
	}

	Inventory::takeItem(player, itemid, 1);
	Item *rewardItem = new Item(reward->rewardid, reward->quantity);
	Inventory::addItem(player, rewardItem, true);
	InventoryPacket::sendRewardItemAnimation(player, itemid, reward->effect);
}

void InventoryHandler::handleScriptItem(Player *player, PacketReader &packet) {
	if (player->getNpc() != nullptr || player->getShop() != 0 || player->getTradeId() != 0) {
		// Player is busy :O! Hack
		InventoryPacket::blankUpdate(player); // We don't want stuck players, do we?
		return;
	}

	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int16_t slot = packet.get<int16_t>();
	int32_t itemid = packet.get<int32_t>();

	Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
	if (item == nullptr || item->getId() != itemid) {
		// Hacking or hacking failure
		InventoryPacket::blankUpdate(player); // We don't want stuck players, do we?
		return;
	}

	string scriptName = ScriptDataProvider::Instance()->getItemScript(itemid);
	if (scriptName == "") {
		// Hacking or no script for item found.
		InventoryPacket::blankUpdate(player); // We don't want stuck players, do we?
		return;
	}

	int32_t npcid = ItemDataProvider::Instance()->getItemNpc(itemid);

	// Lets run the NPC!
	Npc *npc = new Npc(npcid, player, scriptName);
	if (!npc->checkEnd()) {
		// NPC is running/script found!
		// Delete the item used.
		Inventory::takeItem(player, itemid, 1);
		npc->run();
	}
	else {
		// NPC didn't ran/no script found O.o! Lets unstuck the player.
		InventoryPacket::blankUpdate(player); // We don't want stuck players, do we?
	}
}