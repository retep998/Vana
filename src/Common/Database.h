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

#include "Types.h" // For nullptr, remove when C++0x comes out
#include <boost/thread/tss.hpp> // thread_specific_ptr

class Database {
public:
	typedef boost::thread_specific_ptr<mysqlpp::Connection> tsConn;

	static void connectCharDB();
	static void connectDataDB();
	static mysqlpp::Connection & getCharDB();
	static mysqlpp::Connection & getDataDB();
private:
	static tsConn chardb;
	static tsConn datadb;
};

inline
mysqlpp::Connection & Database::getCharDB() {
	if (chardb.get() == nullptr)
		connectCharDB();
	return *chardb.get();
}

inline
mysqlpp::Connection & Database::getDataDB() {
	if (datadb.get() == nullptr)
		connectDataDB();
	return *datadb.get();
}
