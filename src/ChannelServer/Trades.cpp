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
#include "Trades.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "Player.h"
#include "Players.h"
#include "ReadPacket.h"
#include "Timer.h"
#include "TradesPacket.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

hash_map<int, ActiveTrade *> Trades::trades;

class TradeTimer : public Timer::TimerHandler {
public:
	static TradeTimer * Instance() {
		if (singleton == 0)
			singleton = new TradeTimer;
		return singleton;
	}
	void setTradeTimer(Player *starter, Player *receiver, int id, int time) {
		TTimer timer;
		timer.id = Timer::Instance()->setTimer(time, this);
		timer.starter = starter;
		timer.receiver = receiver;
		timer.tradeid = id;
		timer.time = time;
		timers.push_back(timer);
	}
	void stop (Player *starter, Player *receiver) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (starter == timers[i].starter && receiver == timers[i].receiver) {
				Timer::Instance()->cancelTimer(timers[i].id);
				break;
			}
		}
	}
	bool isTiming (Player *starter, Player *receiver) {
		bool timeron = false;
		for (size_t i = 0; i < timers.size(); i++) {
			if (starter == timers[i].starter && receiver == timers[i].receiver) {
				timeron = true;
				break;
			}
		}
		return timeron;
	}
private:
	static TradeTimer *singleton;
	TradeTimer() {};
	TradeTimer(const TradeTimer&);
	TradeTimer& operator=(const TradeTimer&);
	struct TTimer {
		int id;
		Player *starter;
		Player *receiver;
		int tradeid;
		int time;
	};
	vector <TTimer> timers;
	void handle (Timer *timer, int id) {
		int tradeid;
		Player *starter;
		Player *receiver;
		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {
				starter = timers[i].starter;
				receiver = timers[i].receiver;
				tradeid = timers[i].tradeid;
				break;
			}
		}
		Trades::timeout(starter, receiver, tradeid);
	}
	void remove (int id) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {	
				timers.erase(timers.begin()+i);	
				return;
			}
		}
	}
};
TradeTimer * TradeTimer::singleton = 0;

void Trades::tradeHandler(Player *player, ReadPacket *packet) {
	unsigned char subopcode = packet->getByte();
	switch (subopcode) {
		case 0x00: { // Open trade - this usually comes with 03 00 - no clue why
			vector<Player *> players;
			vector<unsigned char> pos;
			players.push_back(player);
			pos.push_back(0xFF);			
			TradesPacket::sendOpenTrade(player, players, pos);
			break;
		}
		case 0x02: {
			if (player->isTrading() == 0) {  // Send trade request
				int playerid = packet->getInt();
				Player *receiver = Players::players[playerid];
				switch (receiver->isTrading()) {
					case -1: // Has a trade request already, this doesn't matter in global at the moment
					case 0: {
						ActiveTrade *trade = new ActiveTrade(player, receiver);
						Trades::addTrade(trade);
						player->setTrading(1); // Busy
						player->setTradeSendID(receiver->getPlayerid());
						receiver->setTrading(-1); // Handling request
						receiver->setTradeRecvID(player->getPlayerid());
						TradesPacket::sendTradeRequest(player, receiver, trade->getID());
						Trades::startTimeout(player, receiver, player->getPlayerid());
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
			int tradeid = packet->getInt();
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
				TradesPacket::sendTradeMessage(receiver, sender, 0x03, packet->getByte());
			}
			break;
		}
		case 0x04: {
			int tradeid = packet->getInt();
			ActiveTrade *trade = Trades::getTrade(tradeid);
			if (trade != 0) {
				Player *one = trade->getStarter()->player;
				Player *two = trade->getReceiver()->player;
				TradesPacket::sendAddUser(one, two, 0x01);
				two->setTrading(1);
				vector <Player *> players;
				vector <unsigned char> pos;
				players.push_back(one);
				players.push_back(two);
				pos.push_back(0x01);
				pos.push_back(0xFF);
				TradesPacket::sendOpenTrade(player, players, pos);
				Trades::stopTimeout(one, two);
			}
			else // Pool's closed, AIDS
				TradesPacket::sendTradeMessage(player, 0x05, 0x01);
			break;
		}
		case 0x06: { // Chat in a trade
			string chat = player->getName();
			chat.append(" : ");
			chat.append(packet->getString());
			int playerid = player->getPlayerid();
			bool isself = false;
			if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) // Receiver chatting
				playerid = player->getTradeRecvID();
			ActiveTrade *trade = Trades::getTrade(playerid);
			Player *one = trade->getStarter()->player;
			Player *two = trade->getReceiver()->player;
			unsigned char blue = 0x00;
			if (player->getPlayerid() == two->getPlayerid())
				blue = 0x01;
			TradesPacket::sendTradeChat(one, blue, chat);
			TradesPacket::sendTradeChat(two, blue, chat);
			break;
		}
		case 0x0A:
			Trades::cancelTrade(player);
			break;
		case 0x0E: { // Add items
			int playerid = player->getPlayerid();
			bool isreceiver = false;
			if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) { // Receiver
				playerid = player->getTradeRecvID();
				isreceiver = true;
			}
			ActiveTrade *trade = Trades::getTrade(playerid);
			Player *one = trade->getStarter()->player;
			Player *two = trade->getReceiver()->player;
			TradeInfo *send = trade->getStarter();
			TradeInfo *recv = trade->getReceiver();
			char inventory = packet->getByte();
			short slot = packet->getShort();
			short amount = packet->getShort();
			char addslot = packet->getByte();
			Item *use;
			Item *item;
			unsigned char user = 0x00;
			if (player->getPlayerid() == two->getPlayerid())
				user = 0x01;
			if (isreceiver) {
				item = two->getInventory()->getItem(inventory, slot);
				use = new Item(item);
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
			int playerid = player->getPlayerid();
			bool isreceiver = false;
			if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) { // Receiver
				playerid = player->getTradeRecvID();
				isreceiver = true;
			}
			ActiveTrade *trade = Trades::getTrade(playerid);
			Player *one = trade->getStarter()->player;
			Player *two = trade->getReceiver()->player;
			int amount = packet->getInt();
			int mesos = trade->getStarter()->mesos;
			if (player->getPlayerid() == two->getPlayerid()) {
				mesos = trade->getReceiver()->mesos;
				mesos += amount;
				trade->getReceiver()->mesos = mesos;
				two->getInventory()->setMesos(two->getInventory()->getMesos() - amount);
				InventoryPacket::blankUpdate(two); // TODO: Remove when we have a better client
				TradesPacket::sendAddMesos(one, 0x01, mesos);
				TradesPacket::sendAddMesos(two, 0x00, mesos);
			}
			else {
				mesos += amount;
				trade->getStarter()->mesos = mesos;
				one->getInventory()->setMesos(one->getInventory()->getMesos() - amount);
				InventoryPacket::blankUpdate(one); // TODO: Remove when we have a better client
				TradesPacket::sendAddMesos(one, 0x00, mesos);
				TradesPacket::sendAddMesos(two, 0x01, mesos);
			}
			break;
		}
		case 0x10: { // Accept trade
			int playerid = player->getPlayerid();
			bool isreceiver = false;
			if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) { // Receiver leaving
				playerid = player->getTradeRecvID();
				isreceiver = true;
			}
			ActiveTrade *trade = Trades::getTrade(playerid);
			Player *one = trade->getStarter()->player;
			Player *two = trade->getReceiver()->player;
			bool finish = false;
			if (player->getPlayerid() == two->getPlayerid()) {
				trade->getReceiver()->accepted = true;
				TradesPacket::sendAccepted(one);
				if (trade->getStarter()->accepted)
					finish = true;
			}
			else {
				trade->getStarter()->accepted = true;
				TradesPacket::sendAccepted(two);
				if (trade->getReceiver()->accepted)
					finish = true;
			}
			if (finish) { // Do trade processing
				TradeInfo *send = trade->getStarter();
				TradeInfo *recv = trade->getReceiver();
				int sendermesos = one->getInventory()->getMesos();
				int receivermesos = two->getInventory()->getMesos();
				bool fail = false;
				unsigned int comparison = send->mesos + receivermesos;
				if (comparison > 2147483647)
					fail = true;
				comparison = recv->mesos + sendermesos;
				if (comparison > 2147483647 && !fail)
					fail = true;
				if (send->count > 0 && !fail) // Determine if the opposite can accept all the items
					if (!(Trades::canTrade(one, send)))
						fail = true;
				if (recv->count > 0 && !fail) 
					if (!(Trades::canTrade(two, recv)))
						fail = true;
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
						int added = send->mesos;
						float taxrate = Trades::getTaxLevel(added);
						if (added > 49999)
							added = (int)(added / (1.0 + (taxrate / 100.0)));
						two->getInventory()->setMesos(receivermesos + added);
					}
					if (recv->mesos > 0) {
						int added = recv->mesos;
						float taxrate = Trades::getTaxLevel(added);
						if (added > 49999)
							added = (int)(added / (1.0 + (taxrate / 100.0)));
						one->getInventory()->setMesos(sendermesos + added);					
					}
					if (send->count > 0) {
						for (char i = 0; i < 9; i++) {
							if (send->slot[i]) {
								Item *item = send->items[i];
								Inventory::addItem(two, new Item(item));
								delete item;
							}
						}
					}
					if (recv->count > 0) {
						for (char i = 0; i < 9; i++) {
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
	int id = trade->getStarter()->player->getPlayerid();
	trade->setID(id);
	trades[id] = trade;
}

void Trades::removeTrade(int id) {
	if (trades.find(id) != trades.end())
			trades.erase(id);
}

ActiveTrade * Trades::getTrade(int id) {
	if (trades.find(id) != trades.end())
		return trades[id];
	return 0;
}

float Trades::getTaxLevel(int mesos) {
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
	int playerid = player->getPlayerid();
	bool isreceiver = false;
	if (!(player->getTradeSendID() > 0) && (player->getTradeRecvID() > 0)) { // Receiver leaving
		playerid = player->getTradeRecvID();
		isreceiver = true;
	}
	ActiveTrade *trade = Trades::getTrade(playerid);
	if (trade != 0) {
		Player *one = trade->getStarter()->player;
		Player *two = trade->getReceiver()->player;
		TradeInfo *send = trade->getStarter();
		TradeInfo *recv = trade->getReceiver();
		if (isreceiver || (!isreceiver && two->isTrading() == 1)) { // Left while in trade, give items back
			if (isreceiver)
				TradesPacket::sendLeaveTrade(one);
			else
				TradesPacket::sendLeaveTrade(two);
			Trades::returnItems(one, send);
			Trades::returnItems(two, recv);
			Trades::returnMesos(one, send);
			Trades::returnMesos(one, recv);
		}
		Trades::removeTrade(playerid);
		one->setTrading(0);
		one->setTradeSendID(0);
		two->setTrading(0);
		two->setTradeRecvID(0);
		if (TradeTimer::Instance()->isTiming(one, two))
			Trades::stopTimeout(one, two);
	}
}

bool Trades::canTrade(Player *player, TradeInfo *info) {
	bool yes = true;
	char amounts[4] = {0};
	for (char i = 0; i < 9; i++) {
		if (info->slot[i])
			amounts[GETINVENTORY(info->items[i]->id) - 1]++;
	}
	for (char i = 0; i < 4; i++) {
		if (amounts[i] > 0) {
			char incrementor = 0;
			for (char g = 0; g < player->getInventory()->getMaxSlots(i + 1); i++) { 
				if (player->getInventory()->getItem(i + 1, g) == 0)
					incrementor++;
				if (incrementor >= amounts[i])
					break;
			}
			if (incrementor < amounts[i]) {
				yes = false;
				break;
			}
		}
	}
	return yes;
}

void Trades::returnItems(Player *player, TradeInfo *info) {
	if (info->count > 0) {
		for (char i = 0; i < 9; i++) {
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
		player->getInventory()->setMesos(player->getInventory()->getMesos() + info->mesos);
}

void Trades::timeout(Player *starter, Player *receiver, int tradeid) {
	Trades::cancelTrade(starter);
}

void Trades::stopTimeout(Player *starter, Player *receiver) {
	TradeTimer::Instance()->stop(starter, receiver);
}

void Trades::startTimeout(Player *starter, Player *receiver, int tradeid) {
	TradeTimer::Instance()->setTradeTimer(starter, receiver, tradeid, 180000);
}
