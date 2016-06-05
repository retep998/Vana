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

#include "common/timer_container.hpp"
#include "common/types.hpp"
#include <memory>

namespace vana {
	class timer_container_holder {
	public:
		timer_container_holder() {
			m_timers = make_ref_ptr<timer::container>();
		}
	protected:
		auto clear_timers() -> void { m_timers.reset(); }
		auto get_timers() const -> ref_ptr<timer::container> { return m_timers; }
	private:
		ref_ptr<timer::container> m_timers;
	};
}