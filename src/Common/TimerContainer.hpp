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

#include "Timer.hpp"
#include "TimerId.hpp"
#include "TimerType.hpp"
#include "Types.hpp"
#include <functional>
#include <memory>
#include <unordered_map>

namespace Timer {

class Container {
public:
	template <typename TDuration>
	auto getRemainingTime(const Id &id) const -> TDuration;
	auto isTimerRunning(const Id &id) const -> bool;
	auto registerTimer(ref_ptr_t<Timer> timer, const Id &id, time_point_t runAt) -> void;
	auto removeTimer(const Id &id) -> void;
private:
	hash_map_t<Id, ref_ptr_t<Timer>> m_timers;
};

template <typename TDuration>
auto Container::getRemainingTime(const Id &id) const -> TDuration {
	auto iter = m_timers.find(id);
	if (iter != std::end(m_timers)) {
		return duration_cast<TDuration>(iter->second->getTimeLeft());
	}
	return TDuration{0};
}


}