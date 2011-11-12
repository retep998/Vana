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
#include "InventoryPacket.h"
#include "ChannelServer.h"
#include "GameLogicUtilities.h"
#include "InterHeader.h"
#include "Inventory.h"
#include "InventoryPacketHelper.h"
#include "ItemConstants.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerInventory.h"
#include "PlayerPacketHelper.h"
#include "SendHeader.h"

void InventoryPacket::moveItem(Player *player, int8_t inv, int16_t slot1, int16_t slot2) {
	PacketCreator packet;
	packet.addHeader(SMSG_INVENTORY_ITEM_MOVE);
	packet.add<int8_t>(1);
	packet.add<int8_t>(1);
	packet.add<int8_t>(2);
	packet.add<int8_t>(inv);
	packet.add<int16_t>(slot1);
	packet.add<int16_t>(slot2);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void InventoryPacket::updatePlayer(Player *player) {
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_CHANGE_LOOK);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(1);
	PlayerPacketHelper::addPlayerDisplay(packet, player);
	packet.add<int8_t>(0);
	packet.add<int16_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void InventoryPacket::addNewItem(Player *player, int8_t inv, int16_t slot, Item *item, bool is) {
	PacketCreator packet;
	packet.addHeader(SMSG_INVENTORY_ITEM_MOVE);
	packet.addBool(is);
	packet.add<int8_t>(1);
	packet.add<int8_t>(0);
	packet.add<int8_t>(inv);
	PlayerPacketHelper::addItemInfo(packet, slot, item, true);
	player->getSession()->send(packet);
}

void InventoryPacket::addItem(Player *player, int8_t inv, int16_t slot, Item *item, bool is) {
	PacketCreator packet;
	packet.addHeader(SMSG_INVENTORY_ITEM_MOVE);
	packet.addBool(is);
	packet.add<int8_t>(1);
	packet.add<int8_t>(1);
	packet.add<int8_t>(inv);
	packet.add<int16_t>(slot);
	packet.add<int16_t>(item->getAmount());
	player->getSession()->send(packet);
}

void InventoryPacket::updateItemAmounts(Player *player, int8_t inv, int16_t slot1, int16_t amount1, int16_t slot2, int16_t amount2) {
	PacketCreator packet;
	packet.addHeader(SMSG_INVENTORY_ITEM_MOVE);
	packet.add<int8_t>(1);
	packet.add<int8_t>((slot2 > 0) + 1);
	packet.add<int8_t>(1);
	packet.add<int8_t>(inv);
	packet.add<int16_t>(slot1);
	packet.add<int16_t>(amount1);
	if (slot2 > 0) {
		packet.add<int8_t>(1);
		packet.add<int8_t>(inv);
		packet.add<int16_t>(slot2);
		packet.add<int16_t>(amount2);
	}
	player->getSession()->send(packet);
}

void InventoryPacket::sitChair(Player *player, int32_t chairid) {
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_UPDATE);
	packet.add<int16_t>(1);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_CHAIR_SIT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(chairid);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void InventoryPacket::sitMapChair(Player *player, int16_t chairid) {
	PacketCreator packet;
	packet.addHeader(SMSG_CHAIR);
	packet.add<int8_t>(1);
	packet.add<int16_t>(chairid);
	player->getSession()->send(packet);
}

void InventoryPacket::stopChair(Player *player, bool showMap) {
	PacketCreator packet;
	packet.addHeader(SMSG_CHAIR);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);

	if (player->getActiveBuffs()->isUsingHide() || !showMap) {
		return;
	}

	packet = PacketCreator();
	packet.addHeader(SMSG_CHAIR_SIT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void InventoryPacket::useScroll(Player *player, int8_t succeed, bool destroy, bool legendarySpirit) {
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	PacketCreator packet;
	packet.addHeader(SMSG_SCROLL_USE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(succeed);
	packet.addBool(destroy);
	packet.add<int16_t>(legendarySpirit);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void InventoryPacket::showMegaphone(Player *player, const string &msg) {
	PacketCreator packet;
	packet.addHeader(SMSG_MESSAGE);
	packet.add<int8_t>(2);
	packet.addString(msg);
	PlayerDataProvider::Instance()->sendPacket(packet); // In global, this sends to everyone on the current channel, not the map
}

void InventoryPacket::showSuperMegaphone(Player *player, const string &msg, bool whisper) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_TO_PLAYERS);
	packet.addHeader(SMSG_MESSAGE);
	packet.add<int8_t>(3);
	packet.addString(msg);
	packet.add<int8_t>((int8_t) ChannelServer::Instance()->getChannel());
	packet.addBool(whisper);
	ChannelServer::Instance()->sendToWorld(packet);
}

void InventoryPacket::showMessenger(Player *player, const string &msg, const string &msg2, const string &msg3, const string &msg4, unsigned char *displayInfo, int32_t displayInfo_size, int32_t itemid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_TO_PLAYERS);
	packet.addHeader(SMSG_AVATAR_MEGAPHONE);
	packet.add<int32_t>(itemid);
	packet.addString(player->getName());
	packet.addString(msg);
	packet.addString(msg2);
	packet.addString(msg3);
	packet.addString(msg4);
	packet.add<int32_t>(ChannelServer::Instance()->getChannel());
	packet.addBuffer(displayInfo, displayInfo_size);
	ChannelServer::Instance()->sendToWorld(packet);
}

void InventoryPacket::showItemMegaphone(Player *player, const string &msg, bool whisper, Item *item) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_TO_PLAYERS);
	packet.addHeader(SMSG_MESSAGE);
	packet.add<int8_t>(8);
	packet.addString(msg);
	packet.add<int8_t>((int8_t) ChannelServer::Instance()->getChannel());
	packet.addBool(whisper);
	if (item == nullptr) {
		packet.add<int8_t>(0);
	}
	else {
		PlayerPacketHelper::addItemInfo(packet, 1, item);
	}
	ChannelServer::Instance()->sendToWorld(packet);
}

void InventoryPacket::showTripleMegaphone(Player *player, int8_t lines, const string &line1, const string &line2, const string &line3, bool whisper) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_TO_PLAYERS);
	packet.addHeader(SMSG_MESSAGE);
	packet.add<int8_t>(0x0a);
	packet.addString(line1);
	packet.add<int8_t>(lines);
	if (lines > 1) {
		packet.addString(line2);
	}
	if (lines > 2) {
		packet.addString(line3);
	}
	packet.add<int8_t>((int8_t) ChannelServer::Instance()->getChannel());
	packet.addBool(whisper);
	ChannelServer::Instance()->sendToWorld(packet);
}

void InventoryPacket::useSkillbook(Player *player, int32_t skillid, int32_t newMaxLevel, bool use, bool succeed) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILLBOOK);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(1); // Number of skills? Maybe just padding or random boolean
	packet.add<int32_t>(skillid);
	packet.add<int32_t>(newMaxLevel);
	packet.addBool(use);
	packet.addBool(succeed);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void InventoryPacket::useItemEffect(Player *player, int32_t itemid) {
	PacketCreator packet;
	packet.addHeader(SMSG_ITEM_EFFECT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(itemid);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void InventoryPacket::updateSlots(Player *player, int8_t inventory, int8_t slots) {
	PacketCreator packet;
	packet.addHeader(SMSG_INVENTORY_SLOT_UPDATE);
	packet.add<int8_t>(inventory);
	packet.add<int8_t>(slots);
	player->getSession()->send(packet);
}

void InventoryPacket::blankUpdate(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_INVENTORY_ITEM_MOVE);
	packet.add<int8_t>(0x01);
	packet.add<int8_t>(0x00);
	player->getSession()->send(packet);
}

void InventoryPacket::sendRockUpdate(Player *player, int8_t mode, int8_t type, const vector<int32_t> &maps) {
	PacketCreator packet;
	packet.addHeader(SMSG_TELEPORT_ROCK);
	packet.add<int8_t>(mode);
	packet.add<int8_t>(type);

	InventoryPacketHelper::fillRockPacket(packet, maps, (type == RockTypes::Regular ? Inventories::TeleportRockMax : Inventories::VipRockMax));

	player->getSession()->send(packet);
}

void InventoryPacket::sendRockError(Player *player, int8_t code, int8_t type) {
	PacketCreator packet;
	packet.addHeader(SMSG_TELEPORT_ROCK);
	packet.add<int8_t>(code);
	packet.add<int8_t>(type);
	player->getSession()->send(packet);
}

void InventoryPacket::sendMesobagSucceed(Player *player, int32_t mesos) {
	PacketCreator packet;
	packet.addHeader(SMSG_MESOBAG_SUCCESS);
	packet.add<int32_t>(mesos);
	player->getSession()->send(packet);
}

void InventoryPacket::sendMesobagFailed(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_MESOBAG_FAILURE);
	player->getSession()->send(packet);
}

void InventoryPacket::useCharm(Player *player, uint8_t charmsleft, uint8_t daysleft) {
	PacketCreator packet;
	packet.addHeader(SMSG_THEATRICS);
	packet.add<int8_t>(0x06);
	packet.add<int8_t>(0x01);
	packet.add<uint8_t>(charmsleft);
	packet.add<uint8_t>(daysleft);
	player->getSession()->send(packet);
}

void InventoryPacket::sendHammerSlots(Player *player, int32_t slots) {
	PacketCreator packet;
	packet.addHeader(SMSG_HAMMER);
	packet.add<int8_t>(0x34); // No idea... mode of some sort, I think
	packet.add<int32_t>(0x00);
	packet.add<int32_t>(slots);
	player->getSession()->send(packet);
}

void InventoryPacket::sendHulkSmash(Player *player, int16_t slot, Item *hammered) {
	PacketCreator packet;
	packet.addHeader(SMSG_INVENTORY_ITEM_MOVE);
	packet.add<int8_t>(0x00);
	packet.add<int8_t>(0x02);
	packet.add<int8_t>(0x03);
	packet.add<int8_t>(GameLogicUtilities::getInventory(hammered->getId()));
	packet.add<int16_t>(slot);
	packet.add<int8_t>(0x00);
	packet.add<int8_t>(0x01);
	PlayerPacketHelper::addItemInfo(packet, slot, hammered, true);
	player->getSession()->send(packet);
}

void InventoryPacket::sendHammerUpdate(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_HAMMER);
	packet.add<int8_t>(0x38); // No idea... mode of some sort, I think
	packet.add<int32_t>(0x00);
	player->getSession()->send(packet);
}

void InventoryPacket::sendChalkboardUpdate(Player *player, const string &msg) {
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	PacketCreator packet;
	packet.addHeader(SMSG_CHALKBOARD);
	packet.add<int32_t>(player->getId());
	packet.addBool(!msg.empty());
	packet.addString(msg);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void InventoryPacket::playCashSong(int32_t map, int32_t itemid, const string &playername) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASH_SONG);
	packet.add<int32_t>(itemid);
	packet.addString(playername);
	Maps::getMap(map)->sendPacket(packet);
}

void InventoryPacket::playCashSongPlayer(Player *player, int32_t itemid, const string &playername) {
	PacketCreator packet;
	packet.addHeader(SMSG_CASH_SONG);
	packet.add<int32_t>(itemid);
	packet.addString(playername);
	player->getSession()->send(packet);
}

void InventoryPacket::sendRewardItemAnimation(Player *player, int32_t itemid, const string &effect) {
	PacketCreator packet;
	packet.addHeader(SMSG_THEATRICS);
	packet.add<int8_t>(0x0E);
	packet.add<int32_t>(itemid);
	packet.add<int8_t>(1); // Unk...?
	packet.addString(effect);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(0x0E);
	packet.add<int32_t>(itemid);
	packet.add<int8_t>(1); // Unk...?
	packet.addString(effect);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void InventoryPacket::sendItemExpired(Player *player, vector<int32_t> *items) {
	PacketCreator packet;
	packet.add<int16_t>(SMSG_NOTICE);
	packet.add<int8_t>(0x08);
	packet.add<int8_t>(items->size());
	for (size_t i = 0; i < items->size(); i++) {
		packet.add<int32_t>(items->at(i));
	}
	player->getSession()->send(packet);
}

void InventoryPacket::sendCannotFlyHere(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_MESSAGE_CANT_FLY_HERE);
	player->getSession()->send(packet);
}