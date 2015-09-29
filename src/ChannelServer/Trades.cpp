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
#include "Trades.hpp"
#include "Common/TimeUtilities.hpp"
#include "Common/Timer.hpp"
#include "Common/TimerContainer.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/Trade.hpp"
#include "ChannelServer/TradeHandler.hpp"
#include <functional>

namespace Vana {
namespace ChannelServer {

seconds_t Trades::TradeTimeout = seconds_t{180};

Trades::Trades() :
	m_tradeIds{1, 100000}
{
}

auto Trades::newTrade(Player *start, Player *recv) -> trade_id_t {
	trade_id_t id = m_tradeIds.lease();
	m_trades[id] = make_ref_ptr<ActiveTrade>(start, recv, id);
	startTimeout(id, start);
	return id;
}

auto Trades::removeTrade(trade_id_t id) -> void {
	if (getTimerSecondsRemaining(id).count() > 0) {
		stopTimeout(id);
	}
	Player *p = m_trades[id]->getSender();
	if (p != nullptr) {
		p->setTrading(false);
		p->setTradeId(0);
	}
	p = m_trades[id]->getReceiver();
	if (p != nullptr) {
		p->setTrading(false);
		p->setTradeId(0);
	}
	m_trades.erase(id);
	m_tradeIds.release(id);
}

auto Trades::getTrade(trade_id_t id) -> ActiveTrade * {
	return m_trades.find(id) != std::end(m_trades) ? m_trades[id].get() : nullptr;
}

auto Trades::getTimerSecondsRemaining(trade_id_t id) -> seconds_t {
	Vana::Timer::Id timerId{TimerType::TradeTimer, id};
	return getTimers()->getRemainingTime<seconds_t>(timerId);
}

auto Trades::timeout(Player *sender) -> void {
	TradeHandler::cancelTrade(sender);
}

auto Trades::stopTimeout(trade_id_t id) -> void {
	Vana::Timer::Id timerId{TimerType::TradeTimer, id};
	getTimers()->removeTimer(timerId);
}

auto Trades::startTimeout(trade_id_t id, Player *sender) -> void {
	Vana::Timer::Id timerId{TimerType::TradeTimer, id};
	Vana::Timer::Timer::create(
		[this, sender](const time_point_t &now) {
			this->timeout(sender);
		},
		timerId,
		nullptr,
		TradeTimeout);
}

}
}