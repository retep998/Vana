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
m_resort_timer(false),
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
	m_resort_timer = true;
	m_timers.push_back(timer);
	m_main_loop_condition.notify_one();
}

void NewTimer::removeTimer(OneTimer *timer) {
	boost::mutex::scoped_lock l(m_timers_mutex);
	m_timers.remove(timer);
	m_main_loop_condition.notify_one();
}

NewTimer::OneTimer * NewTimer::findMin() {
	// Unsynchronized
	if (m_timers.size() == 0) {
		return 0;
	}

	if (m_resort_timer) {
		struct {
			bool operator()(const OneTimer *t1, const OneTimer *t2) {
				return (t1->getRunAt() < t2->getRunAt());
			}
		} less_than;

		m_timers.sort(less_than);
		m_resort_timer = false;
	}

	return *m_timers.begin();
}

void NewTimer::forceReSort() {
	boost::mutex::scoped_lock l(m_timers_mutex);
	m_resort_timer = true;
	m_main_loop_condition.notify_one();
}

void NewTimer::runThread() {
	boost::mutex::scoped_lock l(m_timers_mutex);
	while (!m_terminate) {
		// Find minimum wakeup time
		OneTimer *minTimer = findMin();
		clock_t msec = (minTimer == 0) ? msec = 1000000000 : minTimer->getRunAt() - clock();
		if (msec <= 0) {
			minTimer->run();
			continue;
		}
		
		if (m_main_loop_condition.timed_wait(m_timers_mutex,
			boost::get_system_time() + boost::posix_time::milliseconds(msec))) {
				continue;
		}

		if (minTimer != 0) {
			l.unlock();
			minTimer->run();
			l.lock();
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
	NewTimer::Instance()->forceReSort();
}

NewTimer::OneTimer::Id::Id(unsigned int type, unsigned int id, unsigned int id2) :
type(type),
id(id),
id2(id2)
{
}

bool NewTimer::OneTimer::Id::operator==(Id const &other) const {
	return type == other.type && id == other.id && id2 == other.id2;
}

// For hashing NewTimer::OneTimer::Id
size_t hash_value(NewTimer::OneTimer::Id const &id) {
	size_t seed = 0;
	
	boost::hash_combine(seed, id.type);
	boost::hash_combine(seed, id.id);
	boost::hash_combine(seed, id.id2);
	
	return seed;
}

bool NewTimer::Container::checkTimer(const OneTimer::Id &id) {
	return (m_timers.find(id) != m_timers.end()) ? true : false;
}

void NewTimer::Container::registerTimer(OneTimer *timer) {
	m_timers[timer->getId()] = shared_ptr<NewTimer::OneTimer>(timer);
}

void NewTimer::Container::removeTimer(const OneTimer::Id &id) {
	m_timers.erase(id);
}
