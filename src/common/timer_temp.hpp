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

#include "common/timer_id.hpp"
#include "common/timer_type.hpp"
#include "common/types_temp.hpp"
#include <ctime>
#include <functional>
#include <memory>

namespace vana {
	namespace timer {
		using timer_func = function<void(const time_point &)>;

		enum class run_result {
			reset,
			complete,
		};

		class container;
		class thread;

		class timer {
			NONCOPYABLE(timer);
			NO_DEFAULT_CONSTRUCTOR(timer);
		public:
			timer(const timer_func func, const id &id, ref_ptr<container> container, const duration &difference_from_now, const duration &repeat);
			static auto create(const timer_func func, const id &id, ref_ptr<container> container, const duration &difference_from_now, const duration &repeat = seconds{0}) -> void;

			auto get_time_left() const -> duration;
			auto run(const time_point &now) const -> run_result;
			auto reset(const time_point &now) -> time_point;
			auto remove_from_container() const -> void;
		private:
			id m_id;
			view_ptr<container> m_container;
			time_point m_run_at;
			bool m_repeat;
			duration m_repeat_time;
			timer_func m_function;
		};
	}
}