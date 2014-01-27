/*
Copyright (C) 2008-2014 Vana Development Team

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

class Item;
class Player;

struct TradeInfo {
	TradeInfo()
	{
		for (uint8_t i = 0; i < TradeSize; i++) {
			slot[i] = false;
			items[i] = 0;
		}
	}

	const static uint8_t TradeSize = 9;

	bool accepted = false;
	uint8_t count = 0;
	int32_t mesos = 0;
	array_t<Item *, TradeSize> items;
	array_t<bool, TradeSize> slot;
};

class ActiveTrade {
	NONCOPYABLE(ActiveTrade);
	NO_DEFAULT_CONSTRUCTOR(ActiveTrade);
public:
	ActiveTrade(Player *starter, Player *receiver, int32_t id);

	auto getId() const -> int32_t { return m_id; }
	auto getSenderTrade() const -> TradeInfo * { return m_sender.get(); }
	auto getReceiverTrade() const -> TradeInfo * { return m_receiver.get(); }

	auto getSender() -> Player *;
	auto getReceiver() -> Player *;

	auto bothCanTrade() -> bool;
	auto bothAccepted() -> bool;
	auto returnTrade() -> void;
	auto swapTrade() -> void;
	auto accept(TradeInfo *unit) -> void;
	auto addMesos(Player *holder, TradeInfo *unit, int32_t amount) -> int32_t;
	auto addItem(Player *holder, TradeInfo *unit, Item *item, uint8_t tradeSlot, int16_t inventorySlot, int8_t inventory, int16_t amount) -> Item *;
	auto isItemInSlot(TradeInfo *unit, uint8_t tradeSlot) -> bool { return (tradeSlot > TradeInfo::TradeSize ? true : unit->slot[tradeSlot - 1]); }
private:
	auto canTrade(Player *target, TradeInfo *unit) -> bool;
	auto giveItems(Player *target, TradeInfo *unit) -> void;
	auto giveMesos(Player *player, TradeInfo *info, bool traded = false) -> void;

	int32_t m_id = 0;
	int32_t m_senderId = 0;
	int32_t m_receiverId = 0;
	owned_ptr_t<TradeInfo> m_sender;
	owned_ptr_t<TradeInfo> m_receiver;
};