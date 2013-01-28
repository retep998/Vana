/*
Copyright (C) 2008-2013 Vana Development Team

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
# include <winsock2.h> // Prevent conflict with boost::asio
#endif

#include <soci.h>
#include "SociExtensions.h"
#include "Types.h"
#include <memory>
#include <string>

struct DbConfig;

class Database {
public:
	static void connectCharDb();
	static void connectDataDb();
	static soci::session & getCharDb();
	static soci::session & getDataDb();
	template <typename T> static T getLastId(soci::session &sql);
private:
	static thread_local soci::session *m_chardb;
	static thread_local soci::session *m_datadb;
	static std::string buildConnectionString(const DbConfig &conf);
};

inline
soci::session & Database::getCharDb() {
	if (m_chardb == nullptr) {
		connectCharDb();
	}
	return *m_chardb;
}

inline
soci::session & Database::getDataDb() {
	if (m_datadb == nullptr) {
		connectDataDb();
	}
	return *m_datadb;
}

template<typename T>
T Database::getLastId(soci::session &sql) {
	T val;
	sql.once << "SELECT LAST_INSERT_ID()", soci::into(val);
	return val;
}