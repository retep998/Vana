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
#pragma once

#include "../Types.h" // For nullptr, remove with C++0x
#include <list>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

namespace Timer {

using std::list;

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

	void registerTimer(Timer *timer);
	void removeTimer(Timer *timer);
	void forceReSort();
private:
	Thread();
	static Thread *singleton;

	Timer * findMin();
	void runThread();

	bool m_resort_timer; // True if a new timer gets inserted into m_timers or it gets modified so it's not arranged
	volatile bool m_terminate;
	list<Timer *> m_timers;
	boost::recursive_mutex m_timers_mutex;

	boost::scoped_ptr<boost::thread> m_thread;
	boost::condition_variable_any m_main_loop_condition;

	boost::scoped_ptr<Container> m_container; // Central container for Timers that don't belong to other containers
};

}
