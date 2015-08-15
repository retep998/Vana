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

struct DbConfig;

class Database {
public:
	static auto getCharDb() -> soci::session &;
	static auto getCharSchema() -> string_t;
	static auto getDataDb() -> soci::session &;
	static auto initCharDb() -> soci::session &;
	template <typename TIdentifier>
	static auto getLastId(soci::session &sql) -> TIdentifier;
	static auto makeCharTable(const string_t &table) -> string_t;
	static auto makeDataTable(const string_t &table) -> string_t;
private:
	static auto connectCharDb() -> void;
	static auto connectDataDb() -> void;
	static auto buildConnectionString(const DbConfig &conf, bool includeDatabase) -> string_t;

	static thread_local owned_ptr_t<soci::session> m_chardb;
	static thread_local owned_ptr_t<soci::session> m_datadb;
	static string_t m_charDatabase;
	static string_t m_charTablePrefix;
	static string_t m_dataTablePrefix;
};

inline
auto Database::getCharDb() -> soci::session & {
	if (m_chardb == nullptr) {
		connectCharDb();
	}
	return *m_chardb;
}

inline
auto Database::getDataDb() -> soci::session & {
	if (m_datadb == nullptr) {
		connectDataDb();
	}
	return *m_datadb;
}

template <typename TIdentifier>
auto Database::getLastId(soci::session &sql) -> TIdentifier {
	TIdentifier val;
	sql.once << "SELECT LAST_INSERT_ID()", soci::into(val);
	return val;
}