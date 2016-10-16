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
#include "common/timer/timer.hpp"
#include "common/timer/container.hpp"
#include "common/timer/thread.hpp"
#include "common/util/time.hpp"
#include <memory>

namespace vana {
namespace timer {

auto timer::create(const func f, const id &id, ref_ptr<container> container, const duration &difference_from_now, const duration &repeat) -> void {
	if (container == nullptr) {
		container = vana::timer::thread::get_instance().get_timer_container();
	}

	ref_ptr<timer> timer = make_ref_ptr<vana::timer::timer>(f, id, container, difference_from_now, repeat);
	container->register_timer(timer, id, timer->m_run_at);
}

timer::timer(const func f, const id &id, ref_ptr<container> container, const duration &difference_from_now, const duration &repeat) :
	m_id{id},
	m_container{container},
	m_repeat_time{repeat},
	m_function{f}
{
	m_repeat = repeat.count() != 0;
	m_run_at = vana::util::time::get_now_with_time_added(difference_from_now);
}

auto timer::remove_from_container() const -> void {
	if (ref_ptr<container> container = m_container.lock()) {
		container->remove_timer(m_id);
	}
}

auto timer::run(const time_point &now) const -> run_result {
	m_function(now);
	return m_repeat ? run_result::reset : run_result::complete;
}

auto timer::reset(const time_point &now) -> time_point {
	m_run_at = now + m_repeat_time;
	return m_run_at;
}

auto timer::get_time_left() const -> duration {
	return m_run_at - vana::util::time::get_now();
}

}
}