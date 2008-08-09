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
#include "PlayerPacketHelper.h"
#include "Inventory.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "SendHeader.h"

void TradesPacket::sendOpenTrade(Player *player, const vector<Player *> &players, const vector<unsigned char> &pos) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x05);
	packet.addByte(0x03);
	packet.addByte(0x02);
	packet.addShort(players.size() - 1);
	for (unsigned char c = 0; c < players.size(); c++) { // lol	
		PlayerPacketHelper::addPlayerDisplay(packet, players[c]);
		packet.addString(players[c]->getName());
		packet.addByte(pos[c]); // Location in the window
	}
	player->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendTradeRequest(Player *player, Player *receiver, int tradeid) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x02);
	packet.addByte(0x03);
	packet.addString(player->getName());
	packet.addInt(tradeid);
	receiver->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendTradeMessage(Player *player, Player *receiver, char type, char message) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(type);
	packet.addByte(message);
	packet.addString(player->getName());
	receiver->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendTradeMessage(Player *receiver, char type, char message) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(type);
	packet.addByte(0x00);
	packet.addByte(message);
	receiver->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendTradeChat(Player *player, unsigned char blue, string chat) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x06);
	packet.addByte(0x08);
	packet.addByte(blue);
	packet.addString(chat);
	player->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendAddUser(Player *original, Player *newb, char slot) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x04);
	packet.addByte(slot);
	PlayerPacketHelper::addPlayerDisplay(packet, newb);
	packet.addString(newb->getName());
	original->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendLeaveTrade(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x0A);
	packet.addByte(0x01); // Slot, doesn't matter for trades
	packet.addByte(0x02); // Message, doesn't matter for trades
	player->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendAddMesos(Player *receiver, unsigned char slot, int amount) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x0F);
	packet.addByte(slot);
	packet.addInt(amount);
	receiver->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendAccepted(Player *destination) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x10);
	destination->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendEndTrade(Player *destination, unsigned char message) {
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
	destination->getPacketHandler()->sendPacket(packet);
}

void TradesPacket::sendAddItem(Player *destination, unsigned char player, char slot, char inventory, Item *item) {
	PacketCreator packet;
	packet.addShort(SEND_SHOP_ACTION);
	packet.addByte(0x0E);
	packet.addByte(player);
	PlayerPacketHelper::addItemInfo(packet, slot, item);
	destination->getPacketHandler()->sendPacket(packet);
}