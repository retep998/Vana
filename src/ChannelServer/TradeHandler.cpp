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
#include "TradeHandler.h"
#include "GameLogicUtilities.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PacketReader.h"
#include "Timer/Thread.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include "Trade.h"
#include "Trades.h"
#include "TradesPacket.h"
#include <functional>
#include <string>

using std::tr1::bind;
using std::string;

void TradeHandler::tradeHandler(Player *player, PacketReader &packet) {
	uint8_t subopcode = packet.get<int8_t>();
	switch (subopcode) {
		case 0x00: // Open trade - this usually comes with 03 00 - no clue why
			TradesPacket::sendOpenTrade(player, player, 0);
			break;
		case 0x02: { // Send trade request
			if (player->isTrading()) {
				// most likely hacking
				return;
			}
			int32_t recvid = packet.get<int32_t>();
			Player *receiver = PlayerDataProvider::Instance()->getPlayer(recvid);
			if (receiver != 0) {
				if (!receiver->isTrading())
					TradesPacket::sendTradeRequest(player, receiver, Trades::Instance()->newTrade(player, receiver));
				else
					TradesPacket::sendTradeMessage(receiver, player, 0x03, 0x02);
			}
			break;
		}
		case 0x03: { // Deny request - trade ID + message ID
			int32_t tradeid = packet.get<int32_t>();
			ActiveTrade *trade = Trades::Instance()->getTrade(tradeid);
			if (trade != 0) {
				Player *one = trade->getSender();
				Player *two = trade->getReceiver();
				TradeHandler::removeTrade(tradeid);
				TradesPacket::sendTradeMessage(two, one, 0x03, packet.get<int8_t>());
			}
			break;
		}
		case 0x04: { // Accept request
			int32_t tradeid = packet.get<int32_t>();
			ActiveTrade *trade = Trades::Instance()->getTrade(tradeid);
			if (trade != 0) {
				Player *one = trade->getSender();
				Player *two = trade->getReceiver();
				two->setTrading(true);
				TradesPacket::sendAddUser(one, two, 0x01);
				TradesPacket::sendOpenTrade(player, two, one);
				Trades::Instance()->stopTimeout(tradeid);
			}
			else { // Pool's closed, AIDS
				TradesPacket::sendTradeMessage(player, 0x05, 0x01);
			}
			break;
		}
		case 0x06: { // Chat in a trade
			ActiveTrade *trade = Trades::Instance()->getTrade(player->getTradeId());
			if (trade == 0) {
				// Hacking
				return;
			}
			Player *one = trade->getSender();
			Player *two = trade->getReceiver();
			uint8_t blue = (player == two ? 0x01 : 0x00);
			string chat = player->getName() + " : " + packet.getString();
			TradesPacket::sendTradeChat(one, blue, chat);
			if (two != 0)
				TradesPacket::sendTradeChat(two, blue, chat);
			break;
		}
		case 0x0a:
			TradeHandler::cancelTrade(player);
			break;
		case 0x0e: // Add items
		case 0x0f: // Add mesos
		case 0x10: { // Accept trade
			int32_t tradeid = player->getTradeId();
			ActiveTrade *trade = Trades::Instance()->getTrade(tradeid);
			if (trade == 0) {
				// most likely hacking
				return;
			}
			TradeInfo *send = trade->getSenderTrade();
			TradeInfo *recv = trade->getReceiverTrade();
			Player *one = trade->getSender();
			Player *two = trade->getReceiver();
			bool isreceiver = (player == two);
			switch (subopcode) {
				case 0x0e: {
					int8_t inventory = packet.get<int8_t>();
					int16_t slot = packet.get<int16_t>();
					int16_t amount = packet.get<int16_t>();
					int8_t addslot = packet.get<int8_t>();
					Item *item = player->getInventory()->getItem(inventory, slot);
					if (item == 0 || (!isreceiver && trade->isItemInSlot(send, addslot)) || (isreceiver && trade->isItemInSlot(recv, addslot))) {
						// Hacking
						return;
					}
					if (GameLogicUtilities::isRechargeable(item->id)) {
						amount = item->amount;
					}
					else if (amount > item->amount || amount < 0) {
						// Hacking
						amount = item->amount;
					}
					Item *use = trade->addItem(player, isreceiver ? recv : send, item, addslot, slot, inventory, amount);
					TradesPacket::sendAddItem(one, (isreceiver ? 0x01 : 0x00), addslot, inventory, use);
					TradesPacket::sendAddItem(two, (isreceiver ? 0x00 : 0x01), addslot, inventory, use);
					break;
				}
				case 0x0f: { // Add mesos
					int32_t amount = packet.get<int32_t>();
					if (player->getInventory()->getMesos() < amount || amount < 0) {
						// Hacking
						return;
					}
					int32_t mesos = trade->addMesos(player, isreceiver ? recv : send, amount);
					TradesPacket::sendAddMesos(one, (isreceiver ? 0x01 : 0x00), mesos);
					TradesPacket::sendAddMesos(two, (isreceiver ? 0x00 : 0x01), mesos);
					break;
				}
				case 0x10: { // Accept trade
					trade->accept(isreceiver ? recv : send);
					TradesPacket::sendAccepted(isreceiver ? one : two);
					if (trade->bothAccepted()) { // Do trade processing
						bool fail = !trade->bothCanTrade();
						if (fail)
							trade->returnTrade();
						else
							trade->swapTrade();
						TradesPacket::sendEndTrade(one, (fail ? 0x07 : 0x06));
						TradesPacket::sendEndTrade(two, (fail ? 0x07 : 0x06));
						TradeHandler::removeTrade(tradeid);
					}
					break;
				}
			}
			break;
		}
	}
}

void TradeHandler::cancelTrade(Player *player) {
	int32_t tradeid = player->getTradeId();
	ActiveTrade *trade = Trades::Instance()->getTrade(tradeid);
	if (trade != 0) {
		Player *one = trade->getSender();
		Player *two = trade->getReceiver();
		bool isreceiver = (player == two);
		if (isreceiver || (!isreceiver && two != 0 && two->getTradeId() == one->getTradeId())) { // Left while in trade, give items back
			TradesPacket::sendLeaveTrade(isreceiver ? one : two);
			trade->returnTrade();
		}
		TradeHandler::removeTrade(tradeid);
	}
}

void TradeHandler::removeTrade(int32_t id) {
	Trades::Instance()->removeTrade(id);
}