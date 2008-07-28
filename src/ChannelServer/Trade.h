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

#include <hash_map>

using namespace std;
using namespace stdext;

class Player;
class ReadPacket;

struct Item;

struct TradeInfo {
	TradeInfo() : mesos(0), count(0), accepted(false) { 
		for (char i = 0; i < 9; i++)
			slot[i] = false;
	}
	Player *player;
	Item *items[9];
	int mesos;
	bool slot[9];
	char count;
	bool accepted;
};

class ActiveTrade {
	public:
		ActiveTrade();
		ActiveTrade(Player *starter, Player *receiver);
		void setID(int id) {
			this->id = id;
		}
		int getID() {
			return id;
		}
		TradeInfo * getStarter() {
			return starter;
		}
		TradeInfo * getReceiver() {
			return receiver;
		}
	private:
		TradeInfo *starter;
		TradeInfo *receiver;
		int id;
};

#endif