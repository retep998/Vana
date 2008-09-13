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
#ifndef TRADESPACKET_H
#define TRADESPACKET_H

#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class Player;
struct Item;

namespace TradesPacket {
	void sendOpenTrade(Player *player, const vector<Player *> &players, const vector<unsigned char> &pos);
	void sendTradeRequest(Player *player, Player *receiver, int32_t tradeid);
	void sendTradeMessage(Player *player, Player *receiver, char type, char message);
	void sendTradeMessage(Player *receiver, char type, char message);
	void sendTradeChat(Player *player, unsigned char blue, string chat);
	void sendLeaveTrade(Player *player);
	void sendAddUser(Player *original, Player *newb, char slot);
	void sendAddMesos(Player *receiver, unsigned char slot, int32_t amount);
	void sendAccepted(Player *desintation);
	void sendEndTrade(Player *destination, unsigned char message);
	void sendAddItem(Player *destination, unsigned char player, char slot, char inventory, Item *item);
}

#endif