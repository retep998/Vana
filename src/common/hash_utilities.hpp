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
#include <cstdlib>
#include <string>
#include <vector>

namespace vana {
	namespace config {
		struct salt;
		struct salt_size;
	}

	namespace hash_utilities {
		auto hash_password(const string &password) -> string;
		auto hash_password(const string &password, const string &raw_salt, const config::salt &conf) -> string;
		auto salt_password(const string &password, const string &raw_salt, const config::salt &conf) -> string;
		auto generate_salt(const config::salt_size &conf) -> string;
	}
}