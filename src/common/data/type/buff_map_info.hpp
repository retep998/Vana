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

#include "common/data/type/buff_skill_value.hpp"
#include "common/types.hpp"
#include "common/util/hash_combine.hpp"

namespace vana {
	namespace data {
		namespace type {
			class buff_map_info {
			public:
				buff_map_info();
				buff_map_info(buff_skill_value value);
				buff_map_info(uint8_t size, buff_skill_value value);
				buff_map_info(uint8_t size, int64_t predefined_value);

				auto get_predefined_value() const -> int64_t;
				auto get_value() const -> buff_skill_value;
				auto get_size() const -> uint8_t;
				auto with_value(buff_skill_value value) const -> buff_map_info;
				auto with_predefined_value(int16_t value) const -> buff_map_info;
			private:
				int64_t m_predefined_value = 0;
				uint8_t m_size = 0;
				buff_skill_value m_value = buff_skill_value::predefined;
			};
		}
	}
}