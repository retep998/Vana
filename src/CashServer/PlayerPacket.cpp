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
#include "PlayerPacket.h"
#include "CashDataProvider.h"
#include "CashServer.h"
#include "InterHeader.h"
#include "GameLogicUtilities.h"
#include "KeyMaps.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "PetsPacket.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacketHelper.h"
#include "Randomizer.h"
#include "SendHeader.h"
#include "SkillMacros.h"
#include "Skills.h"
#include "TimeUtilities.h"
#include <boost/tr1/unordered_map.hpp>

using std::tr1::unordered_map;

void PlayerPacket::connectData(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_CONNECT_CASHSHOP);
	packet.add<int64_t>(-1);
	packet.add<int32_t>(player->getId());
	packet.addString(player->getName(), 13);
	packet.add<int8_t>(player->getGender());
	packet.add<int8_t>(player->getSkin());
	packet.add<int32_t>(player->getEyes());
	packet.add<int32_t>(player->getHair());

	player->getPets()->petConnectData(packet);

	player->getStats()->connectData(packet); // Stats

	packet.add<int32_t>(0); // Gachapon EXP

	packet.add<int32_t>(player->getMap());
	packet.add<int8_t>(player->getMappos());

	packet.add<int32_t>(0); // Unknown int32 added in .62

	packet.add<int8_t>(player->getBuddyListSize());

	player->getInventory()->connectData(packet); // Inventory data
	player->getSkills()->connectData(packet); // Skills - levels and cooldowns
	player->getQuests()->connectData(packet); // Quests

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	player->getInventory()->rockPacket(packet); // Teleport Rock/VIP Rock maps
	player->getMonsterBook()->connectData(packet);

	packet.add<int16_t>(0);

	// Party Quest data (quest needs to be added in the quests list)
	packet.add<int16_t>(0); // Amount of pquests
	// for every pquest: int16_t questid, string questdata

	packet.add<int16_t>(0);


	packet.addBool(true); // Shows the username or not, the username isn't sent when false.
	packet.addString(player->getUserName());

	vector<int32_t> *items = CashDataProvider::Instance()->getLimitSellData();
	packet.add<int32_t>(items->size());
	size_t i = 0;
	for (; i < items->size(); i++) {
		packet.add<int32_t>((*items)[i]);
	}

	
	vector<ModifiedCashItem> *modifiedItems = CashDataProvider::Instance()->getModifiedItems();
	ModifiedCashItem item;
	packet.add<int16_t>(modifiedItems->size());
	for (i = 0; i < modifiedItems->size(); i++) {
		item = (*modifiedItems)[i];
		PlayerPacketHelper::addModifiedCashInfo(packet, item);
	}
	
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);

	vector<int32_t> *bestItems = CashDataProvider::Instance()->getBestItems();
	// Top 5 items (sorted by hotness...)
	for (int8_t i = 1; i <= 8; i++) {
		for (int8_t j = 0; j <= 1; j++) {
			// These items aren't ordered...
			packet.add<int32_t>(bestItems->at(0));
			packet.add<int32_t>(i);
			packet.add<int32_t>(j);

			packet.add<int32_t>(bestItems->at(1));
			packet.add<int32_t>(i);
			packet.add<int32_t>(j);

			packet.add<int32_t>(bestItems->at(2));
			packet.add<int32_t>(i);
			packet.add<int32_t>(j);

			packet.add<int32_t>(bestItems->at(3));
			packet.add<int32_t>(i);
			packet.add<int32_t>(j);
				
			packet.add<int32_t>(bestItems->at(4));
			packet.add<int32_t>(i);
			packet.add<int32_t>(j);
		}
	}
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(92);
	player->getSession()->send(packet);
}

void PlayerPacket::changeChannel(Player *player, uint32_t ip, uint16_t port) {
	PacketCreator packet;
	packet.addHeader(SMSG_CHANNEL_CHANGE);
	packet.addBool(true);
	packet.add<uint32_t>(htonl(ip)); // MapleStory accepts IP addresses in big-endian
	packet.add<uint16_t>(port);
	player->getSession()->send(packet);
}

void PlayerPacket::disconnectPlayer(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_CHANNEL_CHANGE);
	packet.addBool(false);
	player->getSession()->send(packet);
}

void PlayerPacket::sendBlockedMessage(Player *player, int8_t type) {
	PacketCreator packet;
	packet.addHeader(SMSG_CHANNEL_BLOCKED);
	packet.add<int8_t>(type);
	player->getSession()->send(packet);
}

void PlayerPacket::showNX(Player *player, bool update) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_NX);
	packet.add<int32_t>(player->getStorage()->getNxCredit());
	packet.add<int32_t>(player->getStorage()->getMaplePoints());
	packet.add<int32_t>(player->getStorage()->getNxPrepaid());
	player->getSession()->send(packet);
}

void PlayerPacket::showWishList(Player *player, bool update) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(update ? 0x48 : 0x42); // V.81: update ? 0x55 : 0x4f
	player->getInventory()->wishListPacket(packet);

	player->getSession()->send(packet);
}

void PlayerPacket::showCashInventory(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x3e);
	player->getStorage()->cashItemsPacket(packet);
	packet.add<int16_t>(player->getStorage()->getSlots());
	packet.add<int16_t>(player->getStorage()->getCharSlots());
	
	player->getSession()->send(packet);
}

void PlayerPacket::sendWishListFailed(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x49);
	packet.add<int8_t>(0);

	player->getSession()->send(packet);
}

void PlayerPacket::showBoughtItem(Player *player, CashItem *item) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x4a);
	PlayerPacketHelper::addCashItemInfo(packet, item);
	player->getSession()->send(packet);
}

void PlayerPacket::showFailure(Player *player, uint8_t reason) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x4B);
	packet.add<int8_t>(reason);
	player->getSession()->send(packet);
}

void PlayerPacket::showGiftItems(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x40);
	player->getStorage()->giftPacket(packet);

	player->getSession()->send(packet);
}

void PlayerPacket::sendItemMovedToInventory(Player *player, Item *item, int16_t pos) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x59);
	if (GameLogicUtilities::isPet(item->getId())) {
		packet.add<int16_t>(pos);
		PetsPacket::addInfo(packet, player->getPets()->getPet(item->getPetId()));
	}
	else {
		PlayerPacketHelper::addItemInfo(packet, pos, item, true);
	}
	player->getSession()->send(packet);
}

void PlayerPacket::sendItemMovedToStorage(Player *player, CashItem *item) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x5b);
	PlayerPacketHelper::addCashItemInfo(packet, item);
	player->getSession()->send(packet);
}

void PlayerPacket::sendIncreasedInventorySlots(Player *player, int8_t inventory, int8_t slots) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x53);
	packet.add<int8_t>(inventory);
	packet.add<int16_t>(slots);
	player->getSession()->send(packet);
}

void PlayerPacket::sendIncreasedStorageSlots(Player *player, int8_t slots) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x55);
	packet.add<int16_t>(slots);
	player->getSession()->send(packet);
}

void PlayerPacket::sendSentItem(Player *player, const string &receiver, int32_t itemid, int32_t cashSpent) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x7c);
	packet.addString(receiver);
	packet.add<int32_t>(itemid);
	packet.add<int16_t>(0x00); // Are skipped
	packet.add<int16_t>(0x00);
	packet.add<int32_t>(cashSpent);
	player->getSession()->send(packet);
}

void PlayerPacket::sendBoughtPackage(Player *player, vector<CashItem *> *items) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x7a);
	packet.add<int8_t>(items->size());
	for (size_t i = 0; i < items->size(); i++) {
		PlayerPacketHelper::addCashItemInfo(packet, items->at(i));
	}
	packet.add<int16_t>(0x00);
	player->getSession()->send(packet);
}

void PlayerPacket::sendGotCouponRewards(Player *player, vector<CashItem *> items, int32_t mesos, int32_t maplePoints) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x4c);
	packet.add<int8_t>(items.size());
	for (size_t i = 0; i < items.size(); i++) {
		PlayerPacketHelper::addCashItemInfo(packet, items[i]);
	}
	packet.add<int32_t>(maplePoints);
	packet.add<int32_t>(0);
	packet.add<int32_t>(mesos);
	player->getSession()->send(packet);

}

void PlayerPacket::sendItemExpired(Player *player, int64_t cashId) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<int8_t>(0x5d);
	packet.add<int64_t>(cashId);
	player->getSession()->send(packet);
}

void PlayerPacket::sendBoughtNonCashItem(Player *player, int16_t amount, int16_t slot, int32_t itemid) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASHSHOP_OPERATION);
	packet.add<uint8_t>(0x7e);
	packet.add<int32_t>(0x01); // Amount of items
	packet.add<int16_t>(amount);
	packet.add<int16_t>(slot);
	packet.add<int32_t>(itemid);
	player->getSession()->send(packet);
}

void PlayerPacket::sendChangeNameResult(Player *player, const string &name, int8_t result) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_CASHSHOP_NAMECHANGE_RESULT);
	packet.addString(name);
	packet.add<int8_t>(result);
	player->getSession()->send(packet);
}
