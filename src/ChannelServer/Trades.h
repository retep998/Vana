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

#ifndef TRADES_H
#define TRADES_H

#include "Trade.h"
#include "LoopingId.h"
#include <hash_map>
#include <boost/scoped_ptr.hpp>

using stdext::hash_map;
using boost::scoped_ptr;

class Player;
class ReadPacket;
struct TradeInfo;

namespace Trades {
	extern hash_map<int, ActiveTrade *> trades;
	void tradeHandler(Player *player, ReadPacket *packet);
	void addTrade(ActiveTrade *trade);
	float getTaxLevel(int mesos);
	void removeTrade(int id);
	void cancelTrade(Player *player);
	void returnItems(Player *player, TradeInfo *info);
	void returnMesos(Player *player, TradeInfo *info);
	void timeout(Player *starter, Player *receiver, int tradeid);
	void stopTimeout(Player *starter, Player *receiver);
	void startTimeout(Player *starter, Player *receiver, int tradeid);
	bool canTrade(Player *player, TradeInfo *info);
	ActiveTrade * getTrade(int id);
};

#endif