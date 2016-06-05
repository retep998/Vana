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

#include "common/types_temp.hpp"
#include <string>
#include <unordered_map>

namespace vana {
	class variables {
	public:
		auto set_variable(const string &name, const string &value) -> void;
		auto get_variable(const string &name) const -> string;
		auto delete_variable(const string &name) -> void;

		auto operator[](const string &key) const -> string;
	protected:
		hash_map<string, string> m_variables;
	};
}