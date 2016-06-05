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
#include "TimerContainer.hpp"
#include "common_temp/TimerThread.hpp"
#include "common_temp/Timer.hpp"
#include <chrono>

namespace vana {
namespace timer {

auto container::is_timer_running(const id &id) const -> bool {
	return m_timers.find(id) != std::end(m_timers);
}

auto container::register_timer(ref_ptr<timer> timer, const id &id, time_point run_at) -> void {
	m_timers[id] = timer;
	timer_thread::get_instance().register_timer(timer, run_at);
}

auto container::remove_timer(const id &id) -> void {
	auto iter = m_timers.find(id);
	if (iter != std::end(m_timers)) {
		m_timers.erase(iter);
	}
}

}
}