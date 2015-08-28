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

#ifdef WIN32
# include <winsock2.h> // Prevent conflict with asio
#endif

#include <soci.h>
#include "SociExtensions.hpp"
#include "Types.hpp"
#include <memory>
#include <string>

namespace Vana {
	namespace soci = ::soci;
	struct DbConfig;

	class Database {
	public:
		Database(const DbConfig &conf, bool includeDatabase);

		static auto getCharDb() -> Database &;
		static auto getDataDb() -> Database &;
		static auto initCharDb() -> Database &;

		auto getSession() -> soci::session &;
		auto getSchema() const -> string_t;
		auto getTablePrefix() const -> string_t;
		auto makeTable(const string_t &table) const -> string_t;
		template <typename TIdentifier>
		auto getLastId() -> TIdentifier;
		auto tableExists(const string_t &table) -> bool;

		static auto schemaExists(soci::session &sql, const string_t &schema) -> bool;
		static auto tableExists(soci::session &sql, const string_t &schema, const string_t &table) -> bool;
	private:
		owned_ptr_t<soci::session> m_session;
		string_t m_schema;
		string_t m_tablePrefix;

		static auto connectCharDb() -> void;
		static auto connectDataDb() -> void;
		static auto buildConnectionString(const DbConfig &conf, bool includeDatabase) -> string_t;

		static thread_local owned_ptr_t<Database> m_chardb;
		static thread_local owned_ptr_t<Database> m_datadb;
	};

	inline
	auto Database::getCharDb() -> Database & {
		if (m_chardb == nullptr) {
			connectCharDb();
		}
		return *m_chardb;
	}

	inline
	auto Database::getDataDb() -> Database & {
		if (m_datadb == nullptr) {
			connectDataDb();
		}
		return *m_datadb;
	}

	template <typename TIdentifier>
	auto Database::getLastId() -> TIdentifier {
		TIdentifier val;
		getSession().once << "SELECT LAST_INSERT_ID()", soci::into(val);
		return val;
	}
}