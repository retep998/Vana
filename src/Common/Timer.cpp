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
#include "Timer.hpp"
#include "TimerContainer.hpp"
#include "TimerThread.hpp"
#include "TimeUtilities.hpp"
#include <memory>

namespace Timer {

auto Timer::create(const timer_func_t func, const Id &id, ref_ptr_t<Container> container, const duration_t &differenceFromNow, const duration_t &repeat) -> void {
	if (container == nullptr) {
		container = TimerThread::getInstance().getTimerContainer();
	}

	ref_ptr_t<Timer> timer = make_ref_ptr<Timer>(func, id, container, differenceFromNow, repeat);
	container->registerTimer(timer, id, timer->m_runAt);
}

Timer::Timer(const timer_func_t func, const Id &id, ref_ptr_t<Container> container, const duration_t &differenceFromNow, const duration_t &repeat) :
	m_id{id},
	m_container{container},
	m_repeatTime{repeat},
	m_function{func}
{
	m_repeat = repeat.count() != 0;
	m_runAt = TimeUtilities::getNowWithTimeAdded(differenceFromNow);
}

auto Timer::removeFromContainer() const -> void {
	if (ref_ptr_t<Container> container = m_container.lock()) {
		container->removeTimer(m_id);
	}
}

auto Timer::run(const time_point_t &now) const -> RunResult {
	m_function(now);
	return m_repeat ? RunResult::Reset : RunResult::Complete;
}

auto Timer::reset(const time_point_t &now) -> time_point_t {
	m_runAt = now + m_repeatTime;
	return m_runAt;
}

auto Timer::getTimeLeft() const -> duration_t {
	return m_runAt - TimeUtilities::getNow();
}

}