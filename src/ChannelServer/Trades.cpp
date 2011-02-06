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
#include "Trades.h"
#include "Player.h"
#include "Trade.h"
#include "TradeHandler.h"
#include "Timer/Container.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

Trades * Trades::singleton = nullptr;

Trades::Trades() :
ids(0),
container(new Timer::Container)
{
}

int32_t Trades::newTrade(Player *start, Player *recv) {
	int32_t id = getNewId();
	trades[id].reset(new ActiveTrade(start, recv, id));
	startTimeout(id, start);
	return id;
}

void Trades::removeTrade(int32_t id) {
	if (checkTimer(id) != 0) {
		stopTimeout(id);
	}
	Player *p = trades[id]->getSender();
	if (p != nullptr) {
		p->setTrading(false);
		p->setTradeId(0);
	}
	p = trades[id]->getReceiver();
	if (p != nullptr) {
		p->setTrading(false);
		p->setTradeId(0);
	}
	trades.erase(id);
}

ActiveTrade * Trades::getTrade(int32_t id) {
	return (trades.find(id) != trades.end() ? trades[id].get() : nullptr);
}

int32_t Trades::checkTimer(int32_t id) {
	Timer::Id check(Timer::Types::TradeTimer, id, 0);
	return getTimers()->checkTimer(check);
}

void Trades::timeout(Player *sender) {
	TradeHandler::cancelTrade(sender);
}

void Trades::stopTimeout(int32_t id) {
	Timer::Id rid(Timer::Types::TradeTimer, id, 0);
	getTimers()->removeTimer(rid);
}

void Trades::startTimeout(int32_t id, Player *sender) {
	Timer::Id tid(Timer::Types::TradeTimer, id, 0);
	new Timer::Timer(bind(&Trades::timeout, this, sender),
		tid, nullptr, Timer::Time::fromNow(TradeTimeout * 1000));
}