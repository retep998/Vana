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
#pragma once

#include "Types.h"
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>

using std::tr1::unordered_map;

class ActiveTrade;
class Player;
namespace Timer {
	class Container;
};

class Trades : boost::noncopyable {
public:
	static Trades * Instance() {
		if (singleton == nullptr)
			singleton = new Trades;
		return singleton;
	}

	int32_t newTrade(Player *start, Player *recv);
	void removeTrade(int32_t id);
	void stopTimeout(int32_t id);
	ActiveTrade * getTrade(int32_t id);
private:
	Trades();
	static Trades *singleton;
	const static int32_t TradeTimeout = 180; // Trade timeout in seconds

	boost::scoped_ptr<Timer::Container> container;
	unordered_map<int32_t, boost::shared_ptr<ActiveTrade> > trades;
	int32_t ids;

	Timer::Container * getTimers() const { return container.get(); }
	int32_t getNewId() { return ++ids; }
	int32_t checkTimer(int32_t id);
	void timeout(Player *sender);
	void startTimeout(int32_t id, Player *sender);
};
