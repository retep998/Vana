/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "LuaVariant.hpp"

LuaVariant::LuaVariant() :
	m_boolean{false},
	m_type{LuaType::Nil}
{
}

LuaVariant::LuaVariant(const LuaVariant &value) :
	m_type{value.m_type}
{
	switch (m_type) {
		case LuaType::Bool:
			m_boolean = value.m_boolean;
			break;
		case LuaType::Nil:
			m_boolean = false;
			break;
		case LuaType::String:
			new (&m_string) string_t{value.m_string};
			break;
		case LuaType::Table:
			new (&m_table) table_map_t{value.m_table};
			break;
		case LuaType::Number:
			m_number = value.m_number;
			break;
		default:
			throw NotImplementedException{"LuaType"};
	}
}

LuaVariant::LuaVariant(bool value) :
	m_boolean{value},
	m_type{LuaType::Bool}
{
}

LuaVariant::LuaVariant(int32_t value) :
	m_number{static_cast<double>(value)},
	m_type{LuaType::Number}
{
}

LuaVariant::LuaVariant(double value) :
	m_number{value},
	m_type{LuaType::Number}
{
}

LuaVariant::LuaVariant(string_t value) :
	m_string{value},
	m_type{LuaType::String}
{
}

LuaVariant::~LuaVariant() {
	if (m_type == LuaType::String) m_string.~basic_string();
}

auto LuaVariant::operator =(const LuaVariant &value) -> LuaVariant & {
	m_type = value.m_type;
	switch (m_type) {
		case LuaType::Bool:
			m_boolean = value.m_boolean;
			break;
		case LuaType::Nil:
			m_boolean = false;
			break;
		case LuaType::String:
			new (&m_string) string_t{value.m_string};
			break;
		case LuaType::Table:
			new (&m_table) table_map_t{value.m_table};
			break;
		case LuaType::Number:
			m_number = value.m_number;
			break;
		default:
			throw NotImplementedException{"LuaType"};
	}
	return *this;
}

auto LuaVariant::getType() const -> LuaType {
	return m_type;
}

auto LuaVariant::is(LuaType type) const -> bool {
	return m_type == type;
}

auto LuaVariant::isAny(init_list_t<LuaType> types) const -> bool {
	return std::any_of(std::begin(types), std::end(types), [&](LuaType type) -> bool {
		return type == m_type;
	});
}

