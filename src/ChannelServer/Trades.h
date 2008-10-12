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
#include "Types.h"
#include <unordered_map>

using std::tr1::unordered_map;

class Player;
class ReadPacket;

namespace Trades {
	extern unordered_map<int32_t, ActiveTrade *> trades;
	void tradeHandler(Player *player, ReadPacket &packet);
	void addTrade(ActiveTrade *trade);
	float getTaxLevel(int32_t mesos);
	void removeTrade(int32_t id);
	void cancelTrade(Player *player);
	void returnItems(Player *player, TradeInfo *info);
	void returnMesos(Player *player, TradeInfo *info);
	void timeout(Player *starter, Player *receiver, int32_t tradeid);
	void stopTimeout(Player *starter, Player *receiver);
	void startTimeout(Player *starter, Player *receiver, int32_t tradeid);
	bool canTrade(Player *player, TradeInfo *info);
	ActiveTrade * getTrade(int32_t id);
};

#endif