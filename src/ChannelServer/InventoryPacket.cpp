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
#include "InventoryPacket.h"
#include "ChannelServer.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InterHeader.h"
#include "Inventory.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerInventory.h"
#include "PlayerPacketHelper.h"
#include "Players.h"
#include "SendHeader.h"

void InventoryPacket::moveItem(Player *player, int8_t inv, int16_t slot1, int16_t slot2) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_ITEM);
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
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_UPDATE_CHAR_LOOK);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(1);
	PlayerPacketHelper::addPlayerDisplay(packet, player);
	packet.add<int8_t>(0);
	packet.add<int16_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void InventoryPacket::bought(Player *player, uint8_t msg) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOP_BOUGHT);
	packet.add<int8_t>(msg);
	player->getSession()->send(packet);
}

void InventoryPacket::addNewItem(Player *player, int8_t inv, int16_t slot, Item *item, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_ITEM);
	packet.add<int8_t>(is);
	packet.add<int8_t>(1);
	packet.add<int8_t>(0);
	packet.add<int8_t>(inv);
	PlayerPacketHelper::addItemInfo(packet, slot, item, true);
	player->getSession()->send(packet);
}

void InventoryPacket::addItem(Player *player, int8_t inv, int16_t slot, Item *item, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_ITEM);
	packet.add<int8_t>(is);
	packet.add<int8_t>(1);
	packet.add<int8_t>(1);
	packet.add<int8_t>(inv);
	packet.add<int16_t>(slot);
	packet.add<int16_t>(item->amount);
	player->getSession()->send(packet);
}

void InventoryPacket::updateItemAmounts(Player *player, int8_t inv, int16_t slot1, int16_t amount1, int16_t slot2, int16_t amount2) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_ITEM);
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
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_UPDATE_STAT);
	packet.add<int16_t>(1);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
	packet = PacketCreator();
	packet.add<int16_t>(SEND_SIT_CHAIR);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(chairid);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void InventoryPacket::sitMapChair(Player *player, int16_t chairid) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CHAIR_ACTION);
	packet.add<int8_t>(1);
	packet.add<int16_t>(chairid);
	player->getSession()->send(packet);
}

void InventoryPacket::stopChair(Player *player, bool showMap) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CHAIR_ACTION);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide() || !showMap)
		return;
	packet = PacketCreator();
	packet.add<int16_t>(SEND_SIT_CHAIR);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void InventoryPacket::useScroll(Player *player, int8_t succeed, bool destroy, bool legendary_spirit) {
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_USE_SCROLL);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(succeed); // Succeed/Fail
	packet.add<int8_t>(destroy); // Destroy/Not Destroy
	packet.add<int16_t>(legendary_spirit);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void InventoryPacket::showMegaphone(Player *player, const string &msg) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_NOTICE);
	packet.add<int8_t>(2);
	packet.addString(msg);
	Players::Instance()->sendPacket(packet); // In global, this sends to everyone on the current channel, not the map
}

void InventoryPacket::showSuperMegaphone(Player *player, const string &msg, uint8_t whisper) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TO_PLAYERS);
	packet.add<int16_t>(SEND_NOTICE);
	packet.add<int8_t>(3);
	packet.addString(msg);
	packet.add<int8_t>((uint8_t) ChannelServer::Instance()->getChannel());
	packet.add<int8_t>(whisper);
	ChannelServer::Instance()->sendToWorld(packet);
}

void InventoryPacket::showMessenger(Player *player, const string &msg, const string &msg2, const string &msg3, const string &msg4, unsigned char *displayInfo, int32_t displayInfo_size, int32_t itemid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TO_PLAYERS);
	packet.add<int16_t>(SEND_SHOW_MESSENGER);
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

void InventoryPacket::showItemMegaphone(Player *player, const string &msg, uint8_t whisper, Item *item) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TO_PLAYERS);
	packet.add<int16_t>(SEND_NOTICE);
	packet.add<int8_t>(8);
	packet.addString(msg);
	packet.add<int8_t>((uint8_t) ChannelServer::Instance()->getChannel());
	packet.add<int8_t>(whisper);
	if (item == 0) {
		packet.add<int8_t>(0);
	}
	else {
		PlayerPacketHelper::addItemInfo(packet, 1, item);
	}
	ChannelServer::Instance()->sendToWorld(packet);
}

void InventoryPacket::useSkillbook(Player *player, int32_t skillid, int32_t newMaxLevel, bool use, bool succeed) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_USE_SKILLBOOK);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(1); // Number of skills? Maybe just padding or random boolean
	packet.add<int32_t>(skillid);
	packet.add<int32_t>(newMaxLevel);
	packet.add<int8_t>(use); // Use/cannot use
	packet.add<int8_t>(succeed); // Pass/fail
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void InventoryPacket::useItemEffect(Player *player, int32_t itemid) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_ITEM_EFFECT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(itemid);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void InventoryPacket::updateSlots(Player *player, int8_t inventory, int8_t slots) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_UPDATE_INVENTORY_SLOTS);
	packet.add<int8_t>(inventory);
	packet.add<int8_t>(slots);
	player->getSession()->send(packet);
}

void InventoryPacket::blankUpdate(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_ITEM);
	packet.add<int8_t>(0x01);
	packet.add<int8_t>(0x00);
	player->getSession()->send(packet);
}

void InventoryPacket::sendRockUpdate(Player *player, int8_t mode, int8_t type, const vector<int32_t> &maps) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_TELEPORT_ROCK_FUNCTION);
	packet.add<int8_t>(mode);
	packet.add<int8_t>(type);
	size_t remaining = 1;
	uint8_t max = (type == 0 ? Inventories::TeleportRockMax : Inventories::VipRockMax);
	for (; remaining <= maps.size(); remaining++) {
		packet.add<int32_t>(maps[remaining - 1]);
	}
	for (; remaining <= max; remaining++) {
		packet.add<int32_t>(Maps::NoMap);
	}
	player->getSession()->send(packet);
}

void InventoryPacket::sendMesobagSucceed(Player *player, int32_t mesos) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_GAIN_MESOBAG_MESOS);
	packet.add<int32_t>(mesos);
	player->getSession()->send(packet);
}

void InventoryPacket::sendMesobagFailed(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_USE_MESOBAG_FAILED);
	player->getSession()->send(packet);
}

void InventoryPacket::sendRockError(Player *player, int8_t code, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_TELEPORT_ROCK_FUNCTION);
	packet.add<int8_t>(code);
	packet.add<int8_t>(type);
	player->getSession()->send(packet);
}

void InventoryPacket::useCharm(Player *player, uint8_t charmsleft, uint8_t daysleft) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_GAIN_ITEM);
	packet.add<int8_t>(0x06);
	packet.add<int8_t>(0x01);
	packet.add<uint8_t>(charmsleft);
	packet.add<uint8_t>(daysleft);
	player->getSession()->send(packet);
}

void InventoryPacket::sendHammerSlots(Player *player, int32_t slots) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_HAMMER_TIME);
	packet.add<int8_t>(0x34); // No idea... mode of some sort, I think
	packet.add<int32_t>(0x00);
	packet.add<int32_t>(slots);
	player->getSession()->send(packet);
}

void InventoryPacket::sendHulkSmash(Player *player, int16_t slot, Item *hammered) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_ITEM);
	packet.add<int8_t>(0x00);
	packet.add<int8_t>(0x02);
	packet.add<int8_t>(0x03);
	packet.add<int8_t>(GameLogicUtilities::getInventory(hammered->id));
	packet.add<int16_t>(slot);
	packet.add<int8_t>(0x00);
	packet.add<int8_t>(0x01);
	PlayerPacketHelper::addItemInfo(packet, slot, hammered, true);
	player->getSession()->send(packet);
}

void InventoryPacket::sendHammerUpdate(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_HAMMER_TIME);
	packet.add<int8_t>(0x38); // No idea... mode of some sort, I think
	packet.add<int32_t>(0x00);
	player->getSession()->send(packet);
}