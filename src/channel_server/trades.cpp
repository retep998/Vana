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
#include "trades.hpp"
#include "common/timer/container.hpp"
#include "common/timer/timer.hpp"
#include "common/util/time.hpp"
#include "channel_server/player.hpp"
#include "channel_server/trade.hpp"
#include "channel_server/trade_handler.hpp"
#include <functional>

namespace vana {
namespace channel_server {

seconds trades::trade_timeout = seconds{180};

trades::trades() :
	m_trade_ids{1, 100000}
{
}

auto trades::new_trade(ref_ptr<player> start, ref_ptr<player> recv) -> game_trade_id {
	game_trade_id id = m_trade_ids.lease();
	m_trades[id] = make_ref_ptr<active_trade>(start, recv, id);
	start_timeout(id, start);
	return id;
}

auto trades::remove_trade(game_trade_id id) -> void {
	if (get_timer_seconds_remaining(id).count() > 0) {
		stop_timeout(id);
	}
	auto p = m_trades[id]->get_sender();
	if (p != nullptr) {
		p->set_trading(false);
		p->set_trade_id(0);
	}
	p = m_trades[id]->get_receiver();
	if (p != nullptr) {
		p->set_trading(false);
		p->set_trade_id(0);
	}
	m_trades.erase(id);
	m_trade_ids.release(id);
}

auto trades::get_trade(game_trade_id id) -> active_trade * {
	return m_trades.find(id) != std::end(m_trades) ? m_trades[id].get() : nullptr;
}

auto trades::get_timer_seconds_remaining(game_trade_id id) -> seconds {
	vana::timer::id timer_id{vana::timer::type::trade_timer, id};
	return get_timers()->get_remaining_time<seconds>(timer_id);
}

auto trades::timeout(ref_ptr<player> sender) -> void {
	trade_handler::cancel_trade(sender);
}

auto trades::stop_timeout(game_trade_id id) -> void {
	vana::timer::id timer_id{vana::timer::type::trade_timer, id};
	get_timers()->remove_timer(timer_id);
}

auto trades::start_timeout(game_trade_id id, ref_ptr<player> sender) -> void {
	vana::timer::id timer_id{vana::timer::type::trade_timer, id};
	vana::timer::timer::create(
		[this, sender](const time_point &now) {
			this->timeout(sender);
		},
		timer_id,
		nullptr,
		trade_timeout);
}

}
}