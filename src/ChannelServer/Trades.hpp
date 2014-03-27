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

#include "LoopingId.hpp"
#include "TimerContainerHolder.hpp"
#include "Types.hpp"
#include <memory>
#include <unordered_map>

class ActiveTrade;
class Player;

class Trades : public TimerContainerHolder {
public:
	Trades();
	auto newTrade(Player *start, Player *recv) -> trade_id_t;
	auto removeTrade(trade_id_t id) -> void;
	auto stopTimeout(trade_id_t id) -> void;
	auto getTrade(trade_id_t id) -> ActiveTrade *;
private:
	static seconds_t TradeTimeout;

	auto getNewId() -> trade_id_t { return m_tradeIds.next(); }
	auto getTimerSecondsRemaining(trade_id_t id) -> seconds_t;
	auto timeout(Player *sender) -> void;
	auto startTimeout(trade_id_t id, Player *sender) -> void;

	LoopingId<trade_id_t> m_tradeIds;
	hash_map_t<trade_id_t, ref_ptr_t<ActiveTrade>> m_trades;
};