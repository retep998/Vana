/*
Copyright (C) 2008-2011 Vana Development Team

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

#pragma warning(push)
#pragma warning(disable : 4275)
#include "mysql++.h"
#pragma warning(pop)

#include "Types.h"
#include <boost/thread/tss.hpp>

struct DbConfig;

class Database {
public:
	typedef boost::thread_specific_ptr<mysqlpp::Connection> tsConn;

	static void connectCharDb();
	static void connectDataDb();
	static mysqlpp::Connection & getCharDb();
	static mysqlpp::Connection & getDataDb();
private:
	static tsConn m_chardb;
	static tsConn m_datadb;
};

inline
mysqlpp::Connection & Database::getCharDb() {
	if (m_chardb.get() == nullptr) {
		connectCharDb();
	}
	return *m_chardb.get();
}

inline
mysqlpp::Connection & Database::getDataDb() {
	if (m_datadb.get() == nullptr) {
		connectDataDb();
	}
	return *m_datadb.get();
}