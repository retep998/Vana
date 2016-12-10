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

#include "common/io/version_check_result.hpp"
#include "common/types.hpp"
#include <map>
#include <string>

namespace vana {
	class abstract_server;

	namespace io {
		class database_updater {
		public:
			database_updater(abstract_server *server, bool update);
			auto check_version() -> version_check_result;
			auto update() -> void;
		private:
			auto load_database_info() -> void;
			auto load_sql_files() const -> pair<size_t, ord_map<int32_t, string>>;
			auto update(size_t version) -> void;
			auto run_queries(const string &filename) -> void;
			static auto create_info_table() -> void;
			static auto update_info_table(size_t version) -> void;

			size_t m_file_version = 0;
			size_t m_sql_version = 0;
			bool m_update = false;
			bool m_db_available = false;
			abstract_server *m_server = nullptr;
		};
	}
}