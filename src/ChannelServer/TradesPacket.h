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
#pragma once

#include "Types.h"
#include <string>

using std::string;

class Player;
struct Item;

namespace TradesPacket {
	void sendOpenTrade(Player *player, Player *player1, Player *player2);
	void sendTradeRequest(Player *player, Player *receiver, int32_t tradeid);
	void sendTradeMessage(Player *player, Player *receiver, int8_t type, int8_t message);
	void sendTradeMessage(Player *receiver, int8_t type, int8_t message);
	void sendTradeChat(Player *player, uint8_t blue, const string &chat);
	void sendLeaveTrade(Player *player);
	void sendAddUser(Player *original, Player *newb, int8_t slot);
	void sendAddMesos(Player *receiver, uint8_t slot, int32_t amount);
	void sendAccepted(Player *desintation);
	void sendEndTrade(Player *destination, uint8_t message);
	void sendAddItem(Player *destination, uint8_t player, int8_t slot, int8_t inventory, Item *item);
}
