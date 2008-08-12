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
#ifndef NEWTIMER_H
#define NEWTIMER_H

#include <list>
#include <hash_map>

#define BOOST_ALL_DYN_LINK
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

using std::list;
using stdext::hash_map;

// Will be renamed to Timer after migration to this Timer finished
class NewTimer {
public:
	static NewTimer * Instance() {
		if (singleton == 0)
			singleton = new NewTimer;
		return singleton;
	}

	class OneTimer;
	class Container;
	struct Types {
		enum {
			CoolTimer,
			ItemTimer,
			MapTimer,
			PetTimer,
			PingTimer,
			ReactionTimer,
			SkillTimer,
			TradeTimer
		};
	};

	~NewTimer();

	Container * getContainer() const { return m_container.get(); }

	void registerTimer(OneTimer *timer);
	void removeTimer(OneTimer *timer);
private:
	NewTimer();
	NewTimer(const NewTimer&);
	NewTimer& operator=(const NewTimer&);
	static NewTimer *singleton;

	OneTimer * findMin();
	void runThread();

	volatile bool m_terminate;
	list<OneTimer *> m_timers;
	boost::mutex m_timers_mutex;

	boost::scoped_ptr<boost::thread> m_thread;
	boost::mutex m_main_loop_mutex;
	boost::condition m_main_loop_condition;

	boost::scoped_ptr<Container> m_container; // Central container for Timers that don't belong to other containers
};

class NewTimer::OneTimer {
public:
	struct Id {
		Id(unsigned int type, unsigned int id, unsigned int id2);
		unsigned int type;
		unsigned int id;
		unsigned int id2;
	};

	OneTimer(boost::function<void ()> func, Id id, Container *container,
		clock_t length, bool persistent);
	~OneTimer();

	Id getId() const { return m_id; }
	clock_t getRunAt() const { return m_run_at; }
	bool getPersistent() const { return m_persistent; }
	void setPersistent(bool val) { m_persistent = val; }

	void run();
	void reset();
private:
	Id m_id;
	Container *m_container;
	clock_t m_run_at; // The time that this timer will run
	clock_t m_length; // Difference between the time the timer is set and the time the timer is ran
	bool m_persistent;
	boost::function<void ()> m_function;
};

class NewTimer::Container {
public:
	bool checkTimer(const OneTimer::Id &id);
	void registerTimer(OneTimer *timer);
	void removeTimer(const OneTimer::Id &id);
private:
	hash_map<__int64, boost::shared_ptr<OneTimer>> m_timers;
};

#endif
