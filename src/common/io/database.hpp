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

#include "common/data/table.hpp"
#include "common/soci_extensions.hpp"
#include "common/table.hpp"
#include "common/types.hpp"
#include <memory>
#include <string>

namespace vana {
	namespace soci = ::soci;
	namespace config {
		struct database;
	}

	namespace io {
		class database {
		public:
			database(const config::database &conf, bool include_database);

			static auto get_char_db() -> database &;
			static auto get_data_db() -> database &;
			static auto init_char_db() -> database &;

			auto get_session() -> soci::session &;
			auto get_schema() const -> string;
			auto get_table_prefix() const -> string;
			auto make_table(const string &table) const -> string;
			template <typename TIdentifier>
			auto get_last_id() -> TIdentifier;
			auto table_exists(const string &table) -> bool;

			static auto schema_exists(soci::session &sql, const string &schema) -> bool;
			static auto table_exists(soci::session &sql, const string &schema, const string &table) -> bool;
		private:
			owned_ptr<soci::session> m_session;
			string m_schema;
			string m_table_prefix;

			static auto connect_char_db() -> void;
			static auto connect_data_db() -> void;
			static auto build_connection_string(const config::database &conf, bool include_database) -> string;

			static thread_local owned_ptr<database> m_chardb;
			static thread_local owned_ptr<database> m_datadb;
		};

		inline
		auto database::get_char_db() -> database & {
			if (m_chardb == nullptr) {
				connect_char_db();
			}
			return *m_chardb;
		}

		inline
		auto database::get_data_db() -> database & {
			if (m_datadb == nullptr) {
				connect_data_db();
			}
			return *m_datadb;
		}

		template <typename TIdentifier>
		auto database::get_last_id() -> TIdentifier {
			TIdentifier val;
			get_session().once << "SELECT LAST_INSERT_ID()", soci::into(val);
			return val;
		}
	}
}