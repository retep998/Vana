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
#include "Container.h"
#include "Timer.h"

namespace Timer {

int32_t Container::checkTimer(const Id &id) {
	if (m_timers.find(id) != m_timers.end()) {
		return (int32_t)(m_timers[id]->getTimeLeft() / 1000);
	}
	return 0;
}

int64_t Container::checkTimer(const Id &id, bool msec) {
	if (m_timers.find(id) != m_timers.end()) {
		return m_timers[id]->getTimeLeft();
	}
	return 0;
}

void Container::registerTimer(Timer *timer) {
	m_timers[timer->getId()] = shared_ptr<Timer>(timer);
}

void Container::removeTimer(const Id &id) {
	if (m_timers.find(id) != m_timers.end()) {
		m_timers.erase(id);
	}
}

}