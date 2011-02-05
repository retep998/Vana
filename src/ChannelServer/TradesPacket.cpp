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
#include "TradesPacket.h"
#include "Inventory.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "SendHeader.h"

void TradesPacket::sendOpenTrade(Player *player, Player *player1, Player *player2) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(0x05);
	packet.add<int8_t>(0x03);
	packet.add<int8_t>(0x02);
	packet.add<int16_t>(((player1 != nullptr && player2 != nullptr) ? 1 : 0));
	if (player2 != nullptr) {
		PlayerPacketHelper::addPlayerDisplay(packet, player2);
		packet.addString(player2->getName());
		packet.add<int8_t>(1); // Location in the window
	}
	if (player1 != nullptr) {
		PlayerPacketHelper::addPlayerDisplay(packet, player1);
		packet.addString(player1->getName());
		packet.add<int8_t>(-1); // Location in the window
	}
	player->getSession()->send(packet);
}

void TradesPacket::sendTradeRequest(Player *player, Player *receiver, int32_t tradeid) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(0x02);
	packet.add<int8_t>(0x03);
	packet.addString(player->getName());
	packet.add<int32_t>(tradeid);
	receiver->getSession()->send(packet);
}

void TradesPacket::sendTradeMessage(Player *player, Player *receiver, int8_t type, int8_t message) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(type);
	packet.add<int8_t>(message);
	packet.addString(player->getName());
	receiver->getSession()->send(packet);
}

void TradesPacket::sendTradeMessage(Player *receiver, int8_t type, int8_t message) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(type);
	packet.add<int8_t>(0x00);
	packet.add<int8_t>(message);
	receiver->getSession()->send(packet);
}

void TradesPacket::sendTradeChat(Player *player, bool blue, const string &chat) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(0x06);
	packet.add<int8_t>(0x08);
	packet.addBool(blue);
	packet.addString(chat);
	player->getSession()->send(packet);
}

void TradesPacket::sendAddUser(Player *original, Player *newb, int8_t slot) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(0x04);
	packet.add<int8_t>(slot);
	PlayerPacketHelper::addPlayerDisplay(packet, newb);
	packet.addString(newb->getName());
	original->getSession()->send(packet);
}

void TradesPacket::sendLeaveTrade(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(0x0A);
	packet.add<int8_t>(0x01); // Slot, doesn't matter for trades
	packet.add<int8_t>(0x02); // Message, doesn't matter for trades
	player->getSession()->send(packet);
}

void TradesPacket::sendAddMesos(Player *receiver, uint8_t slot, int32_t amount) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(0x0F);
	packet.add<int8_t>(slot);
	packet.add<int32_t>(amount);
	receiver->getSession()->send(packet);
}

void TradesPacket::sendAccepted(Player *destination) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(0x10);
	destination->getSession()->send(packet);
}

void TradesPacket::sendAddItem(Player *destination, uint8_t player, uint8_t slot, Item *item) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_ROOM);
	packet.add<int8_t>(0x0E);
	packet.add<int8_t>(player);
	PlayerPacketHelper::addItemInfo(packet, slot, item);
	destination->getSession()->send(packet);
}