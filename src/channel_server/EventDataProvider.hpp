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

#include "common/timer_container_holder.hpp"
#include "common/types_temp.hpp"
#include "common/variables_temp.hpp"
#include <memory>
#include <string>

namespace vana {
	namespace channel_server {
		class event_data_provider : public timer_container_holder {
		public:
			event_data_provider();
			auto load_data() -> void;
			auto clear_instances() -> void;
			auto get_variables() const -> variables * { return m_variables.get(); }
		private:
			auto load_events() -> void;
			auto load_instances() -> void;
			auto start_instance(const string &name, const duration &time, const duration &repeat = seconds{0}) -> void;

			owned_ptr<variables> m_variables;
		};
	}
}