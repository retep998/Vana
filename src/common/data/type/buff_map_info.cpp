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
#include "buff_map_info.hpp"

namespace vana {
namespace data {
namespace type {

buff_map_info::buff_map_info()
{
}

buff_map_info::buff_map_info(buff_skill_value value) :
	m_value{value}
{
}

buff_map_info::buff_map_info(uint8_t size, buff_skill_value value) :
	m_size{size},
	m_value{value}
{
}

buff_map_info::buff_map_info(uint8_t size, int64_t predefined_value) :
	m_size{size},
	m_predefined_value{predefined_value}
{
}

auto buff_map_info::get_predefined_value() const -> int64_t {
	return m_predefined_value;
}

auto buff_map_info::get_size() const -> uint8_t {
	return m_size;
}

auto buff_map_info::get_value() const -> buff_skill_value {
	return m_value;
}

auto buff_map_info::with_value(buff_skill_value value) const -> buff_map_info {
	buff_map_info result{*this};
	result.m_value= value;
	return result;
}

auto buff_map_info::with_predefined_value(int16_t value) const -> buff_map_info {
	buff_map_info result{*this};
	result.m_value = buff_skill_value::predefined;
	result.m_predefined_value = value;
	return result;
}

}
}
}