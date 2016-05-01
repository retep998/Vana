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

#include "Common/TimerContainer.hpp"
#include "Common/Types.hpp"
#include <memory>

namespace Vana {
	class TimerContainerHolder {
	public:
		TimerContainerHolder() {
			m_timers = make_ref_ptr<Timer::Container>();
		}
	protected:
		auto clearTimers() -> void { m_timers.reset(); }
		auto getTimers() const -> ref_ptr_t<Timer::Container> { return m_timers; }
	private:
		ref_ptr_t<Timer::Container> m_timers;
	};
}