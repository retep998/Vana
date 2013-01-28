/*
Copyright (C) 2008-2013 Vana Development Team

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
#include <array>
#include <memory>

using std::unique_ptr;

class Item;
class Player;

struct TradeInfo {
	TradeInfo() : count(0), mesos(0), accepted(false) {
		for (uint8_t i = 0; i < TradeSize; i++) {
			slot[i] = false;
			items[i] = 0;
		}
	}

	const static uint8_t TradeSize = 9;

	int32_t mesos;
	uint8_t count;
	bool accepted;
	std::array<Item *, TradeSize> items;
	std::array<bool, TradeSize> slot;
};

class ActiveTrade {
public:
	ActiveTrade(Player *starter, Player *receiver, int32_t id);

	int32_t getId() const { return m_id; }
	TradeInfo * getSenderTrade() const { return m_sender.get(); }
	TradeInfo * getReceiverTrade() const { return m_receiver.get(); }

	// Wrapper functions using their IDs in case the pointers are now bad
	Player * getSender();
	Player * getReceiver();

	bool bothCanTrade();
	bool bothAccepted();
	void returnTrade();
	void swapTrade();
	void accept(TradeInfo *unit);
	int32_t addMesos(Player *holder, TradeInfo *unit, int32_t amount);
	Item * addItem(Player *holder, TradeInfo *unit, Item *item, uint8_t tradeSlot, int16_t inventorySlot, int8_t inventory, int16_t amount);
	bool isItemInSlot(TradeInfo *unit, uint8_t tradeSlot) { return (tradeSlot > TradeInfo::TradeSize ? true : unit->slot[tradeSlot - 1]); }
private:
	unique_ptr<TradeInfo> m_sender;
	unique_ptr<TradeInfo> m_receiver;
	int32_t m_id;
	int32_t m_senderId;
	int32_t m_receiverId;

	bool canTrade(Player *target, TradeInfo *unit);
	void giveItems(Player *target, TradeInfo *unit);
	void giveMesos(Player *player, TradeInfo *info, bool traded = false);
};