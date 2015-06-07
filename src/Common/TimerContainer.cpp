/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "TimerThread.hpp"
#include "Timer.hpp"
#include <chrono>

namespace Timer {

auto Container::isTimerRunning(const Id &id) const -> bool {
	return m_timers.find(id) != std::end(m_timers);
}

auto Container::registerTimer(ref_ptr_t<Timer> timer, const Id &id, time_point_t runAt) -> void {
	m_timers[id] = timer;
	TimerThread::getInstance().registerTimer(timer, runAt);
}

auto Container::removeTimer(const Id &id) -> void {
	auto iter = m_timers.find(id);
	if (iter != std::end(m_timers)) {
		m_timers.erase(iter);
	}
}

}