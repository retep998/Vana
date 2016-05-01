/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "Common/Types.hpp"

namespace Vana {
	class PacketReader;

	namespace ChannelServer {
		class Player;
		struct TradeInfo;

		namespace TradeHandler {
			auto tradeHandler(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto removeTrade(trade_id_t id) -> void;
			auto cancelTrade(ref_ptr_t<Player> player) -> void;

			auto getTaxLevel(mesos_t mesos) -> int32_t;
		}
	}
}