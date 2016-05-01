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

#include "Common/Timer.hpp"
#include "Common/TimerThread.hpp"
#include "Common/TimerType.hpp"
#include "Common/Types.hpp"

namespace Vana {
	// Keep track of a pool of objects and query them from time to time until only one reference remains
	template <typename TObject>
	class FinalizationPool {
	public:
		// Minimum use count refers to the baseline number of usages in cases of mutual referencing
		auto initialize(uint32_t uniquePoolSeed, uint32_t minimumUseCount = 0) -> void {
			if (m_initialized) {
				throw InvalidOperationException{"Must only initialize once"};
			}

			// Add 1 to the minimum use count in order to account for our copy
			m_minimumUseCount = minimumUseCount + 1;

			Timer::Timer::create(
				[this](const time_point_t &) {
					this->process();
				},
				Timer::Id{TimerType::FinalizeTimer, uniquePoolSeed},
				Timer::TimerThread::getInstance().getTimerContainer(),
				seconds_t{60},
				seconds_t{30});

			m_initialized = true;
		}

		auto store(ref_ptr_t<TObject> obj) -> void {
			if (!m_initialized) {
				throw InvalidOperationException{"Must initialize before use"};
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
				ref_ptr_t<TObject> &v = m_waiting[i - 1];
				unsigned long useCount = (unsigned long)v.use_count();
				if (useCount <= m_minimumUseCount) {
					m_waiting.erase(std::begin(m_waiting) + (i - 1));
					i--;
				}
			}
		}

		bool m_initialized = false;
		uint32_t m_minimumUseCount = 0;
		vector_t<ref_ptr_t<TObject>> m_waiting;
	};
}