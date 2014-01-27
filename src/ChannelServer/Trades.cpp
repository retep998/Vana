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
#include "Trades.hpp"
#include "Player.hpp"
#include "Trade.hpp"
#include "TradeHandler.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include "TimerContainer.hpp"
#include <functional>

seconds_t Trades::TradeTimeout = seconds_t(180);

Trades::Trades() :
	m_tradeIds(1, 100000)
{
}

auto Trades::newTrade(Player *start, Player *recv) -> int32_t {
	int32_t id = getNewId();
	m_trades[id] = make_ref_ptr<ActiveTrade>(start, recv, id);
	startTimeout(id, start);
	return id;
}

auto Trades::removeTrade(int32_t id) -> void {
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
}

auto Trades::getTrade(int32_t id) -> ActiveTrade * {
	return m_trades.find(id) != std::end(m_trades) ? m_trades[id].get() : nullptr;
}

auto Trades::getTimerSecondsRemaining(int32_t id) -> seconds_t {
	Timer::Id check(Timer::Types::TradeTimer, id, 0);
	return getTimers()->getSecondsRemaining(check);
}

auto Trades::timeout(Player *sender) -> void {
	TradeHandler::cancelTrade(sender);
}

auto Trades::stopTimeout(int32_t id) -> void {
	Timer::Id rid(Timer::Types::TradeTimer, id, 0);
	getTimers()->removeTimer(rid);
}

auto Trades::startTimeout(int32_t id, Player *sender) -> void {
	Timer::Id tid(Timer::Types::TradeTimer, id, 0);
	Timer::create([this, sender](const time_point_t &now) { this->timeout(sender); },
		tid, nullptr, TradeTimeout);
}