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
#include "Variables.hpp"

namespace vana {

auto variables::delete_variable(const string &name) -> void {
	auto kvp = m_variables.find(name);
	if (kvp != std::end(m_variables)) {
		m_variables.erase(kvp);
	}
}

auto variables::set_variable(const string &name, const string &val) -> void {
	m_variables[name] = val;
}

auto variables::get_variable(const string &name) const -> string {
	auto kvp = m_variables.find(name);
	return kvp != std::end(m_variables) ? kvp->second : "";
}

auto variables::operator[](const string &key) const -> string {
	return get_variable(key);
}

}