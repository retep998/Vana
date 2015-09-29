/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Common/Types.hpp"
#include <chrono>
#include <ctime>

namespace Vana {
	class StopWatch {
		NONCOPYABLE(StopWatch);
	public:
		StopWatch() : m_start{effective_clock_t::now()} { }
		auto restart() -> void { m_start = effective_clock_t::now(); }
		template <typename TDuration>
		auto elapsed() const -> typename TDuration::rep { return duration_cast<TDuration>(effective_clock_t::now() - m_start).count(); }
	private:
		time_point_t m_start;
	};
}