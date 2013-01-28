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
#include "SqlLogger.h"
#include "Database.h"

SqlLogger::SqlLogger(const string &filename, const string &format, const string &timeFormat, int16_t serverType, size_t bufferSize) :
	Logger(filename, format, timeFormat, serverType, bufferSize),
	m_bufferSize(bufferSize)
{
	m_buffer.reserve(bufferSize);
}

SqlLogger::~SqlLogger() {
	flush();
}

void SqlLogger::log(LogTypes::LogTypes type, const opt_string &identifier, const string &message) {
	LogMessage m;
	m.type = type;
	m.message = message;
	m.time = time(nullptr);
	m.identifier = identifier;
	m_buffer.push_back(m);
	if (m_buffer.size() >= m_bufferSize) {
		flush();
	}
}

void SqlLogger::flush() {
	if (m_buffer.size() > 0) {
		soci::session &sql = Database::getCharDb();
		int16_t serverType = getServerType();
		int32_t logType = 0;
		opt_string identifier;
		// For GCC, GCC doesn't interpret operators very well
		identifier = "";
		string message = "";
		unix_time_t logTime;

		soci::statement st = (sql.prepare
			<< "INSERT INTO logs (log_time, origin, info_type, identifier, message) "
			<< "VALUES (:time, :origin, :infoType, :identifier, :message)",
			soci::use(logTime, "time"),
			soci::use(serverType, "origin"),
			soci::use(logType, "infoType"),
			soci::use(identifier, "identifier"),
			soci::use(message, "message"));

		for (vector<LogMessage>::const_iterator iter = m_buffer.begin(); iter != m_buffer.end(); ++iter) {
			logType = iter->type;
			logTime = iter->time;
			identifier = iter->identifier;
			message = iter->message;
			st.execute(true);
		}

		m_buffer.clear();
	}
}