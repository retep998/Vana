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
#include "common/util/meso_modify_result.hpp"
#include <string>
#include <vector>

namespace vana {
	namespace util {
		class meso_inventory {
		public:
			meso_inventory() = default;
			meso_inventory(game_mesos mesos);

			auto has_any() const -> bool;
			auto get_mesos() const -> game_mesos;
			auto can_add_mesos(game_mesos mesos) const -> stack_result;
			auto can_take_mesos(game_mesos mesos) const -> stack_result;
			auto can_modify_mesos(game_mesos mesos) const -> stack_result;
			auto can_accept(const meso_inventory &other) const -> stack_result;

			auto set_mesos(game_mesos new_mesos) -> result;
			auto add_mesos(game_mesos mesos, bool allow_partial = false) -> meso_modify_result;
			auto take_mesos(game_mesos mesos, bool allow_partial = false) -> meso_modify_result;
			auto modify_mesos(game_mesos mesos, bool allow_partial = false) -> meso_modify_result;
			auto accept(meso_inventory &other, bool allow_partial = false) -> meso_modify_result;
		private:
			auto modify_mesos_internal(stack_result query, game_mesos mesos, bool allow_partial) -> meso_modify_result;

			game_mesos m_mesos = 0;
		};
	}
}