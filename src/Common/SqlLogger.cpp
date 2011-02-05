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
#include "SqlLogger.h"
#include "Database.h"

SqlLogger::SqlLogger(const string &format, const string &timeFormat, int16_t serverType, size_t bufferSize) :
Logger(format, timeFormat, serverType),
m_bufferSize(bufferSize)
{
	m_buffer.reserve(bufferSize);
}

SqlLogger::~SqlLogger() {
	flush();
}

void SqlLogger::log(LogTypes::LogTypes type, const string &identifier, const string &message) {
	LogMessage m;
	m.type = type;
	m.message = message;
	m.time = time(0);
	m.identifier = identifier;
	m_buffer.push_back(m);
	if (m_buffer.size() >= m_bufferSize) {
		flush();
	}
}

void SqlLogger::flush() {
	if (m_buffer.size() > 0) {
		mysqlpp::Query query = Database::getCharDB().query("INSERT INTO logs VALUES ");
		for (vector<LogMessage>::const_iterator iter = m_buffer.begin(); iter != m_buffer.end(); ++iter) {
			if (iter != m_buffer.begin()) {
				query << ",";
			}
			query << "(DEFAULT, "
				<< mysqlpp::quote << mysqlpp::DateTime(iter->time) << ","
				<< getServerType() << ","
				<< iter->type << ","
				<< mysqlpp::quote << iter->identifier << ","
				<< mysqlpp::quote << iter->message << ")";
		}
		query.exec();
		m_buffer.clear();
	}
}