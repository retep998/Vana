/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "TimerId.hpp"
#include "TimerTypes.hpp"
#include "Types.hpp"
#include <ctime>
#include <functional>
#include <memory>

namespace Timer {

using timer_func_t = function_t<void(const time_point_t &)>;

enum class RunResult {
	Reset,
	Complete
};

class Container;
class Thread;

auto create(const timer_func_t func, const Id &id, ref_ptr_t<Container> container, const duration_t &differenceFromNow, const duration_t &repeat = seconds_t(0)) -> void;

class Timer {
	NONCOPYABLE(Timer);
	NO_DEFAULT_CONSTRUCTOR(Timer);
public:
	Timer(const timer_func_t func, const Id &id, ref_ptr_t<Container> container, const duration_t &differenceFromNow, const duration_t &repeat);

	auto getId() const -> Id { return m_id; }
	auto getRunAt() const -> const time_point_t & { return m_runAt; }
	auto getTimeLeft() const -> duration_t;
	auto run(const time_point_t &now) const -> RunResult;
	auto reset(const time_point_t &now) -> time_point_t;
	auto getContainer() const -> view_ptr_t<Container> { return m_container; }
private:
	Id m_id;
	view_ptr_t<Container> m_container;
	time_point_t m_runAt;
	bool m_repeat;
	duration_t m_repeatTime;
	timer_func_t m_function;
};

}