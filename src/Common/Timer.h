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

#include "TimerId.h"
#include "TimerTypes.h"
#include "Types.h"
#include <ctime>
#include <functional>

using std::function;

namespace TimerRunResult {
	enum RunResult {
		Reset,
		Complete
	};
}

namespace Timer {

class Container;
class Thread;

void create(const function<void()> func, const Id &id, Container *container, const duration_t &differenceFromNow);
void create(const function<void()> func, const Id &id, Container *container, const duration_t &differenceFromNow, const duration_t &repeat);

class Timer {
public:
	Timer(const function<void()> func, const Id &id, Container *container, const duration_t &differenceFromNow);
	Timer(const function<void()> func, const Id &id, Container *container, const duration_t &differenceFromNow, const duration_t &repeat);

	Id getId() const { return m_id; }
	const time_point_t & getRunAt() const { return m_runAt; }
	duration_t getTimeLeft() const;
	TimerRunResult::RunResult run();
	time_point_t reset(const time_point_t &now);
	Container * getContainer() const { return m_container; }
private:
	Timer() = delete;

	Id m_id;
	Container *m_container;
	time_point_t m_runAt;
	bool m_repeat;
	duration_t m_repeatTime;
	function<void ()> m_function;
};

}