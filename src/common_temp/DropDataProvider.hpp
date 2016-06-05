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

#include "DropInfo.hpp"
#include "GlobalDropInfo.hpp"
#include "Types.hpp"
#include <unordered_map>
#include <vector>

namespace vana {
	class drop_data_provider {
	public:
		auto load_data() -> void;

		auto has_drops(int32_t object_id) const -> bool;
		auto get_drops(int32_t object_id) const -> const vector<drop_info> &;
		auto get_global_drops() const -> const vector<global_drop_info> &;
	private:
		auto load_drops() -> void;
		auto load_global_drops() -> void;

		hash_map<int32_t, vector<drop_info>> m_drop_info;
		vector<global_drop_info> m_global_drops;
	};
}