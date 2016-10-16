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

#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	namespace util {
		struct meso_modify_result {
			auto get_result() const -> stack_result { return m_result; }
			auto get_final_amount() const -> game_mesos { return m_final_mesos; }
			auto get_applied_amount() const -> game_mesos { return m_applied_mesos; }

			meso_modify_result(
				stack_result result,
				game_mesos final_mesos,
				game_mesos applied_mesos) :
				m_result{result},
				m_final_mesos{final_mesos},
				m_applied_mesos{applied_mesos}
			{
			}

		private:
			stack_result m_result;
			game_mesos m_final_mesos;
			game_mesos m_applied_mesos;
		};
	}
}