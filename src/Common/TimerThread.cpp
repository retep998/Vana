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

namespace Vana {
namespace Timer {

TimerThread::TimerThread()
{
	m_container = make_ref_ptr<Container>();
	m_thread = ThreadPool::lease(
		[this](owned_lock_t<recursive_mutex_t> &lock) {
			time_point_t waitTime = getWaitTime();
			time_point_t now = TimeUtilities::getNow();

			while (waitTime <= now && m_timers.size() > 0) {
				timer_pair_t top = m_timers.top();

				if (ref_ptr_t<Timer> timer = top.second.lock()) {
					m_timers.pop();

					if (timer->run(now) == RunResult::Reset) {
						m_timers.emplace(timer->reset(now), timer);
					}
					else {
						timer->removeFromContainer();
					}

					waitTime = getWaitTime();
				}
				else {
					// Expired timer
					m_timers.pop();
					continue;
				}
			}

			m_mainLoopCondition.wait_until(lock, waitTime);
		},
		[this] {
			m_mainLoopCondition.notify_one();
		},
		m_timersMutex);
}

TimerThread::~TimerThread() {
	m_thread.reset();
}

auto TimerThread::getTimerContainer() const -> ref_ptr_t<Container> {
	return m_container;
}

auto TimerThread::registerTimer(ref_ptr_t<Timer> timer, time_point_t runAt) -> void {
	owned_lock_t<recursive_mutex_t> l{m_timersMutex};
	m_timers.emplace(runAt, timer);
	m_mainLoopCondition.notify_one();
}

auto TimerThread::getWaitTime() const -> time_point_t {
	if (m_timers.size() > 0) {
		return m_timers.top().first;
	}

	return TimeUtilities::getNowWithTimeAdded(milliseconds_t{1000000000});
}

}
}