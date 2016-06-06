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
#include "lua_variant.hpp"

namespace vana {
namespace lua {

lua_variant::lua_variant() :
	m_boolean{false},
	m_type{lua_type::nil}
{
}

lua_variant::lua_variant(const lua_variant &value) :
	m_type{value.m_type}
{
	switch (m_type) {
		case lua_type::boolean:
			m_boolean = value.m_boolean;
			break;
		case lua_type::nil:
			m_boolean = false;
			break;
		case lua_type::string:
			new (&m_string) string{value.m_string};
			break;
		case lua_type::table:
			new (&m_table) table_map{value.m_table};
			break;
		case lua_type::number:
			m_number = value.m_number;
			break;
		default:
			throw not_implemented_exception{"lua_type"};
	}
}

lua_variant::lua_variant(bool value) :
	m_boolean{value},
	m_type{lua_type::boolean}
{
}

lua_variant::lua_variant(int32_t value) :
	m_number{static_cast<double>(value)},
	m_type{lua_type::number}
{
}

lua_variant::lua_variant(double value) :
	m_number{value},
	m_type{lua_type::number}
{
}

lua_variant::lua_variant(string value) :
	m_string{value},
	m_type{lua_type::string}
{
}

lua_variant::~lua_variant() {
	if (m_type == lua_type::string) m_string.~basic_string();
}

auto lua_variant::operator =(const lua_variant &value) -> lua_variant & {
	m_type = value.m_type;
	switch (m_type) {
		case lua_type::boolean:
			m_boolean = value.m_boolean;
			break;
		case lua_type::nil:
			m_boolean = false;
			break;
		case lua_type::string:
			new (&m_string) string{value.m_string};
			break;
		case lua_type::table:
			new (&m_table) table_map{value.m_table};
			break;
		case lua_type::number:
			m_number = value.m_number;
			break;
		default:
			throw not_implemented_exception{"lua_type"};
	}
	return *this;
}

auto lua_variant::get_type() const -> lua_type {
	return m_type;
}

auto lua_variant::is(lua_type type) const -> bool {
	return m_type == type;
}

auto lua_variant::is_any_of(init_list<lua_type> types) const -> bool {
	return std::any_of(std::begin(types), std::end(types), [&](lua_type type) -> bool {
		return type == m_type;
	});
}

}
}