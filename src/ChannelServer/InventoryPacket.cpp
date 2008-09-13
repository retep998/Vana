/*
Copyright (C) 2008 Vana Development Team

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

void InventoryPacket::moveItem(Player *player, char inv, int16_t slot1, int16_t slot2) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_ITEM);
	packet.addByte(1);
	packet.addByte(1);
	packet.addByte(2);
	packet.addByte(inv);
	packet.addShort(slot1);
	packet.addShort(slot2);
	packet.addByte(1);
	player->getSession()->send(packet);
}

void InventoryPacket::updatePlayer(Player *player) {
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_CHAR_LOOK);
	packet.addInt(player->getId());
	packet.addByte(1);
	PlayerPacketHelper::addPlayerDisplay(packet, player);
	packet.addByte(0);
	packet.addShort(0);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void InventoryPacket::bought(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_BOUGHT);
	packet.addByte(0);
	player->getSession()->send(packet);
}

void InventoryPacket::addNewItem(Player *player, char inv, int16_t slot, Item *item, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_ITEM);
	packet.addByte(is);
	packet.addByte(1);
	packet.addByte(0);
	packet.addByte(inv);
	PlayerPacketHelper::addItemInfo(packet, slot, item, true);
	player->getSession()->send(packet);
}
void InventoryPacket::addItem(Player *player, char inv, int16_t slot, Item *item, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_ITEM);
	packet.addByte(is);
	packet.addByte(1);
	packet.addByte(1);
	packet.addByte(inv);
	packet.addShort(slot);
	packet.addShort(item->amount);
	player->getSession()->send(packet);
}

void InventoryPacket::updateItemAmounts(Player *player, char inv, int16_t slot1, int16_t amount1, int16_t slot2, int16_t amount2) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_ITEM);
	packet.addByte(1);
	packet.addByte((slot2 > 0) + 1);
	packet.addByte(1);
	packet.addByte(inv);
	packet.addShort(slot1);
	packet.addShort(amount1);
	if (slot2 > 0) {
		packet.addByte(1);
		packet.addByte(inv);
		packet.addShort(slot2);
		packet.addShort(amount2);
	}
	player->getSession()->send(packet);
}

void InventoryPacket::sitChair(Player *player, int32_t chairid) {
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addShort(1);
	packet.addInt(0);
	player->getSession()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_SIT_CHAIR);
	packet.addInt(player->getId());
	packet.addInt(chairid);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void InventoryPacket::stopChair(Player *player) {
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	PacketCreator packet;
	packet.addShort(SEND_STOP_CHAIR);
	packet.addByte(0);
	player->getSession()->send(packet);
	packet = PacketCreator();
	packet.addShort(SEND_SIT_CHAIR);
	packet.addInt(player->getId());
	packet.addInt(0);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}
void InventoryPacket::useScroll(Player *player, bool succeed, bool destroy, bool legendary_spirit) {
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	PacketCreator packet;
	packet.addShort(SEND_USE_SCROLL);
	packet.addInt(player->getId());
	packet.addByte(succeed); // Succeed/Fail
	packet.addByte(destroy); // Destroy/Not Destroy
	packet.addByte(legendary_spirit);
	packet.addByte(0);
	Maps::maps[player->getMap()]->sendPacket(packet);
}

void InventoryPacket::showMegaphone(Player *player, const string & msg) {
	string fullMessage = string(player->getName()) + " : " + msg;
	PacketCreator packet;
	packet.addShort(SEND_NOTICE);
	packet.addByte(2);
	packet.addString(fullMessage);
	Maps::maps[player->getMap()]->sendPacket(packet);
}

void InventoryPacket::showSuperMegaphone(Player *player, const string & msg, uint8_t whisper) {
	string fullMessage = string(player->getName()) + " : " + msg;
	PacketCreator packet;
	packet.addShort(INTER_TO_PLAYERS);
	packet.addShort(SEND_NOTICE);
	packet.addByte(3);
	packet.addString(fullMessage);
	packet.addByte((uint8_t) ChannelServer::Instance()->getChannel());
	packet.addByte(whisper);
	ChannelServer::Instance()->sendToWorld(packet);
}

void InventoryPacket::showMessenger(Player *player, const string & msg, const string & msg2, const string & msg3, const string & msg4, unsigned char *displayInfo, int32_t displayInfo_size, int32_t itemid) {
	PacketCreator packet;
	packet.addShort(INTER_TO_PLAYERS);
	packet.addShort(SEND_SHOW_MESSENGER);
	packet.addInt(itemid);
	packet.addString(player->getName());
	packet.addString(msg);
	packet.addString(msg2);
	packet.addString(msg3);
	packet.addString(msg4);
	packet.addInt(ChannelServer::Instance()->getChannel());
	packet.addBuffer(displayInfo, displayInfo_size);
	ChannelServer::Instance()->sendToWorld(packet);
}
// Use buff item
void InventoryPacket::useItem(Player *player, int32_t itemid, int32_t time, unsigned char types[8], vector<int16_t> vals, bool morph) { // Test/Beta function, PoC only
	PacketCreator packet;
	packet.addShort(SEND_USE_SKILL);
	packet.addInt64(0);
	for (char i = 0; i < 8; i++)
		packet.addByte(types[i]);
	for (size_t i = 0; i < vals.size(); i++) {
		packet.addShort(vals[i]);
		packet.addInt(itemid * -1);
		packet.addInt(time);
	}
	packet.addShort(0);
	packet.addByte(morph);
	packet.addByte(0);
	packet.addByte(0); // Speed/jump related item buffs will EoF without this
	if (morph)
		packet.addByte(0);
	player->getSession()->send(packet);
	if (morph) {
		if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
			return;
		packet = PacketCreator();
		packet.addShort(SEND_SHOW_OTHERS_SKILL);
		packet.addInt(player->getId());
		packet.addInt64(0);
		for (char i = 0; i < 8; i++)
			packet.addByte(types[i]);
		for (size_t i = 0; i < vals.size(); i++)
			packet.addShort(vals[i]);
		packet.addByte(1);
		packet.addShort(0);
		packet.addByte(0);
		Maps::maps[player->getMap()]->sendPacket(packet, player);
	}
}
void InventoryPacket::endItem(Player *player, unsigned char types[8], bool morph) {
	PacketCreator packet;
	packet.addShort(SEND_CANCEL_SKILL);
	packet.addInt64(0);
	for (char i = 0; i < 8; i++)
		packet.addByte(types[i]);
	packet.addByte(0);
	player->getSession()->send(packet);
	if (morph) {
		if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
			return;
		PacketCreator packet;
		packet.addShort(SEND_CANCEL_OTHERS_BUFF);
		packet.addInt(player->getId());
		packet.addInt64(0);
		for (char i = 0; i < 8; i++)
			packet.addByte(types[i]);
		Maps::maps[player->getMap()]->sendPacket(packet, player);
	}
}
// Skill Books
void InventoryPacket::useSkillbook(Player *player, int32_t skillid, int32_t newMaxLevel, bool use, bool succeed) {
	PacketCreator packet;
	packet.addShort(SEND_USE_SKILLBOOK);
	packet.addInt(player->getId());
	packet.addByte(1); // Number of skills? Maybe just padding or random boolean
	packet.addInt(skillid); // Skill ID
	packet.addInt(newMaxLevel); // New max level
	packet.addByte(use); // Use/Cannot use
	packet.addByte(succeed); // Pass/Fail
	Maps::maps[player->getMap()]->sendPacket(packet);
}

void InventoryPacket::useItemEffect(Player *player, int32_t itemid) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_ITEM_EFFECT);
	packet.addInt(player->getId());
	packet.addInt(itemid);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void InventoryPacket::updateSlots(Player *player, char inventory, char slots) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_INVENTORY_SLOTS);
	packet.addByte(inventory);
	packet.addByte(slots);
	player->getSession()->send(packet);
}

void InventoryPacket::blankUpdate(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_ITEM);
	packet.addByte(0x01);
	packet.addByte(0x00);
	player->getSession()->send(packet);
}
