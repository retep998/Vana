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
#pragma once

#include "Types.hpp"
#include <string>
#include <unordered_map>

namespace Vana {
	class Variables {
	public:
		auto setVariable(const string_t &name, const string_t &value) -> void;
		auto getVariable(const string_t &name) const -> string_t;
		auto deleteVariable(const string_t &name) -> void;

		auto operator[](const string_t &key) const -> string_t;
	protected:
		hash_map_t<string_t, string_t> m_variables;
	};
}