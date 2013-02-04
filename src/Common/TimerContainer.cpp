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
#include "TimerContainer.h"
#include "Timer.h"
#include <chrono>

namespace Timer {

seconds_t Container::getSecondsRemaining(const Id &id) {
	auto iter = m_timers.find(id);
	if (iter != m_timers.end()) {
		return std::chrono::duration_cast<seconds_t>(iter->second->getTimeLeft());
	}
	return seconds_t(0);
}

milliseconds_t Container::getMillisecondsRemaining(const Id &id) {
	auto iter = m_timers.find(id);
	if (iter != m_timers.end()) {
		return std::chrono::duration_cast<milliseconds_t>(iter->second->getTimeLeft());
	}
	return milliseconds_t(0);
}

bool Container::isTimerRunning(const Id &id) {
	return m_timers.find(id) != m_timers.end();
}

void Container::registerTimer(Timer *timer) {
	m_timers[timer->getId()] = shared_ptr<Timer>(timer);
}

void Container::removeTimer(const Id &id) {
	m_timers.erase(id);
}

}