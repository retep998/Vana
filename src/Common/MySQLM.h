/*
Copyright (C) 2008 Vana Development Team

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
#ifndef MYSQL_H
#define MYSQL_H

#define MYSQLPP_MYSQL_HEADERS_BURIED
#pragma warning(push)
#pragma warning(disable : 4275)
#include "mysql++/mysql++.h"
#pragma warning(pop)
#undef MYSQLPP_MYSQL_HEADERS_BURIED

class Database {
public:
	static void connect();
	static mysqlpp::Connection & getCharDB();
	static mysqlpp::Connection & getDataDB();
private:
	static mysqlpp::Connection chardb;
	static mysqlpp::Connection datadb;
};

#endif
