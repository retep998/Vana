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

#include "Common/Types.hpp"
#include <array>
#include <memory>

namespace Vana {
	class Item;

	namespace ChannelServer {
		class Player;

		struct TradeInfo {
			TradeInfo()
			{
				for (trade_slot_t i = 0; i < TradeSize; i++) {
					items[i] = nullptr;
				}
			}

			const static trade_slot_t TradeSize = 9;

			bool accepted = false;
			trade_slot_t count = 0;
			mesos_t mesos = 0;
			array_t<Item *, TradeSize> items;
		};

		class ActiveTrade {
			NONCOPYABLE(ActiveTrade);
			NO_DEFAULT_CONSTRUCTOR(ActiveTrade);
		public:
			ActiveTrade(ref_ptr_t<Player> starter, ref_ptr_t<Player> receiver, trade_id_t id);

			auto getId() const -> trade_id_t { return m_id; }
			auto getSenderTrade() const -> TradeInfo * { return m_sender.get(); }
			auto getReceiverTrade() const -> TradeInfo * { return m_receiver.get(); }

			auto getSender() -> ref_ptr_t<Player>;
			auto getReceiver() -> ref_ptr_t<Player>;

			auto bothCanTrade() -> bool;
			auto bothAccepted() -> bool;
			auto returnTrade() -> void;
			auto swapTrade() -> void;
			auto accept(TradeInfo *unit) -> void;
			auto addMesos(ref_ptr_t<Player> holder, TradeInfo *unit, mesos_t amount) -> mesos_t;
			auto addItem(ref_ptr_t<Player> holder, TradeInfo *unit, Item *item, trade_slot_t tradeSlot, inventory_slot_t inventorySlot, inventory_t inventory, slot_qty_t amount) -> Item *;
			auto isItemInSlot(TradeInfo *unit, trade_slot_t tradeSlot) -> bool {
				return tradeSlot > TradeInfo::TradeSize ? true : unit->items[tradeSlot - 1] != nullptr;
			}
		private:
			auto canTrade(ref_ptr_t<Player> target, TradeInfo *unit) -> bool;
			auto giveItems(ref_ptr_t<Player> target, TradeInfo *unit) -> void;
			auto giveMesos(ref_ptr_t<Player> player, TradeInfo *info, bool traded = false) -> void;

			trade_id_t m_id = 0;
			player_id_t m_senderId = 0;
			player_id_t m_receiverId = 0;
			owned_ptr_t<TradeInfo> m_sender;
			owned_ptr_t<TradeInfo> m_receiver;
		};
	}
}