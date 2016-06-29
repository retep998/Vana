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

#include "common/data/type/drop_info.hpp"
#include "common/data/type/global_drop_info.hpp"
#include "common/types.hpp"
#include <unordered_map>
#include <vector>

namespace vana {
	namespace data {
		namespace provider {
			class drop {
			public:
				auto load_data() -> void;

				auto get_drops(int32_t object_id) const -> const vector<data::type::drop_info> &;
				auto get_global_drops() const -> const vector<data::type::global_drop_info> &;
			private:
				auto load_drops() -> void;
				auto load_global_drops() -> void;

				vector<pair<int32_t, vector<data::type::drop_info>>> m_drop_info;
				vector<data::type::global_drop_info> m_global_drops;
			};
		}
	}
}