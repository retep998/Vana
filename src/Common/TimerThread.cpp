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
#include "TimerThread.hpp"
#include "Common/ThreadPool.hpp"
#include "Common/Timer.hpp"
#include "Common/TimerContainer.hpp"
#include "Common/TimeUtilities.hpp"
#include <chrono>
#include <functional>

namespace vana {
namespace timer {

timer_thread::timer_thread()
{
	m_container = make_ref_ptr<container>();
	m_thread = thread_pool::lease(
		[this](owned_lock<recursive_mutex> &lock) {
			time_point wait_time = get_wait_time();
			time_point now = utilities::time::get_now();

			while (wait_time <= now && m_timers.size() > 0) {
				timer_pair top = m_timers.top();

				if (ref_ptr<timer> timer = top.second.lock()) {
					m_timers.pop();

					if (timer->run(now) == run_result::reset) {
						m_timers.emplace(timer->reset(now), timer);
					}
					else {
						timer->remove_from_container();
					}

					wait_time = get_wait_time();
				}
				else {
					// Expired timer
					m_timers.pop();
					continue;
				}
			}

			m_main_loop_condition.wait_until(lock, wait_time);
		},
		[this] {
			m_main_loop_condition.notify_one();
		},
		m_timers_mutex);
}

timer_thread::~timer_thread() {
	m_thread.reset();
}

auto timer_thread::get_timer_container() const -> ref_ptr<container> {
	return m_container;
}

auto timer_thread::register_timer(ref_ptr<timer> timer, time_point run_at) -> void {
	owned_lock<recursive_mutex> l{m_timers_mutex};
	m_timers.emplace(run_at, timer);
	m_main_loop_condition.notify_one();
}

auto timer_thread::get_wait_time() const -> time_point {
	if (m_timers.size() > 0) {
		return m_timers.top().first;
	}

	return utilities::time::get_now_with_time_added(milliseconds{1000000000});
}

}
}