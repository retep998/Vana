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

#include "common/types.hpp"

namespace vana {
	namespace log {
		enum class destination : int32_t {
			none = 0x00,
			file = 0x01,
			console = 0x02,
			sql = 0x04,
			// If more constants are added, please add them to config_file.cpp as well
			file_sql = file | sql,
			file_console = file | console,
			sql_console = sql | console,
			file_sql_console = file | sql | console,
			all = file_sql_console,
		};
	}
}