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
#ifndef TRADE_H
#define TRADE_H

#include "Types.h"
#include <boost/scoped_ptr.hpp>

using boost::scoped_ptr;

class Player;
class PacketReader;
struct Item;

struct TradeInfo {
	TradeInfo(Player *player) : player(player), mesos(0), count(0), accepted(false) {
		for (size_t i = 0; i < 9; i++) {
			slot[i] = false;
		}
	}
	Player *player;
	Item *items[9];
	int8_t count;
	int32_t mesos;
	bool slot[9];
	bool accepted;
};

class ActiveTrade {
public:
	ActiveTrade(Player *starter, Player *receiver);

	void setID(int32_t id) { this->id = id; }

	int32_t getID() const { return id; }
	TradeInfo * getStarter() const { return starter.get(); }
	TradeInfo * getReceiver() const { return receiver.get(); }
private:
	scoped_ptr<TradeInfo> starter;
	scoped_ptr<TradeInfo> receiver;
	int32_t id;
};

#endif