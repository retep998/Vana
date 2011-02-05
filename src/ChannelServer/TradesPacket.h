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

class Item;
class Player;

namespace TradesPacket {
	namespace Messages {
		enum DenyTradeMessages {
			UnableToFind = 0x01,
			DoingSomethingElse = 0x02,
			Rejected = 0x03,
			TradesDisabled = 0x04
		};
		enum ShopEntryMessages {
			NoTradesWhileDead = 0x00, // This message uses the overload that takes 2 Player *s
			RoomAlreadyClosed = 0x01, // ALL the others use the overload with 1
			RoomAlreadyFull = 0x02,
			HandlingOtherRequests = 0x03,
			NoTradesWhileDead2 = 0x04,
			NoTradesDuringEvents = 0x05,
			UnableToTrade = 0x06,
			UnableToTradeOtherItems = 0x07, // No clue...
			RequiresSameMap = 0x09,
			NoStoresNearPortal = 0x0A,
			NoMinigameRoom = 0x0B,
			NoGame = 0x0C,
			NoShop = 0x0D,
			CutOffMessage = 0x0E, // 'oom 7~22, located at the 2nd floor and above of th'
			BannedFromShop = 0x0F,
			UndergoingMaintenance = 0x10,
			NoTournamentRoom = 0x11,
			UnableToTradeOtherItems2 = 0x12, // No clue...
			NotEnoughMesos = 0x13,
			IncorrectPassword = 0x14
		};
		enum EndTradeMessages {
			Success = 0x06,
			Failure = 0x07,
			TradeHasLimitedItems = 0x08, // Can't carry more than 1
			PlayerIsOnDifferentMap = 0x09
		};
	}
	namespace MessageTypes {
		enum Types {
			DenyTrade = 0x03,
			ShopEntryMessages = 0x05,
			EndTrade = 0x0A
		};
	}
	void sendOpenTrade(Player *player, Player *player1, Player *player2);
	void sendTradeRequest(Player *player, Player *receiver, int32_t tradeid);
	void sendTradeMessage(Player *player, Player *receiver, int8_t type, int8_t message);
	void sendTradeMessage(Player *receiver, int8_t type, int8_t message);
	void sendTradeChat(Player *player, bool blue, const string &chat);
	void sendLeaveTrade(Player *player);
	void sendAddUser(Player *original, Player *newb, int8_t slot);
	void sendAddMesos(Player *receiver, uint8_t slot, int32_t amount);
	void sendAccepted(Player *desintation);
	void sendAddItem(Player *destination, uint8_t player, uint8_t slot, Item *item);
}
