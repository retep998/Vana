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

#include "common/timer_temp.hpp"
#include "common/timer_id.hpp"
#include "common/timer_type.hpp"
#include "common/types_temp.hpp"
#include <functional>
#include <memory>
#include <unordered_map>

namespace vana {
	namespace timer {
		class container {
		public:
			template <typename TDuration>
			auto get_remaining_time(const id &id) const -> TDuration;
			auto is_timer_running(const id &id) const -> bool;
			auto register_timer(ref_ptr<timer> timer, const id &id, time_point run_at) -> void;
			auto remove_timer(const id &id) -> void;
		private:
			hash_map<id, ref_ptr<timer>> m_timers;
		};

		template <typename TDuration>
		auto container::get_remaining_time(const id &id) const -> TDuration {
			auto iter = m_timers.find(id);
			if (iter != std::end(m_timers)) {
				return duration_cast<TDuration>(iter->second->get_time_left());
			}
			return TDuration{0};
		}
	}
}