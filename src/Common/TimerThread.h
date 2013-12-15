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
#pragma once

#include "noncopyable.hpp"
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace Timer {

using std::pair;
using std::shared_ptr;
using std::weak_ptr;

class Container;
class Timer;

class Thread : boost::noncopyable {
public:
	static Thread * Instance() {
		if (singleton == nullptr)
			singleton = new Thread;
		return singleton;
	}

	~Thread();

	Container * getContainer() const { return m_container.get(); }

	void registerTimer(shared_ptr<Timer> timer);
private:
	Thread();
	static Thread *singleton;
	void runThread();
	time_point_t getWaitTime() const;

	struct FindClosestTimer {
		bool operator()(const pair<time_point_t, weak_ptr<Timer>> &t1, const pair<time_point_t, weak_ptr<Timer>> &t2) const {
			return t1.first > t2.first;
		}
	};

	volatile bool m_terminate;

	std::priority_queue<pair<time_point_t, weak_ptr<Timer>>, std::vector<pair<time_point_t, weak_ptr<Timer>>>, FindClosestTimer> m_timers;
	std::recursive_mutex m_timersMutex;
	std::unique_ptr<std::thread> m_thread;
	std::condition_variable_any m_mainLoopCondition;

	std::unique_ptr<Container> m_container; // Central container for Timers that don't belong to other containers
};

}