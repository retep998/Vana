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
#include "TradesPacket.h"
#include "Inventory.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "PlayerPacketHelper.h"
#include "SendHeader.h"

void TradesPacket::sendOpenTrade(Player *player, Player *player1, Player *player2) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x05);
	packet.addByte(0x03);
	packet.addByte(0x02);
	packet.addShort(((player1 != 0 && player2 != 0) ? 1 : 0));
	if (player2 != 0) {
		PlayerPacketHelper::addPlayerDisplay(packet, player2);
		packet.addString(player2->getName());
		packet.addByte(1); // Location in the window
	}
	if (player1 != 0) {
		PlayerPacketHelper::addPlayerDisplay(packet, player1);
		packet.addString(player1->getName());
		packet.addByte(-1); // Location in the window
	}
	player->getSession()->send(packet);
}

void TradesPacket::sendTradeRequest(Player *player, Player *receiver, int32_t tradeid) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x02);
	packet.addByte(0x03);
	packet.addString(player->getName());
	packet.addInt(tradeid);
	receiver->getSession()->send(packet);
}

void TradesPacket::sendTradeMessage(Player *player, Player *receiver, int8_t type, int8_t message) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(type);
	packet.addByte(message);
	packet.addString(player->getName());
	receiver->getSession()->send(packet);
}

void TradesPacket::sendTradeMessage(Player *receiver, int8_t type, int8_t message) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(type);
	packet.addByte(0x00);
	packet.addByte(message);
	receiver->getSession()->send(packet);
}

void TradesPacket::sendTradeChat(Player *player, uint8_t blue, const string &chat) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x06);
	packet.addByte(0x08);
	packet.addByte(blue);
	packet.addString(chat);
	player->getSession()->send(packet);
}

void TradesPacket::sendAddUser(Player *original, Player *newb, int8_t slot) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x04);
	packet.addByte(slot);
	PlayerPacketHelper::addPlayerDisplay(packet, newb);
	packet.addString(newb->getName());
	original->getSession()->send(packet);
}

void TradesPacket::sendLeaveTrade(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x0A);
	packet.addByte(0x01); // Slot, doesn't matter for trades
	packet.addByte(0x02); // Message, doesn't matter for trades
	player->getSession()->send(packet);
}

void TradesPacket::sendAddMesos(Player *receiver, uint8_t slot, int32_t amount) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x0F);
	packet.addByte(slot);
	packet.addInt(amount);
	receiver->getSession()->send(packet);
}

void TradesPacket::sendAccepted(Player *destination) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x10);
	destination->getSession()->send(packet);
}

void TradesPacket::sendEndTrade(Player *destination, uint8_t message) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x0A);
	packet.addByte(0x00);
	packet.addByte(message);
	// Message:
	//			0x06 = success [tax is automated]
	//			0x07 = unsuccessful
	//			0x08 = "You cannot make the trade because there are some items which you cannot carry more than one."
	//			0x09 = "You cannot make the trade because the other person's on a different map."
	destination->getSession()->send(packet);
}

void TradesPacket::sendAddItem(Player *destination, uint8_t player, int8_t slot, int8_t inventory, Item *item) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x0E);
	packet.addByte(player);
	PlayerPacketHelper::addItemInfo(packet, slot, item);
	destination->getSession()->send(packet);
}