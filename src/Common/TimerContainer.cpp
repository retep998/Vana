/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "TimerContainer.h"
#include "TimerThread.h"
#include "Timer.h"
#include <chrono>

namespace Timer {

auto Container::getSecondsRemaining(const Id &id) -> seconds_t {
	auto iter = m_timers.find(id);
	if (iter != std::end(m_timers)) {
		return duration_cast<seconds_t>(iter->second->getTimeLeft());
	}
	return seconds_t(0);
}

auto Container::getMillisecondsRemaining(const Id &id) -> milliseconds_t {
	auto iter = m_timers.find(id);
	if (iter != std::end(m_timers)) {
		return duration_cast<milliseconds_t>(iter->second->getTimeLeft());
	}
	return milliseconds_t(0);
}

auto Container::isTimerRunning(const Id &id) -> bool {
	return m_timers.find(id) != std::end(m_timers);
}

auto Container::registerTimer(ref_ptr_t<Timer> timer) -> void {
	m_timers[timer->getId()] = timer;
	TimerThread::getInstance().registerTimer(timer);
}

auto Container::removeTimer(const Id &id) -> void {
	auto iter = m_timers.find(id);
	if (iter != std::end(m_timers)) {
		m_timers.erase(iter);
	}
}

}