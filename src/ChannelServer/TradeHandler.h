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
#ifndef TRADEHANDLER_H
#define TRADEHANDLER_H

#include "Types.h"

class PacketReader;
class Player;
struct TradeInfo;

namespace TradeHandler {
	void tradeHandler(Player *player, PacketReader &packet);
	int32_t getTaxLevel(int32_t mesos);
	void cancelTrade(Player *player);
	void giveItems(Player *player, TradeInfo *info);
	void giveMesos(Player *player, TradeInfo *info, bool traded = false);
	void timeout(Player *starter, Player *receiver, int32_t tradeid);
	void removeTrade(int32_t id, Player *one, Player *two);
	void stopTimeout(Player *starter, Player *receiver);
	void startTimeout(Player *starter, Player *receiver, int32_t tradeid);
	bool canTrade(Player *player, TradeInfo *info);
}

#endif