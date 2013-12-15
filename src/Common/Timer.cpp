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
#include "Timer.h"
#include "TimerContainer.h"
#include "TimerThread.h"
#include "TimeUtilities.h"
#include <memory>

namespace Timer {

void create(const function<void()> func, const Id &id, Container *container, const duration_t &differenceFromNow) {
	if (container == nullptr) {
		container = Thread::Instance()->getContainer();
	}

	std::shared_ptr<Timer> timer = std::make_shared<Timer>(func, id, container, differenceFromNow);
	container->registerTimer(timer);
}

void create(const function<void()> func, const Id &id, Container *container, const duration_t &differenceFromNow, const duration_t &repeat) {
	if (container == nullptr) {
		container = Thread::Instance()->getContainer();
	}

	std::shared_ptr<Timer> timer = std::make_shared<Timer>(func, id, container, differenceFromNow, repeat);
	container->registerTimer(timer);
}

Timer::Timer(const function<void ()> func, const Id &id, Container *container, const duration_t &differenceFromNow) :
	m_id(id),
	m_container(container),
	m_repeat(false),
	m_function(func),
	m_repeatTime(0)
{
	m_runAt = TimeUtilities::getNowWithTimeAdded(differenceFromNow);
}

Timer::Timer(const function<void ()> func, const Id &id, Container *container, const duration_t &differenceFromNow, const duration_t &repeat) :
	m_id(id),
	m_container(container),
	m_repeat(true),
	m_repeatTime(repeat),
	m_function(func)
{
	m_runAt = TimeUtilities::getNowWithTimeAdded(differenceFromNow);
}

TimerRunResult::RunResult Timer::run() {
	m_function();
	return m_repeat ? TimerRunResult::Reset : TimerRunResult::Complete;
}

time_point_t Timer::reset(const time_point_t &now) {
	m_runAt = now + m_repeatTime;
	return m_runAt;
}

duration_t Timer::getTimeLeft() const {
	return m_runAt - TimeUtilities::getNow();
}

}