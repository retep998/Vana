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
#include "Buff.hpp"
#include "common/Algorithm.hpp"

namespace vana {

buff::buff()
{
}

buff::buff(const init_list<buff_info> &buff_info) :
	m_buff_info{buff_info}
{
	parse_buffs_for_data();
	sort();
}

buff::buff(const vector<buff_info> &buff_info) :
	m_buff_info{buff_info}
{
	parse_buffs_for_data();
	sort();
}

auto buff::parse_buffs_for_data() -> void {
	for (const auto &buff : m_buff_info) {
		if (ext::in_range_inclusive<int16_t>(buff.get_chance(), 0, 99)) {
			m_is_selection_buff = true;
		}
		if (buff.has_act()) {
			m_any_acts = true;
		}
	}
}

auto buff::sort() -> void {
	std::sort(m_buff_info.begin(), m_buff_info.end(), [](const buff_info &l, const buff_info &r) {
		return l.get_bit_position() < r.get_bit_position();
	});
}

auto buff::get_delay() const -> milliseconds {
	return m_delay;
}

auto buff::get_buff_info() const -> const vector<buff_info> & {
	return m_buff_info;
}

auto buff::is_selection_buff() const -> bool {
	return m_is_selection_buff;
}

auto buff::any_buffs() const -> bool {
	return m_buff_info.size() > 0;
}

auto buff::any_acts() const -> bool {
	return m_any_acts;
}

auto buff::with_buffs(const vector<buff_info> &buff_info) const -> buff {
	buff result{*this};
	result.m_buff_info = buff_info;
	result.sort();
	return result;
}

auto buff::with_delay(milliseconds value) const -> buff {
	buff result{*this};
	result.m_delay = value;
	return result;
}

}