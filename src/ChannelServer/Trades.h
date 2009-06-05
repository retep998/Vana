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
#ifndef TRADES_H
#define TRADES_H

#include "Trade.h"
#include "Types.h"
#include <boost/shared_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>

using std::tr1::unordered_map;

class ActiveTrade;
class Player;
class PacketReader;

class Trades : boost::noncopyable {
public:
	static Trades * Instance() {
		if (singleton == 0)
			singleton = new Trades;
		return singleton;
	}

	void addTrade(ActiveTrade *trade);
	void removeTrade(int32_t id);
	ActiveTrade * getTrade(int32_t id);
private:
	Trades() {};
	static Trades *singleton;

	unordered_map<int32_t, boost::shared_ptr<ActiveTrade> > trades;
};

#endif
