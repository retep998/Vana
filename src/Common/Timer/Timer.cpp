/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "Container.h"
#include "../TimeUtilities.h"
#include "Thread.h"

namespace Timer {

Timer::Timer(function<void ()> func, const Id &id, Container *container, int64_t runAt, clock_t repeat) :
m_id(id),
m_container(container),
m_run_at(runAt),
m_repeat(repeat),
m_function(func)
{
	if (!m_container) // No container specified, use the central container
		m_container = Thread::Instance()->getContainer();

	if (!m_run_at)
		reset();
	else
		Thread::Instance()->forceReSort();

	m_container->registerTimer(this);
	Thread::Instance()->registerTimer(this);
}

Timer::~Timer() {
	Thread::Instance()->removeTimer(this);
}

void Timer::run() {
	m_function();

	if (m_repeat) {
		reset();
	}
	else {
		m_container->removeTimer(getId());
	}
}

void Timer::reset() {
	m_run_at = m_repeat + TimeUtilities::getTickCount();
	Thread::Instance()->forceReSort();
}

int64_t Timer::getTimeLeft() const {
	return m_run_at - TimeUtilities::getTickCount();
}

}
