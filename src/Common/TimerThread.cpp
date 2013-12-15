/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "TimerThread.h"
#include "Timer.h"
#include "TimerContainer.h"
#include "TimeUtilities.h"
#include <chrono>
#include <functional>

namespace Timer {

using std::bind;

Thread * Thread::singleton = nullptr;

Thread::Thread() :
	m_terminate(false),
	m_container(new Container)
{
	m_thread.reset(new std::thread([this]() { this->runThread(); }));
}

Thread::~Thread() {
	m_terminate = true;
}

void Thread::registerTimer(shared_ptr<Timer> timer) {
	std::unique_lock<std::recursive_mutex> l(m_timersMutex);
	m_timers.push(std::make_pair(timer->getRunAt(), timer));
	m_mainLoopCondition.notify_one();
}

time_point_t Thread::getWaitTime() const {
	if (m_timers.size() > 0) {
		return m_timers.top().first;
	}

	return TimeUtilities::getNowWithTimeAdded(milliseconds_t(1000000000));
}

void Thread::runThread() {
	std::unique_lock<std::recursive_mutex> l(m_timersMutex);
	while (!m_terminate) {
		time_point_t waitTime = getWaitTime();
		time_point_t now = TimeUtilities::getNow();

		while (waitTime <= now) {
			pair<time_point_t, weak_ptr<Timer>> top = m_timers.top();
			if (top.second.expired()) {
				m_timers.pop();
				continue;
			}

			shared_ptr<Timer> timer = top.second.lock();
			m_timers.pop();

			if (timer->run() == TimerRunResult::Reset) {
				m_timers.push(std::make_pair(timer->reset(now), timer));
			}
			else {
				timer->getContainer()->removeTimer(timer->getId());
			}

			waitTime = getWaitTime();
		}

		if (m_mainLoopCondition.wait_until(l, waitTime) == std::cv_status::no_timeout) {
			continue;
		}
	}
}

}