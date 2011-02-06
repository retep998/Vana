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
#include "PlayerHandler.h"
#include "CashDataProvider.h"
#include "CashItem.h"
#include "CashServer.h"
#include "EquipDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "ItemConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "ItemDataProvider.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "PacketReader.h"
#include "Randomizer.h"
#include "TimeUtilities.h"
#include "WorldServerConnectPacket.h"

void PlayerHandler::handleOperation(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case OperationTypes::BuyItemBestItems:
		case OperationTypes::BuyItem: handleBuyItem(player, packet); break;
		case OperationTypes::WishList: handleWishList(player, packet); break;
		case OperationTypes::Gift: 
		case OperationTypes::GiftPackage: handleGift(player, packet); break;
		case OperationTypes::MoveItemToInventory: handleMoveItemToInventory(player, packet); break;
		case OperationTypes::MoveItemToStorage: handleMoveItemToStorage(player, packet); break;
		case OperationTypes::BuyPackage: handleBuyPackage(player, packet); break;
		case OperationTypes::InventoryExpand: handleExpandInventory(player, packet); break;
		case OperationTypes::StorageExpand: handleExpandStorage(player, packet); break;
		case OperationTypes::BuyFriendRing: handleBuyFriendRing(player, packet); break;
		case OperationTypes::BuyItemMesos: handleBuyItemMesos(player, packet); break;
		default: {
			packet.reset();
			std::stringstream str;
			str << "Player " << player->getId() << "; Player used an unknown method of doing something in the Cash Shop. Mode: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int16_t)type;
			CashServer::Instance()->log(LogTypes::Info, str.str());
			PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
			break;
		}
	}
}

void PlayerHandler::handleBuyItem(Player *player, PacketReader &packet) {
	packet.skipBytes(1); // Unk
	int8_t buyMode = (int8_t)packet.get<int32_t>();
	int32_t serial = packet.get<int32_t>();
	CashItemInfo *info = CashDataProvider::Instance()->getItemInfo(serial);
	if (info == nullptr || (info->gender != -1 && info->gender != player->getGender())) {
		// Hackz. Unstuck the sucker...
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
		return;
	}
	
	ModifiedCashItem *item = CashDataProvider::Instance()->getModifiedCashItemData(serial);
	if (item != nullptr && item->showUp && item->discountPrice > 0) {
		if (!takeCash(player, buyMode, item->discountPrice)) {
			PlayerPacket::showNX(player);
			return;
		}
	}
	else {
		if (!takeCash(player, buyMode, info->price)) {
			PlayerPacket::showNX(player);
			return;
		}		
	}
	
	buyCashItem(player, info, buyMode, true);
	CashDataProvider::Instance()->logBoughtItem(player->getUserId(), player->getId(), serial);
	WorldServerConnectPacket::reloadBestItems(CashServer::Instance()->getWorldConnection());

	PlayerPacket::showNX(player);
}

void PlayerHandler::handleBuyPackage(Player *player, PacketReader &packet) {
	packet.skipBytes(1); // Unk
	int8_t mode = (int8_t)packet.get<int32_t>();
	int32_t serial = packet.get<int32_t>();
	CashItemInfo *info = CashDataProvider::Instance()->getItemInfo(serial);

	if (info == nullptr || !CashDataProvider::Instance()->packageExists(info->itemid)) {
		// Hackz. Unstuck the sucker...
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
	}
	else {
		ModifiedCashItem *item = CashDataProvider::Instance()->getModifiedCashItemData(serial);
		if ((item != nullptr && item->showUp && item->discountPrice > 0 && !takeCash(player, mode, item->discountPrice)) || !takeCash(player, mode, info->price)) {
			PlayerPacket::showNX(player);
		}
		else {
			vector<CashItemInfo> *items = CashDataProvider::Instance()->getPackageItems(info->itemid);
			vector<CashItem *> itemList;
			for (size_t i = 0; i < items->size(); i++) {
				buyCashItemPackage(player, &items->at(i), mode, itemList); // Todo: get cash package packet
			}
			
			PlayerPacket::sendBoughtPackage(player, &itemList);

			CashDataProvider::Instance()->logBoughtItem(player->getUserId(), player->getId(), serial);
			WorldServerConnectPacket::reloadBestItems(CashServer::Instance()->getWorldConnection());

			PlayerPacket::showNX(player);
		}
	}
}

void PlayerHandler::handleBuyItemMesos(Player *player, PacketReader &packet) {
	int32_t serial = packet.get<int32_t>();
	int32_t itemid = -1;
	switch (serial) {
		case CashItems::Serials::GoldenBell: itemid = Items::GoldenBell; break;
		case CashItems::Serials::BeginnersShopingGuide: itemid = Items::BeginnersShoppingGuide; break;
		case CashItems::Serials::RedRibbon: itemid = Items::RedRibbon; break;
	}
	if (itemid == -1) {
		std::stringstream str;
		str << "Player " << player->getId() << "; Player tried to buy a non-cash item with the buy cash item packet OR serial isn't handled as a non-cash item. Serial: " << serial;
		CashServer::Instance()->log(LogTypes::Info, str.str());
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
	}
	else if (player->getInventory()->getItemAmount(itemid) >= 1) {
		// Player already got this item.
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
	}
	else if (player->getInventory()->getMesos() < 1) {
		// HAHA n00b!
		PlayerPacket::showFailure(player, ErrorMessages::NotEnoughMesos);
	}
	else if (!player->getInventory()->hasOpenSlotsFor(itemid, 1)) {
		PlayerPacket::showFailure(player, ErrorMessages::CheckInventoryFull);
	}
	else {
		player->getInventory()->modifyMesos(-1);
		Inventory::addNewItem(player, itemid, 1);
	}
}

void PlayerHandler::handleBuyFriendRing(Player *player, PacketReader &packet) {
	int32_t birthDate = packet.get<int32_t>();
	int32_t method = packet.get<int32_t>();
	int32_t serial = packet.get<int32_t>();
	string receiver = packet.getString();
	string message = packet.getString();

	PlayerPacket::showFailure(player, ErrorMessages::UnknownError); // TODO: Handle this.
}

void PlayerHandler::handleWishList(Player *player, PacketReader &packet) {
	player->getInventory()->clearWishList(); // Else it would append older ones too.
	int32_t serial = 0;
	for (size_t i = 0; i < 10; i++) {
		serial = packet.get<int32_t>();
		if (serial != 0) { // We don't want empty wishlist things....
			if (CashDataProvider::Instance()->itemExists(serial)) {
				player->getInventory()->addWishListItem(serial);
			}
			else {
				CashServer::Instance()->log(LogTypes::Warning, "Player tried to add a non-existing cash item serial to whishlist. PlayerID: " + boost::lexical_cast<string>(player->getId()) + ", Serial: " + boost::lexical_cast<string>(serial));
				break;
			}
		}
	}

	PlayerPacket::showWishList(player, true);
}

void PlayerHandler::handleGift(Player *player, PacketReader &packet) {
	int32_t dateOfBirth = packet.get<int32_t>();
	int32_t serial = packet.get<int32_t>();
	string to = packet.getString();
	string message = packet.getString();

	if (dateOfBirth != player->getBirthDate()) {
		PlayerPacket::showFailure(player, ErrorMessages::CheckYourBirthdayCode);
		return;
	}

	CashItemInfo *info = CashDataProvider::Instance()->getItemInfo(serial);
	if (info == nullptr) {
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
		return;
	}
	
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT world_id, userid, id, `name` FROM characters WHERE name = " << mysqlpp::quote << to << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();
	if (res.num_rows() == 0) {
		PlayerPacket::showFailure(player, ErrorMessages::CheckRecipientName);
		return;
	}
		
	mysqlpp::Row row = res[0];
	if (atoi(row["world_id"]) != CashServer::Instance()->getWorld()) {
		PlayerPacket::showFailure(player, ErrorMessages::InvalidRecipient); // I don't know this error, I guess this one.
		return;
	}

	// Lets check where the character is at the moment
	Player *receiver = PlayerDataProvider::Instance()->getPlayer(to);
	ModifiedCashItem *item = CashDataProvider::Instance()->getModifiedCashItemData(serial);
	if (receiver != nullptr) {
		if (CashDataProvider::Instance()->packageExists(info->itemid)) {
			if ((item != nullptr && item->showUp && item->discountPrice > 0 && !takeCash(player, BuyModes::NxPrepaid, item->discountPrice)) || !takeCash(player, BuyModes::NxPrepaid, info->price)) {
				PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
				return;
			}
			else {
				vector<CashItemInfo> *items = CashDataProvider::Instance()->getPackageItems(info->itemid);
				for (size_t i = 0; i < items->size(); i++) {
					buyCashItem(receiver, &items->at(i), 0, true, player->getName(), message);
				}
				CashDataProvider::Instance()->logBoughtItem(player->getUserId(), player->getId(), serial);
				WorldServerConnectPacket::reloadBestItems(CashServer::Instance()->getWorldConnection());
			}
		}
		else {
			if ((item != nullptr && item->showUp && item->discountPrice > 0 && !takeCash(player, BuyModes::NxPrepaid, item->discountPrice)) || !takeCash(player, BuyModes::NxPrepaid, info->price)) {
				PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
				return;
			}
			else {
				buyCashItem(receiver, info, 0, true, player->getName(), message);
				CashDataProvider::Instance()->logBoughtItem(player->getUserId(), player->getId(), serial);
				WorldServerConnectPacket::reloadBestItems(CashServer::Instance()->getWorldConnection());
			}
		}
		
	}
	else {
		if (GameLogicUtilities::isPet(info->itemid)) {
			// Todo: special code for pets!
			PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
			return;
		}
		time_t expirationTime;
		if (CashDataProvider::Instance()->packageExists(info->itemid)) {
			if ((item != nullptr && item->showUp && item->discountPrice > 0 && !takeCash(player, BuyModes::NxPrepaid, item->discountPrice)) || !takeCash(player, BuyModes::NxPrepaid, info->price)) {
				PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
				return;
			}
			else {
				vector<CashItemInfo> *items = CashDataProvider::Instance()->getPackageItems(info->itemid);
				for (size_t i = 0; i < items->size(); i++) {
					expirationTime = items->at(i).expiration_days != 0 ? TimeUtilities::timeToTick(TimeUtilities::addDaysToTime(items->at(i).expiration_days)) : Items::NoExpiration;
					query << "INSERT INTO storage_cash VALUES (NULL, "
						<< atoi(row["userid"]) << ", "
						<< (int16_t) atoi(row["world_id"]) << ", "
						<< items->at(i).itemid << ", "
						<< items->at(i).quantity << ", "
						<< mysqlpp::quote << player->getName() << ", "
						<< 0 << ", "
						<< expirationTime << ")";
					query.exec();

					query << "INSERT INTO character_cashshop_gifts VALUES ("
						<< atoi(row["id"]) << ", "
						<< query.insert_id() << ", "
						<< items->at(i).itemid << ", "
						<< mysqlpp::quote << player->getName() << ", "
						<< mysqlpp::quote << message << ")";
					query.exec();
				}
				CashDataProvider::Instance()->logBoughtItem(player->getUserId(), player->getId(), serial);
				WorldServerConnectPacket::reloadBestItems(CashServer::Instance()->getWorldConnection());
			}
		}
		else {
			if ((item != nullptr && item->showUp && item->discountPrice > 0 && !takeCash(player, BuyModes::NxPrepaid, item->discountPrice)) || !takeCash(player, BuyModes::NxPrepaid, info->price)) {
				PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
			}
			else {
				expirationTime = info->expiration_days != 0 ? TimeUtilities::timeToTick(TimeUtilities::addDaysToTime(info->expiration_days)) : Items::NoExpiration;
				query << "INSERT INTO storage_cash VALUES (NULL, "
					<< atoi(row["userid"]) << ", "
					<< (int16_t) atoi(row["world_id"]) << ", "
					<< info->itemid << ", "
					<< info->quantity << ", "
					<< mysqlpp::quote << player->getName() << ", "
					<< 0 << ", "
					<< expirationTime << ")";
				query.exec();

				query << "INSERT INTO character_cashshop_gifts VALUES ("
					<< atoi(row["id"]) << ", "
					<< query.insert_id() << ", "
					<< info->itemid << ", "
					<< mysqlpp::quote << player->getName() << ", "
					<< mysqlpp::quote << message << ")";
				query.exec();

				CashDataProvider::Instance()->logBoughtItem(player->getUserId(), player->getId(), serial);
				WorldServerConnectPacket::reloadBestItems(CashServer::Instance()->getWorldConnection());
			}
		}
	}
	PlayerPacket::sendSentItem(player, (string)row["name"], serial, item == nullptr ? info->price : item->discountPrice); // TODO
	PlayerPacket::showNX(player);
}

void PlayerHandler::handleMoveItemToInventory(Player *player, PacketReader &packet) {
	int64_t id = packet.get<int64_t>();
	int16_t unk = packet.get<int16_t>();
	int8_t unk2 = packet.get<int8_t>();

	CashItem *item = player->getStorage()->getCashItem(id);
	if (item == nullptr) {
		// Hack or w/e. Unstuck
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
		return;
	}
	
	Item *realItem = new Item(item->getItemId(), item->getAmount());
	if (GameLogicUtilities::isEquip(item->getItemId())) {
		EquipDataProvider::Instance()->setEquipStats(realItem, false);
	}
	realItem->setCashId(item->getId());
	realItem->setName(item->getName());
	realItem->setExpirationTime(item->getExpirationTime());
	realItem->setPetId(item->getPetId());
	Inventory::addItem(player, realItem, GameLogicUtilities::isPet(item->getItemId()));
	player->getStorage()->takeCashItem(item->getId());
}

void PlayerHandler::handleMoveItemToStorage(Player *player, PacketReader &packet) {
	int64_t id = packet.get<int64_t>();
	int8_t inventory = packet.get<int8_t>();

	Item *info = player->getInventory()->getItem(inventory, id);
	if (info == nullptr) {
		// Hacking? Unstuck
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
		return;
	}

	CashItem *item = new CashItem(info, player->getUserId());
	player->getStorage()->addCashItem(item);
	Inventory::takeItemSlot(player, inventory, player->getInventory()->getSlotByCashId(inventory, id), item->getAmount());
	PlayerPacket::sendItemMovedToStorage(player, item);
}

void PlayerHandler::handleChangeNameRequest(Player *player, PacketReader &packet) {
	int32_t characterId = packet.get<int32_t>();
	int32_t birthDate = packet.get<int32_t>();

	if (player->getStats()->getLevel() < 20) {
		
	}
	else if (birthDate != player->getBirthDate()) {
		PlayerPacket::showFailure(player, ErrorMessages::CheckYourBirthdayCode);
	}
}

void PlayerHandler::handleChangeWorldRequest(Player *player, PacketReader &packet) {
	int32_t characterId = packet.get<int32_t>();
	int32_t birthDate = packet.get<int32_t>();

	if (player->getStats()->getLevel() < 20) {
		PlayerPacket::showFailure(player, ErrorMessages::CannotTransferWorldUnder20); // I guess...
	}
	else if (birthDate != player->getBirthDate()) {
		PlayerPacket::showFailure(player, ErrorMessages::CheckYourBirthdayCode);
	}
	else {
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError); // For later, if we are going to handle this.
	}
}

void PlayerHandler::handleExpandInventory(Player *player, PacketReader &packet) {
	packet.skipBytes(1); // Unk
	int8_t mode = (int8_t)packet.get<int32_t>();
	bool isItem = packet.getBool();
	int8_t inventory = -1;
	int8_t addSlots = 4;
	int32_t cost = 4000;

	if (isItem) {
		int32_t serial = packet.get<int32_t>();
		cost = 6400;
		addSlots = 8;
		switch (serial) {
			case CashItems::Serials::AddEquipSlots: inventory = Inventories::EquipInventory; break;
			case CashItems::Serials::AddUseSlots: inventory = Inventories::UseInventory; break;
			case CashItems::Serials::AddEtcSlots: inventory = Inventories::EtcInventory; break;
		}
	}
	else {
		inventory = packet.get<int8_t>();
	}

	uint8_t curSlots = player->getInventory()->getMaxSlots(inventory);

	if (curSlots == 96) {
		PlayerPacket::showFailure(player, ErrorMessages::CheckNumberOfItemsLimit);
	}
	else if (inventory < Inventories::EquipInventory || inventory > Inventories::CashInventory) {
		std::stringstream warning;
		warning << "Player: " << player->getName() << "; Player tried to buy an inventory expand but with an invalid inventory number.";
		CashServer::Instance()->log(LogTypes::Warning, warning.str());
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
	}
	else if (!takeCash(player, mode, cost)) {
		PlayerPacket::showNX(player);
	}
	else {
		player->getInventory()->addMaxSlots(inventory, addSlots / 4); // For amount of rows
		curSlots = player->getInventory()->getMaxSlots(inventory);
		PlayerPacket::sendIncreasedInventorySlots(player, inventory, curSlots);
		PlayerPacket::showNX(player);
	}
}

void PlayerHandler::handleExpandStorage(Player *player, PacketReader &packet) {
	packet.skipBytes(1); // Unk
	int8_t mode = packet.get<int8_t>();
	int8_t curSlots = player->getStorage()->getSlots();

	if (curSlots == 48) {
		PlayerPacket::showFailure(player, ErrorMessages::UnknownError);
	}
	else if (!takeCash(player, mode, 4000)) {
		PlayerPacket::showNX(player);
	}
	else {
		player->getStorage()->setSlots(curSlots + 4);
		curSlots = player->getStorage()->getSlots();
		PlayerPacket::sendIncreasedStorageSlots(player, curSlots);
		PlayerPacket::showNX(player);
	}
}

void PlayerHandler::handleRedeemCoupon(Player *player, PacketReader &packet) {
	packet.skipBytes(2); // Unk
	string couponCode = packet.getString();

	CouponInfo *coupon = CashDataProvider::Instance()->getCouponInfo(couponCode);
	if (coupon == nullptr) {
		PlayerPacket::showFailure(player, ErrorMessages::CheckCouponNumber);
	}
	else if (coupon->used) {
		PlayerPacket::showFailure(player, ErrorMessages::CouponAlreadyUsed);
	}
	else {
		vector<CashItem *> rewardedItems;
		if (coupon->items.size() > 0) {
			// Check if user has enough slots open
			for (size_t i = 0; i < coupon->items.size(); i++) {
				if (!player->getInventory()->hasOpenSlotsFor(coupon->items[i].itemid, coupon->items[i].quantity)) {
					PlayerPacket::showFailure(player, ErrorMessages::CheckInventoryFull);
					return;
				}
			}
			for (size_t i = 0; i < coupon->items.size(); i++) {
				CashItem *item = new CashItem();
				item->setAmount(coupon->items[i].quantity);
				item->setUserId(player->getUserId());
				item->setItemId(coupon->items[i].itemid);
				item->setPetId(0);
				if (coupon->items[i].expiration_days != 0) {
					// Only set the expiration time when the expiration days is higher than 0.
					item->setExpirationTime(TimeUtilities::timeToTick(TimeUtilities::addDaysToTime(coupon->items[i].expiration_days)));
				}
				player->getStorage()->addCashItem(item);
				rewardedItems.push_back(item);
			}
		}
		if (coupon->mesos > 0) {
			player->getInventory()->modifyMesos(coupon->mesos);
		}
		if (coupon->maplePoints > 0) {
			player->getStorage()->changeMaplePoints(coupon->maplePoints);
		}
		if (coupon->nxCredit > 0) {
			player->getStorage()->changeNxCredit(coupon->nxCredit);
		}
		if (coupon->nxPrepaid > 0) {
			player->getStorage()->changeNxPrepaid(coupon->nxPrepaid);
		}
		PlayerPacket::sendGotCouponRewards(player, rewardedItems, coupon->mesos, coupon->maplePoints);
		PlayerPacket::showNX(player);

		CashDataProvider::Instance()->updateCoupon(couponCode, true);
		WorldServerConnectPacket::updateCoupon(CashServer::Instance()->getWorldConnection(), couponCode, true);
	}
}

void PlayerHandler::handleSendNote(Player *player, PacketReader &packet) {
	packet.skipBytes(1);
	string receiver = packet.getString();
	string message = packet.getString();
	packet.skipBytes(1);
	packet.skipBytes(4);
	int64_t cashid = packet.get<int64_t>();
	
	CashshopGift *gift = player->getStorage()->getGift(cashid);
	if (gift != nullptr || gift->sender == receiver) {
		player->getStorage()->removeGift(cashid);
	}
	else {
		PlayerPacket::showNX(player);
	}
}

bool PlayerHandler::buyCashItem(Player *player, CashItemInfo *info, int8_t mode, bool noCashNeeded, const string &giftee, const string &giftMessage) {
	if (!noCashNeeded) {
		if ((CashDataProvider::Instance()->modifiedItemExists(info->serial) && !takeCash(player, mode, CashDataProvider::Instance()->getModifiedCashItemData(info->serial)->discountPrice)) || !takeCash(player, mode, info->price)) {
			PlayerPacket::showNX(player);
			return false;
		}
	}

	CashItem *item = new CashItem();
	item->setAmount(info->quantity);
	item->setUserId(player->getUserId());
	item->setItemId(info->itemid);
	item->setPetId(0);
	item->setName(giftee);
	if (info->expiration_days != 0) {
		// Only set the expiration time when the expiration days is higher than 0.
		item->setExpirationTime(TimeUtilities::timeToTick(TimeUtilities::addDaysToTime(info->expiration_days)));
	}
	player->getStorage()->addCashItem(item);
	if (giftee.empty()) {
		PlayerPacket::showBoughtItem(player, item);
	}
	else {
		// Prepare the gift message
		mysqlpp::Query giftMsg = Database::getCharDB().query();
		giftMsg << "INSERT INTO character_cashshop_gifts VALUES ("
			<< player->getId() << ", "
			<< item->getId() << ", "
			<< item->getItemId() << ", "
			<< mysqlpp::quote << giftee << ", "
			<< mysqlpp::quote << giftMessage  << ")";
		giftMsg.exec();

	}

	return true;
}

bool PlayerHandler::buyCashItemPackage(Player *player, CashItemInfo *info, int8_t mode, vector<CashItem *> &itemList, const string &giftee, const string &giftMessage) {
	CashItem *item = new CashItem();
	item->setAmount(info->quantity);
	item->setUserId(player->getUserId());
	item->setItemId(info->itemid);
	item->setPetId(0);
	item->setName(giftee);
	if (info->expiration_days != 0) {
		// Only set the expiration time when the expiration days is higher than 0.
		item->setExpirationTime(TimeUtilities::timeToTick(TimeUtilities::addDaysToTime(info->expiration_days)));
	}
	player->getStorage()->addCashItem(item);
	if (!giftee.empty()) {
		// Prepare the gift message
		mysqlpp::Query giftMsg = Database::getCharDB().query();
		giftMsg << "INSERT INTO character_cashshop_gifts VALUES ("
			<< player->getId() << ", "
			<< item->getId() << ", "
			<< item->getItemId() << ", "
			<< mysqlpp::quote << giftee << ", "
			<< mysqlpp::quote << giftMessage  << ")";
		giftMsg.exec();
	}

	itemList.push_back(item);
	
	return true;
}

bool PlayerHandler::takeCash(Player *player, int8_t buyMode, int32_t amount) {
	if (buyMode == BuyModes::MaplePoints) {
		if (player->getStorage()->getMaplePoints() >= amount) {
			player->getStorage()->changeMaplePoints(-amount);
		}
		else {
			return false;
		}
	}
	else if (buyMode == BuyModes::NxPrepaid) {
		if (player->getStorage()->getNxPrepaid() >= amount) {
			player->getStorage()->changeNxPrepaid(-amount);
		}
		else {
			return false;
		}
	}
	else if (buyMode == BuyModes::NxCredit) {
		if (player->getStorage()->getNxCredit() >= amount) {
			player->getStorage()->changeNxCredit(-amount);
		}
		else {
			return false;
		}
	}
	else {
		std::stringstream warning;
		warning << "Player: " << player->getName() << "; Player tried to use an unknown method of buying cash item. Mode: " << (int16_t) buyMode;
		CashServer::Instance()->log(LogTypes::Warning, warning.str());
		return false;
	}
	return true;
}
