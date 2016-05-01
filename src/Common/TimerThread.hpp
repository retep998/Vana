/*
Copyright (C) 2008-2016 Vana Development Team

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
#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace Vana {
	namespace Timer {
		class Container;
		class Timer;

		class TimerThread {
			SINGLETON(TimerThread);
		public:
			~TimerThread();
			auto getTimerContainer() const -> ref_ptr_t<Container>;
			auto registerTimer(ref_ptr_t<Timer> timer, time_point_t runAt) -> void;
		private:
			auto getWaitTime() const -> time_point_t;

			using timer_pair_t = pair_t<time_point_t, view_ptr_t<Timer>>;

			struct FindClosestTimer {
				auto operator()(const timer_pair_t &t1, const timer_pair_t &t2) const -> bool {
					return t1.first > t2.first;
				}
			};

			std::priority_queue<timer_pair_t, vector_t<timer_pair_t>, FindClosestTimer> m_timers;
			std::condition_variable_any m_mainLoopCondition;
			recursive_mutex_t m_timersMutex;
			ref_ptr_t<thread_t> m_thread;
			ref_ptr_t<Container> m_container; // Central container for Timers that don't belong to other containers
		};
	}
}