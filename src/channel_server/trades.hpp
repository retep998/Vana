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

#include "common/timer/container_holder.hpp"
#include "common/types.hpp"
#include "common/util/id_pool.hpp"
#include <memory>
#include <unordered_map>

namespace vana {
	namespace channel_server {
		class active_trade;
		class player;

		class trades : public vana::timer::container_holder {
		public:
			trades();
			auto new_trade(ref_ptr<player> start, ref_ptr<player> recv) -> game_trade_id;
			auto remove_trade(game_trade_id id) -> void;
			auto stop_timeout(game_trade_id id) -> void;
			auto get_trade(game_trade_id id) -> active_trade *;
		private:
			static seconds trade_timeout;

			auto get_timer_seconds_remaining(game_trade_id id) -> seconds;
			auto timeout(ref_ptr<player> sender) -> void;
			auto start_timeout(game_trade_id id, ref_ptr<player> sender) -> void;

			vana::util::id_pool<game_trade_id> m_trade_ids;
			hash_map<game_trade_id, ref_ptr<active_trade>> m_trades;
		};
	}
}