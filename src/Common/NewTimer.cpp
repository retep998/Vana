/*
Copyright (C) 2008 Vana Development Team

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
#include "NewTimer.h"
#include <boost/bind.hpp>

NewTimer * NewTimer::singleton = 0;

NewTimer::NewTimer() :
m_terminate(false),
m_container(new Container)
{
	m_thread.reset(new boost::thread(boost::bind(&NewTimer::runThread, this)));
}

NewTimer::~NewTimer() {
	m_terminate = true;
}

void NewTimer::registerTimer(OneTimer *timer) {
	boost::mutex::scoped_lock l(m_timers_mutex);
	m_timers.push_back(timer);
	m_main_loop_condition.notify_one();
}

void NewTimer::removeTimer(OneTimer *timer) {
	boost::mutex::scoped_lock l(m_timers_mutex);
	m_timers.remove(timer);
	m_main_loop_condition.notify_one();
}

NewTimer::OneTimer * NewTimer::findMin() {
	boost::mutex::scoped_lock l(m_timers_mutex);
	if (m_timers.size() == 0) {
		return 0;
	}

	OneTimer *min;
	for (list<OneTimer *>::iterator iter = m_timers.begin(); iter != m_timers.end(); iter++) {
		OneTimer *cur = *iter;
		if (iter == m_timers.begin() || cur->getRunAt() < min->getRunAt()) {
			min = cur;
		}
	}
	return min;
}

void NewTimer::runThread() {
	while (!m_terminate) {
		// Find minimum wakeup time
		OneTimer *minTimer = findMin();
		clock_t msec = (minTimer == 0) ? msec = 1000000000 : minTimer->getRunAt() - clock();
		if (msec <= 0) {
			minTimer->run();
			continue;
		}
		
		boost::mutex::scoped_lock l(m_main_loop_mutex);
		if (m_main_loop_condition.timed_wait(m_main_loop_mutex,
			boost::get_system_time() + boost::posix_time::milliseconds(msec))) {
				continue;
		}

		if (minTimer != 0) {
			minTimer->run();
		}
	}
}

NewTimer::OneTimer::OneTimer(boost::function<void ()> func, Id id,
	Container *container, clock_t length, bool persistent) :
m_id(id),
m_container(container),
m_length(length),
m_persistent(persistent),
m_function(func)
{
	if (!m_container) { // No container specified, use the central container
		m_container = NewTimer::Instance()->getContainer();
	}

	reset();
	m_container->registerTimer(this);
	NewTimer::Instance()->registerTimer(this);
}

NewTimer::OneTimer::~OneTimer() {
	NewTimer::Instance()->removeTimer(this);
}

void NewTimer::OneTimer::run() {
	m_function();

	if (m_persistent) {
		reset();
	}
	else {
		m_container->removeTimer(getId());
	}
}

void NewTimer::OneTimer::reset() {
	m_run_at = m_length + clock();
}

NewTimer::OneTimer::Id::Id(unsigned int type, unsigned int id, unsigned int id2) :
type(type),
id(id),
id2(id2)
{
}


bool NewTimer::Container::checkTimer(const OneTimer::Id &id) {
	__int64 idReal = *(__int64 *)(&id);
	return (m_timers.find(idReal) != m_timers.end()) ? true : false;
}

void NewTimer::Container::registerTimer(OneTimer *timer) {
	__int64 idReal = *(__int64 *)(&(timer->getId()));
	m_timers[idReal] = boost::shared_ptr<NewTimer::OneTimer>(timer);
}

void NewTimer::Container::removeTimer(const OneTimer::Id &id) {
	__int64 idReal = *(__int64 *)(&id);
	m_timers.erase(idReal);
}
