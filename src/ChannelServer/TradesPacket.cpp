/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "TradesPacket.hpp"
#include "Inventory.hpp"
#include "Player.hpp"
#include "PlayerPacketHelper.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace Vana {
namespace Packets {
namespace Trades {

PACKET_IMPL(sendOpenTrade, Player *player1, Player *player2) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x05)
		.add<int8_t>(0x03)
		.add<int8_t>(0x02)
		.add<int16_t>((player1 != nullptr && player2 != nullptr) ? 1 : 0);

	if (player2 != nullptr) {
		builder
			.addBuffer(Helpers::addPlayerDisplay(player2))
			.add<string_t>(player2->getName())
			.add<int8_t>(1); // Location in the window
	}
	if (player1 != nullptr) {
		builder
			.addBuffer(Helpers::addPlayerDisplay(player1))
			.add<string_t>(player1->getName())
			.add<int8_t>(-1); // Location in the window
	}
	return builder;
}

PACKET_IMPL(sendTradeRequest, const string_t &name, trade_id_t tradeId) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x02)
		.add<int8_t>(0x03)
		.add<string_t>(name)
		.add<trade_id_t>(tradeId);
	return builder;
}

PACKET_IMPL(sendTradeMessage, const string_t &name, int8_t type, int8_t message) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(type)
		.add<int8_t>(message)
		.add<string_t>(name);
	return builder;
}

PACKET_IMPL(sendEndTrade, int8_t message) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(Packets::Trades::MessageTypes::EndTrade)
		.add<int8_t>(0x00)
		.add<int8_t>(message);
	return builder;
}

PACKET_IMPL(sendTradeEntryMessage, int8_t message) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(Packets::Trades::MessageTypes::ShopEntryMessages)
		.add<int8_t>(0x00)
		.add<int8_t>(message);
	return builder;
}

PACKET_IMPL(sendTradeChat, bool blue, const string_t &chat) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x06)
		.add<int8_t>(0x08)
		.add<bool>(blue)
		.add<string_t>(chat);
	return builder;
}

PACKET_IMPL(sendAddUser, Player *newPlayer, int8_t slot) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x04)
		.add<int8_t>(slot)
		.addBuffer(Helpers::addPlayerDisplay(newPlayer))
		.add<string_t>(newPlayer->getName());
	return builder;
}

PACKET_IMPL(sendLeaveTrade) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x0A)
		.add<int8_t>(0x01) // Slot, doesn't matter for trades
		.add<int8_t>(0x02); // Message, doesn't matter for trades
	return builder;
}

PACKET_IMPL(sendAddMesos, uint8_t slot, mesos_t amount) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x0F)
		.add<int8_t>(slot)
		.add<mesos_t>(amount);
	return builder;
}

PACKET_IMPL(sendAccepted) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x10);
	return builder;
}

PACKET_IMPL(sendAddItem, uint8_t player, uint8_t slot, Item *item) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_ROOM)
		.add<int8_t>(0x0E)
		.add<int8_t>(player)
		.addBuffer(Helpers::addItemInfo(slot, item));
	return builder;
}

}
}
}