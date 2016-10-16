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
#include <string>

namespace vana {
	class abstract_server;

	namespace data {
		namespace initialize {
			auto check_schema_version(abstract_server *server, bool update = false) -> result;
			auto check_mcdb_version(abstract_server *server) -> result;
			auto set_users_offline(abstract_server *server, int32_t online_id) -> void;

			const int32_t output_width = 27;
		}
	}
}