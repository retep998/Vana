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

#include "common/Types.hpp"
#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace vana {
	namespace timer {
		class container;
		class timer;

		class timer_thread {
			SINGLETON(timer_thread);
		public:
			~timer_thread();
			auto get_timer_container() const -> ref_ptr<container>;
			auto register_timer(ref_ptr<timer> timer, time_point run_at) -> void;
		private:
			auto get_wait_time() const -> time_point;

			using timer_pair = pair<time_point, view_ptr<timer>>;

			struct find_closest_timer {
				auto operator()(const timer_pair &t1, const timer_pair &t2) const -> bool {
					return t1.first > t2.first;
				}
			};

			std::priority_queue<timer_pair, vector<timer_pair>, find_closest_timer> m_timers;
			std::condition_variable_any m_main_loop_condition;
			recursive_mutex m_timers_mutex;
			ref_ptr<thread> m_thread;
			ref_ptr<container> m_container; // Central container for timers that don't belong to other containers
		};
	}
}