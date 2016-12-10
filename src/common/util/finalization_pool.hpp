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

#include "common/timer/timer.hpp"
#include "common/timer/thread.hpp"
#include "common/timer/type.hpp"
#include "common/types.hpp"

namespace vana {
	namespace util {
		// Keep track of a pool of objects and query them from time to time until only one reference remains
		template <typename TObject>
		class finalization_pool {
		public:
			// Minimum use count refers to the baseline number of usages in cases of mutual referencing
			auto initialize(uint32_t unique_pool_seed, uint32_t minimum_use_count = 0) -> void {
				if (m_initialized) {
					THROW_CODE_EXCEPTION(invalid_operation_exception, "must only initialize once");
				}

				// Add 1 to the minimum use count in order to account for our copy
				m_minimum_use_count = minimum_use_count + 1;

				timer::timer::create(
					[this](const time_point &) {
						this->process();
					},
					timer::id{timer::type::finalize_timer, unique_pool_seed},
					timer::thread::get_instance().get_timer_container(),
					seconds{60},
					seconds{30});

				m_initialized = true;
			}

			auto store(ref_ptr<TObject> obj) -> void {
				if (!m_initialized) {
					THROW_CODE_EXCEPTION(invalid_operation_exception, "Must initialize before use");
				}

				m_waiting.push_back(obj);
			}
		private:
			auto process() -> void {
				if (m_waiting.size() == 0) {
					return;
				}

				// The loop starts at one since we could run into the following case:
				// Iteration 0, remove, i--
				// I would be assuming both underflow and overflow behavior and I don't think either is guaranteed
				for (size_t i = 1; i <= m_waiting.size(); i++) {
					ref_ptr<TObject> &v = m_waiting[i - 1];
					unsigned long use_count = (unsigned long)v.use_count();
					if (use_count <= m_minimum_use_count) {
						m_waiting.erase(std::begin(m_waiting) + (i - 1));
						i--;
					}
				}
			}

			bool m_initialized = false;
			uint32_t m_minimum_use_count = 0;
			vector<ref_ptr<TObject>> m_waiting;
		};
	}
}