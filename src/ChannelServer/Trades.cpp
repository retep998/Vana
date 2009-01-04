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
#include "Trades.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "Player.h"
#include "Players.h"
#include "PacketReader.h"
#include "Timer/Thread.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include "TradesPacket.h"
#include <functional>
#include <string>

using std::tr1::bind;
using std::string;

unordered_map<int32_t, ActiveTrade *> Trades::trades;

void Trades::tradeHandler(Player *player, PacketReader &packet) {
	uint8_t subopcode = packet.getByte();
	switch (subopcode) {
		case 0x00: // Open trade - this usually comes with 03 00 - no clue why
			TradesPacket::sendOpenTrade(player, player, 0);
			break;
		case 0x02: {
			if (player->isTrading() == 0) {  // Send trade request
				int32_t playerid = packet.getInt();
				Player *receiver = Players::Instance()->getPlayer(playerid);
				switch (receiver->isTrading()) {
					case -1: // Has a trade request already, this doesn't matter in global at the moment
					case 0: {
						ActiveTrade *trade = new ActiveTrade(player, receiver);
						Trades::addTrade(trade);
						player->setTrading(1); // Busy
						player->setTradeSendID(receiver->getId());
						receiver->setTrading(-1); // Handling request
						receiver->setTradeRecvID(player->getId());
						TradesPacket::sendTradeRequest(player, receiver, trade->getID());
						Trades::startTimeout(player, receiver, player->getId());
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
			int32_t tradeid = packet.getInt();
			ActiveTrade *trade = Trades::getTrade(tradeid);
			if (trade != 0) {
				TradeInfo *tradesend = trade->getStarter();
				TradeInfo *traderecv = trade->getReceiver();
				Player *sender = tradesend->player;
				Player *receiver = traderecv->player;
				Trades::removeTrade(tradeid);
				sender->setTrading(0);
				sender->setTradeSendID(0);
				receiver->setTrading(0);
				receiver->setTradeRecvID(0);
				TradesPacket::sendTradeMessage(receiver, sender, 0x03, packet.getByte());
			}
			break;
		}
		case 0x04: {
			int32_t tradeid = packet.getInt();
			ActiveTrade *trade = Trades::getTrade(tradeid);
			if (trade != 0) {
				Player *one = trade->getStarter()->player;
				Player *two = trade->getReceiver()->player;
				TradesPacket::sendAddUser(one, two, 0x01);
				two->setTrading(1);
				TradesPacket::sendOpenTrade(player, two, one);
				Trades::stopTimeout(one, two);
			}
			else // Pool's closed, AIDS
				TradesPacket::sendTradeMessage(player, 0x05, 0x01);
			break;
		}
		case 0x06: { // Chat in a trade
			string chat = player->getName();
			chat.append(" : ");
			chat.append(packet.getString());
			int32_t playerid = player->getId();
			bool isself = false;
			if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) // Receiver chatting
				playerid = player->getTradeRecvID();
			ActiveTrade *trade = Trades::getTrade(playerid);
			Player *one = trade->getStarter()->player;
			Player *two = trade->getReceiver()->player;
			uint8_t blue = 0x00;
			if (player == two)
				blue = 0x01;
			TradesPacket::sendTradeChat(one, blue, chat);
			TradesPacket::sendTradeChat(two, blue, chat);
			break;
		}
		case 0x0A:
			Trades::cancelTrade(player);
			break;
		case 0x0E: { // Add items
			int32_t playerid = player->getId();
			bool isreceiver = false;
			if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) { // Receiver
				playerid = player->getTradeRecvID();
				isreceiver = true;
			}
			ActiveTrade *trade = Trades::getTrade(playerid);
			TradeInfo *send = trade->getStarter();
			TradeInfo *recv = trade->getReceiver();
			Player *one = send->player;
			Player *two = recv->player;
			int8_t inventory = packet.getByte();
			int16_t slot = packet.getShort();
			int16_t amount = packet.getShort();
			int8_t addslot = packet.getByte();
			Item *use;
			Item *item;
			uint8_t user = 0x00;
			if (player == two)
				user = 0x01;
			if (isreceiver) {
				item = two->getInventory()->getItem(inventory, slot);
				use = new Item(item);
				if (ISRECHARGEABLE(item->id))
					amount = item->amount;
				if (amount == item->amount || inventory == 1) {
					two->getInventory()->setItem(inventory, slot, 0);
					InventoryPacket::moveItem(player, inventory, slot, 0);
					two->getInventory()->deleteItem(inventory, slot);
				}
				else {
					item->amount -= amount;
					two->getInventory()->changeItemAmount(item->id, item->amount);
					InventoryPacket::updateItemAmounts(player, inventory, slot, item->amount, 0, 0);
					use->amount = amount;
				}
				recv->items[addslot - 1] = use;
				recv->count = recv->count + 1;
				recv->slot[addslot - 1] = true;
				TradesPacket::sendAddItem(one, 0x01, addslot, inventory, use);
				TradesPacket::sendAddItem(two, 0x00, addslot, inventory, use);
			}
			else {
				item = one->getInventory()->getItem(inventory, slot);
				use = new Item(item);
				if (ISRECHARGEABLE(item->id))
					amount = item->amount;
				if (amount == item->amount || inventory == 1) {
					one->getInventory()->setItem(inventory, slot, 0);
					InventoryPacket::moveItem(player, inventory, slot, 0);
					one->getInventory()->deleteItem(inventory, slot);
				}
				else {
					item->amount -= amount;
					one->getInventory()->changeItemAmount(item->id, item->amount);
					InventoryPacket::updateItemAmounts(player, inventory, slot, item->amount, 0, 0);
					use->amount = amount;
				}
				send->items[addslot - 1] = use;
				send->count = send->count + 1;
				send->slot[addslot - 1] = true;
				TradesPacket::sendAddItem(one, 0x00, addslot, inventory, use);
				TradesPacket::sendAddItem(two, 0x01, addslot, inventory, use);
			}
			break;
		}
		case 0x0F: { // Add mesos
			int32_t playerid = player->getId();
			bool isreceiver = false;
			if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) { // Receiver
				playerid = player->getTradeRecvID();
				isreceiver = true;
			}
			ActiveTrade *trade = Trades::getTrade(playerid);
			TradeInfo *send = trade->getStarter();
			TradeInfo *recv = trade->getReceiver();
			Player *one = send->player;
			Player *two = recv->player;
			int32_t amount = packet.getInt();
			int32_t mesos = send->mesos;
			if (player == two) {
				mesos = recv->mesos;
				mesos += amount;
				recv->mesos = mesos;
				two->getInventory()->modifyMesos(-amount);
				TradesPacket::sendAddMesos(one, 0x01, mesos);
				TradesPacket::sendAddMesos(two, 0x00, mesos);
			}
			else {
				mesos += amount;
				send->mesos = mesos;
				one->getInventory()->modifyMesos(-amount);
				TradesPacket::sendAddMesos(one, 0x00, mesos);
				TradesPacket::sendAddMesos(two, 0x01, mesos);
			}
			break;
		}
		case 0x10: { // Accept trade
			int32_t playerid = player->getId();
			bool isreceiver = false;
			if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) { // Receiver leaving
				playerid = player->getTradeRecvID();
				isreceiver = true;
			}
			ActiveTrade *trade = Trades::getTrade(playerid);
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
					fail = (!(Trades::canTrade(two, send)));
				}
				if (recv->count > 0 && !fail) { // Determine if sender can receive all the items
					fail = (!(Trades::canTrade(one, recv)));
				}
				if (fail) { // One or the other doesn't have enough space or mesos are ridiculous
					TradesPacket::sendEndTrade(one, 0x07);
					TradesPacket::sendEndTrade(two, 0x07);
					Trades::returnItems(one, send);
					Trades::returnItems(two, recv);
					Trades::returnMesos(one, send);
					Trades::returnMesos(two, recv);
					Trades::removeTrade(playerid);
					one->setTrading(0);
					one->setTradeSendID(0);
					two->setTrading(0);
					two->setTradeRecvID(0);
				}
				else {
					if (send->mesos > 0) {
						int32_t added = send->mesos;
						float taxrate = Trades::getTaxLevel(added);
						if (added > 49999)
							added = (int32_t)(added / (1.0 + (taxrate / 100.0)));
						two->getInventory()->setMesos(receivermesos + added);
					}
					if (recv->mesos > 0) {
						int32_t added = recv->mesos;
						float taxrate = Trades::getTaxLevel(added);
						if (added > 49999)
							added = (int32_t)(added / (1.0 + (taxrate / 100.0)));
						one->getInventory()->setMesos(sendermesos + added);
					}
					if (send->count > 0) {
						for (int8_t i = 0; i < 9; i++) {
							if (send->slot[i]) {
								Item *item = send->items[i];
								Inventory::addItem(two, new Item(item));
								delete item;
							}
						}
					}
					if (recv->count > 0) {
						for (int8_t i = 0; i < 9; i++) {
							if (recv->slot[i]) {
								Item *item = recv->items[i];
								Inventory::addItem(one, new Item(item));
								delete item;
							}
						}
					}
					TradesPacket::sendEndTrade(one, 0x06);
					TradesPacket::sendEndTrade(two, 0x06);
					Trades::removeTrade(playerid);
					one->setTrading(0);
					one->setTradeSendID(0);
					two->setTrading(0);
					two->setTradeRecvID(0);
				}
			}
			break;
		}
		default:
			break;
	}
}

void Trades::addTrade(ActiveTrade *trade) {
	int32_t id = trade->getStarter()->player->getId();
	trade->setID(id);
	trades[id] = trade;
}

void Trades::removeTrade(int32_t id) {
	if (trades.find(id) != trades.end())
		trades.erase(id);
}

ActiveTrade * Trades::getTrade(int32_t id) {
	return (trades.find(id) != trades.end() ? trades[id] : 0);
}

float Trades::getTaxLevel(int32_t mesos) {
	if (mesos < 50000)
		return 0.0;
	if (mesos > 9999999)
		return 4.0;
	if (mesos > 4999999)
		return 3.0;
	if (mesos > 999999)
		return 2.0;
	if (mesos > 99999)
		return 1.0;
	return 0.5;
}

void Trades::cancelTrade(Player *player) {
	int32_t playerid = player->getId();
	bool isreceiver = false;
	if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) { // Receiver leaving
		playerid = player->getTradeRecvID();
		isreceiver = true;
	}
	ActiveTrade *trade = Trades::getTrade(playerid);
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
			Trades::returnItems(one, send);
			Trades::returnItems(two, recv);
			Trades::returnMesos(one, send);
			Trades::returnMesos(two, recv);
		}
		Trades::removeTrade(playerid);
		one->setTrading(0);
		one->setTradeSendID(0);
		two->setTrading(0);
		two->setTradeRecvID(0);
		Timer::Id id(Timer::Types::TradeTimer, one->getId(), two->getId());
		if (Timer::Thread::Instance()->getContainer()->checkTimer(id)) {
			Trades::stopTimeout(one, two);
		}
	}
}

bool Trades::canTrade(Player *player, TradeInfo *info) {
	bool yes = true;
	int8_t totals[4] = {0};
	unordered_map<int32_t, int16_t> added;
	for (int8_t i = 0; i < 9; i++) {
		// Create item structure to determine needed slots among stackable items
		// Also, determine needed slots for nonstackables
		if (info->slot[i]) {
			Item *check = info->items[i];
			int32_t itemid = check->id;
			int8_t inv = GETINVENTORY(itemid);
			if (inv == 1 || ISRECHARGEABLE(itemid)) // Equips and rechargeables always take 1 slot, no need to clutter unordered map
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
			int8_t inv = GETINVENTORY(itemid);
			if (inv != 1 && !ISRECHARGEABLE(itemid)) { // Already did these
				if (added.find(itemid) == added.end()) // Already did this item
					continue;
				int16_t maxslot = ItemDataProvider::Instance()->getMaxslot(itemid);
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

void Trades::returnItems(Player *player, TradeInfo *info) {
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

void Trades::returnMesos(Player *player, TradeInfo *info) {
	if (info->mesos > 0)
		player->getInventory()->modifyMesos(info->mesos);
}

void Trades::timeout(Player *starter, Player *receiver, int32_t tradeid) {
	Trades::cancelTrade(starter);
}

void Trades::stopTimeout(Player *starter, Player *receiver) {
	Timer::Id id(Timer::Types::TradeTimer, starter->getId(), receiver->getId());
	Timer::Thread::Instance()->getContainer()->removeTimer(id);
}

void Trades::startTimeout(Player *starter, Player *receiver, int32_t tradeid) {
	Timer::Id id(Timer::Types::TradeTimer, starter->getId(), receiver->getId());
	new Timer::Timer(bind(&Trades::timeout, starter, receiver, tradeid),
		id, 0, Timer::Time::fromNow(180000));
}
