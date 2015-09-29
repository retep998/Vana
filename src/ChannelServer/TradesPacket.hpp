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
#pragma once

#include "Common/PacketBuilder.hpp"
#include "Common/Types.hpp"
#include <string>

namespace Vana {
	class Item;

	namespace ChannelServer {
		class Player;

		namespace Packets {
			namespace Trades {
				namespace Messages {
					enum DenyTradeMessages : int8_t {
						UnableToFind = 0x01,
						DoingSomethingElse = 0x02,
						Rejected = 0x03,
						TradesDisabled = 0x04
					};
					enum ShopEntryMessages : int8_t {
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
					enum EndTradeMessages : int8_t {
						Success = 0x06,
						Failure = 0x07,
						TradeHasLimitedItems = 0x08, // Can't carry more than 1
						PlayerIsOnDifferentMap = 0x09
					};
				}
				namespace MessageTypes {
					enum Types : int8_t {
						DenyTrade = 0x03,
						ShopEntryMessages = 0x05,
						EndTrade = 0x0A
					};
				}

				PACKET(sendOpenTrade, Player *player1, Player *player2);
				PACKET(sendTradeRequest, const string_t &name, trade_id_t tradeId);
				PACKET(sendTradeMessage, const string_t &name, int8_t type, int8_t message);
				PACKET(sendEndTrade, int8_t message);
				PACKET(sendTradeEntryMessage, int8_t message);
				PACKET(sendTradeChat, bool blue, const string_t &chat);
				PACKET(sendLeaveTrade);
				PACKET(sendAddUser, Player *newPlayer, int8_t slot);
				PACKET(sendAddMesos, uint8_t slot, mesos_t amount);
				PACKET(sendAccepted);
				PACKET(sendAddItem, uint8_t player, uint8_t slot, Item *item);
			}
		}
	}
}