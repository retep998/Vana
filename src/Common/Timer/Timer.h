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

#include "Id.h"
#include "../Types.h"
#include <ctime>
#include <boost/tr1/functional.hpp>

using std::tr1::function;

namespace Timer {

class Container;

class Timer {
public:
	Timer(function<void ()> func, const Id &id, Container *container, int64_t runAt, clock_t repeat = 0);
	~Timer();

	Id getId() const { return m_id; }
	int64_t getRunAt() const { return m_run_at; }
	int64_t getTimeLeft() const;

	void run();
	void reset(); // Only available for repeated timers
private:
	Id m_id;
	Container *m_container;
	int64_t m_run_at; // The time that this timer will run
	clock_t m_repeat; // Repeat this timer x msec after the timer ran
	function<void ()> m_function;
};

}
