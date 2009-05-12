/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "Player.h"
#include "Players.h"
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
			if (player->isTrading() == 0) {
				int32_t playerid = packet.get<int32_t>();
				Player *receiver = Players::Instance()->getPlayer(playerid);
				switch (receiver->isTrading()) {
					case -1: // Has a trade request already, this doesn't matter in global at the moment
					case 0: {
						ActiveTrade *trade = new ActiveTrade(player, receiver);
						Trades::Instance()->addTrade(trade);
						player->setTrading(1); // Busy
						player->setTradeSendId(receiver->getId());
						receiver->setTrading(-1); // Handling request
						receiver->setTradeRecvId(player->getId());
						TradesPacket::sendTradeRequest(player, receiver, trade->getId());
						TradeHandler::startTimeout(player, receiver, trade->getId());
						break;
					}
					case 1: // Busy
						TradesPacket::sendTradeMessage(receiver, player, 0x03, 0x02);
						break;
				}
			}
			break;
		}
		case 0x03: { // Deny request - trade ID + message ID
			int32_t tradeid = packet.get<int32_t>();
			ActiveTrade *trade = Trades::Instance()->getTrade(tradeid);
			if (trade != 0) {
				TradeInfo *tradesend = trade->getStarter();
				TradeInfo *traderecv = trade->getReceiver();
				Player *one = tradesend->player;
				Player *two = traderecv->player;
				removeTrade(tradeid, one, two);
				TradesPacket::sendTradeMessage(two, one, 0x03, packet.get<int8_t>());
			}
			break;
		}
		case 0x04: { // Accept request
			int32_t tradeid = packet.get<int32_t>();
			ActiveTrade *trade = Trades::Instance()->getTrade(tradeid);
			if (trade != 0) {
				Player *one = trade->getStarter()->player;
				Player *two = trade->getReceiver()->player;
				TradesPacket::sendAddUser(one, two, 0x01);
				two->setTrading(1);
				TradesPacket::sendOpenTrade(player, two, one);
				TradeHandler::stopTimeout(one, two);
			}
			else { // Pool's closed, AIDS
				TradesPacket::sendTradeMessage(player, 0x05, 0x01);
			}
			break;
		}
		case 0x06: { // Chat in a trade
			string chat = player->getName() + " : " + packet.getString();
			int32_t playerid = player->getId();
			if (!(player->getTradeSendId() > 0) && (player->getTradeRecvId() > 0)) // Receiver chatting
				playerid = player->getTradeRecvId();
			ActiveTrade *trade = Trades::Instance()->getTrade(playerid);
			Player *one = trade->getStarter()->player;
			Player *two = trade->getReceiver()->player;
			uint8_t blue = (player == two ? 0x01 : 0x00);
			TradesPacket::sendTradeChat(one, blue, chat);
			TradesPacket::sendTradeChat(two, blue, chat);
			break;
		}
		case 0x0a:
			TradeHandler::cancelTrade(player);
			break;
		case 0x0e: { // Add items
			int32_t playerid = player->getId();
			bool isreceiver = false;
			if (!(player->getTradeSendId() > 0) && (player->getTradeRecvId() > 0)) { // Receiver
				playerid = player->getTradeRecvId();
				isreceiver = true;
			}
			ActiveTrade *trade = Trades::Instance()->getTrade(playerid);
			TradeInfo *send = trade->getStarter();
			TradeInfo *recv = trade->getReceiver();
			Player *one = send->player;
			Player *two = recv->player;
			int8_t inventory = packet.get<int8_t>();
			int16_t slot = packet.get<int16_t>();
			int16_t amount = packet.get<int16_t>();
			int8_t addslot = packet.get<int8_t>();
			Item *item = player->getInventory()->getItem(inventory, slot);
			if (item == 0) {
				// Hacking, most likely
				return;
			}
			Item *use = new Item(item);
			if ((!isreceiver && send->slot[addslot - 1]) || (isreceiver && recv->slot[addslot - 1])) {
				// Hacking
				delete use;
				return;
			}

			if (GameLogicUtilities::isRechargeable(item->id)) {
				amount = item->amount;
			}
			else if (amount > item->amount || amount < 0) {
				// Hacking
				amount = item->amount;
			}

			if (amount == item->amount || inventory == 1) {
				player->getInventory()->setItem(inventory, slot, 0);
				InventoryPacket::moveItem(player, inventory, slot, 0);
				player->getInventory()->deleteItem(inventory, slot);
			}
			else {
				item->amount -= amount;
				player->getInventory()->changeItemAmount(item->id, item->amount);
				InventoryPacket::updateItemAmounts(player, inventory, slot, item->amount, 0, 0);
				use->amount = amount;
			}
			if (isreceiver) {
				recv->items[addslot - 1] = use;
				recv->count = recv->count + 1;
				recv->slot[addslot - 1] = true;
			}
			else {
				send->items[addslot - 1] = use;
				send->count = send->count + 1;
				send->slot[addslot - 1] = true;
			}
			TradesPacket::sendAddItem(one, (isreceiver ? 0x01 : 0x00), addslot, inventory, use);
			TradesPacket::sendAddItem(two, (isreceiver ? 0x00 : 0x01), addslot, inventory, use);
			break;
		}
		case 0x0f: { // Add mesos
			int32_t playerid = player->getId();
			bool isreceiver = false;
			if (!(player->getTradeSendId() > 0) && (player->getTradeRecvId() > 0)) { // Receiver
				playerid = player->getTradeRecvId();
				isreceiver = true;
			}
			ActiveTrade *trade = Trades::Instance()->getTrade(playerid);
			TradeInfo *send = trade->getStarter();
			TradeInfo *recv = trade->getReceiver();
			Player *one = send->player;
			Player *two = recv->player;
			int32_t amount = packet.get<int32_t>();
			int32_t mesos = 0;
			if (player->getInventory()->getMesos() < amount || amount < 0) {
				// Hacking
				return;
			}
			if (player == two) {
				recv->mesos += amount;
				mesos = recv->mesos;
				two->getInventory()->modifyMesos(-amount);
			}
			else {
				send->mesos += amount;
				mesos = send->mesos;
				one->getInventory()->modifyMesos(-amount);
			}
			TradesPacket::sendAddMesos(one, (player == two ? 0x01 : 0x00), mesos);
			TradesPacket::sendAddMesos(two, (player == two ? 0x00 : 0x01), mesos);
			break;
		}
		case 0x10: { // Accept trade
			int32_t playerid = player->getId();
			bool isreceiver = false;
			if (!(player->getTradeSendId() > 0) && (player->getTradeRecvId() > 0)) { // Receiver leaving
				playerid = player->getTradeRecvId();
				isreceiver = true;
			}
			ActiveTrade *trade = Trades::Instance()->getTrade(playerid);
			TradeInfo *send = trade->getStarter();
			TradeInfo *recv = trade->getReceiver();
			Player *one = send->player;
			Player *two = recv->player;
			bool finish = false;
			if (player == two) {
				recv->accepted = true;
				TradesPacket::sendAccepted(one);
				if (send->accepted)
					finish = true;
			}
			else {
				send->accepted = true;
				TradesPacket::sendAccepted(two);
				if (recv->accepted)
					finish = true;
			}
			if (finish) { // Do trade processing
				int32_t sendermesos = one->getInventory()->getMesos();
				int32_t receivermesos = two->getInventory()->getMesos();
				bool fail = false;
				int32_t comparison = send->mesos + receivermesos;
				if (comparison < 0) {// Determine if receiver can receive all the mesos
					fail = true;
				}
				comparison = recv->mesos + sendermesos;
				if (comparison < 0 && !fail) { // Determine if sender can receive all the mesos
					fail = true;
				}
				if (send->count > 0 && !fail) { // Determine if receiver can receive all the items
					fail = (!(TradeHandler::canTrade(two, send)));
				}
				if (recv->count > 0 && !fail) { // Determine if sender can receive all the items
					fail = (!(TradeHandler::canTrade(one, recv)));
				}
				if (fail) { // One or the other doesn't have enough space or mesos are ridiculous
					TradeHandler::giveItems(one, send);
					TradeHandler::giveItems(two, recv);
					TradeHandler::giveMesos(one, send);
					TradeHandler::giveMesos(two, recv);
				}
				else {
					TradeHandler::giveItems(one, recv);
					TradeHandler::giveItems(two, send);
					TradeHandler::giveMesos(one, recv, true);
					TradeHandler::giveMesos(two, send, true);						
				}
				TradesPacket::sendEndTrade(one, (fail ? 0x07 : 0x06));
				TradesPacket::sendEndTrade(two, (fail ? 0x07 : 0x06));
				removeTrade(playerid, one, two);
			}
			break;
		}
		default:
			break;
	}
}

int32_t TradeHandler::getTaxLevel(int32_t mesos) {
	if (mesos < 50000)
		return 0;
	if (mesos > 9999999)
		return 400;
	if (mesos > 4999999)
		return 300;
	if (mesos > 999999)
		return 200;
	if (mesos > 99999)
		return 100;
	return 50;
}

void TradeHandler::cancelTrade(Player *player) {
	int32_t playerid = player->getId();
	bool isreceiver = false;
	if (!(player->getTradeSendId() > 0) && (player->getTradeRecvId() > 0)) { // Receiver leaving
		playerid = player->getTradeRecvId();
		isreceiver = true;
	}
	ActiveTrade *trade = Trades::Instance()->getTrade(playerid);
	if (trade != 0) {
		TradeInfo *send = trade->getStarter();
		TradeInfo *recv = trade->getReceiver();
		Player *one = send->player;
		Player *two = recv->player;
		if (isreceiver || (!isreceiver && two->isTrading() == 1)) { // Left while in trade, give items back
			if (isreceiver)
				TradesPacket::sendLeaveTrade(one);
			else
				TradesPacket::sendLeaveTrade(two);
			TradeHandler::giveItems(one, send);
			TradeHandler::giveItems(two, recv);
			TradeHandler::giveMesos(one, send);
			TradeHandler::giveMesos(two, recv);
		}
		removeTrade(playerid, one, two);
		Timer::Id id(Timer::Types::TradeTimer, one->getId(), two->getId());
		if (Timer::Thread::Instance()->getContainer()->checkTimer(id)) {
			TradeHandler::stopTimeout(one, two);
		}
	}
}

bool TradeHandler::canTrade(Player *player, TradeInfo *info) {
	bool yes = true;
	int8_t totals[4] = {0};
	unordered_map<int32_t, int16_t> added;
	for (int8_t i = 0; i < 9; i++) {
		// Create item structure to determine needed slots among stackable items
		// Also, determine needed slots for nonstackables
		if (info->slot[i]) {
			Item *check = info->items[i];
			int32_t itemid = check->id;
			int8_t inv = GameLogicUtilities::getInventory(itemid);
			if (inv == 1 || GameLogicUtilities::isRechargeable(itemid)) // Equips and rechargeables always take 1 slot, no need to clutter unordered map
				totals[inv - 1]++;
			else {
				if (added.find(itemid) != added.end()) // Already initialized this item
					added[itemid] += check->amount;
				else
					added[itemid] = check->amount;
			}
		}
	}
	for (int8_t i = 0; i < 9; i++) { // Determine precisely how many slots are needed for stackables
		if (info->slot[i]) {
			Item *check = info->items[i];
			int32_t itemid = check->id;
			int8_t inv = GameLogicUtilities::getInventory(itemid);
			if (inv != 1 && !GameLogicUtilities::isRechargeable(itemid)) { // Already did these
				if (added.find(itemid) == added.end()) // Already did this item
					continue;
				int16_t maxslot = ItemDataProvider::Instance()->getMaxSlot(itemid);
				int32_t current_amount = player->getInventory()->getItemAmount(itemid);
				int32_t last_slot = (current_amount % maxslot); // Get the number of items in the last slot
				int32_t item_sum = last_slot + added[itemid];
				bool needslots = false;
				if (last_slot > 0) { // Items in the last slot, potential for needing slots
					if (item_sum > maxslot)
						needslots = true;
				}
				else // Full in the last slot, for sure need all slots
					needslots = true;
				if (needslots) {
					uint8_t numslots = (uint8_t)(item_sum / maxslot);
					uint8_t remainder = (uint8_t)(item_sum % maxslot);
					if (remainder > 0)
						totals[inv - 1]++;
					totals[inv - 1] += numslots;
				}
				added.erase(itemid);
			}
		}
	}
	for (int8_t i = 0; i < 4; i++) { // Determine if needed slots are available
		if (totals[i] > 0) {
			int8_t incrementor = 0;
			for (int8_t g = 1; g <= player->getInventory()->getMaxSlots(i + 1); g++) {
				if (player->getInventory()->getItem(i + 1, g) == 0)
					incrementor++;
				if (incrementor >= totals[i])
					break;
			}
			if (incrementor < totals[i]) {
				yes = false;
				break;
			}
		}
	}
	return yes;
}

void TradeHandler::giveItems(Player *player, TradeInfo *info) {
	if (info->count > 0) {
		for (int8_t i = 0; i < 9; i++) {
			if (info->slot[i]) {
				Item *item = info->items[i];
				Inventory::addItem(player, new Item(item));
				delete item;
			}
		}
	}
}

void TradeHandler::giveMesos(Player *player, TradeInfo *info, bool traded) {
	if (info->mesos > 0) {
		if (traded && info->mesos > 49999) {
			int64_t mesos = info->mesos * getTaxLevel(info->mesos) / 10000;
			info->mesos -= (int32_t)(mesos);
		}
		player->getInventory()->modifyMesos(info->mesos);
	}
}

void TradeHandler::removeTrade(int32_t id, Player *one, Player *two) {
	Trades::Instance()->removeTrade(id);
	one->setTrading(0);
	one->setTradeSendId(0);
	two->setTrading(0);
	two->setTradeRecvId(0);
}

void TradeHandler::timeout(Player *starter, Player *receiver, int32_t tradeid) {
	TradeHandler::cancelTrade(starter);
}

void TradeHandler::stopTimeout(Player *starter, Player *receiver) {
	Timer::Id id(Timer::Types::TradeTimer, starter->getId(), receiver->getId());
	Timer::Thread::Instance()->getContainer()->removeTimer(id);
}

void TradeHandler::startTimeout(Player *starter, Player *receiver, int32_t tradeid) {
	Timer::Id id(Timer::Types::TradeTimer, starter->getId(), receiver->getId());
	new Timer::Timer(bind(&TradeHandler::timeout, starter, receiver, tradeid),
		id, 0, Timer::Time::fromNow(180000));
}
