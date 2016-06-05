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

#include "common/buff_info.hpp"
#include "common/buff_skill_value.hpp"
#include "common/types.hpp"

namespace vana {
	class buff {
	public:
		buff();
		buff(const init_list<buff_info> &buff_info);
		buff(const vector<buff_info> &buff_info);

		auto get_delay() const -> milliseconds;
		auto get_buff_info() const -> const vector<buff_info> &;
		auto is_selection_buff() const -> bool;
		auto any_buffs() const -> bool;
		auto any_acts() const -> bool;
		auto with_buffs(const vector<buff_info> &buff_info) const -> buff;
		auto with_delay(milliseconds value) const -> buff;
	private:
		auto parse_buffs_for_data() -> void;
		auto sort() -> void;

		bool m_is_selection_buff = false;
		bool m_any_acts = false;
		milliseconds m_delay = milliseconds{0};
		vector<buff_info> m_buff_info;
	};
}