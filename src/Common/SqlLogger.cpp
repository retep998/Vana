/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "SqlLogger.hpp"
#include "Database.hpp"

SqlLogger::SqlLogger(const string_t &filename, const string_t &format, const string_t &timeFormat, ServerType serverType, size_t bufferSize) :
	Logger(filename, format, timeFormat, serverType, bufferSize),
	m_bufferSize(bufferSize)
{
	m_buffer.reserve(bufferSize);
}

SqlLogger::~SqlLogger() {
	flush();
}

auto SqlLogger::log(LogType type, const opt_string_t &identifier, const string_t &message) -> void {
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

auto SqlLogger::flush() -> void {
	if (m_buffer.size() > 0) {
		soci::session &sql = Database::getCharDb();
		server_type_t serverType = static_cast<server_type_t>(getServerType());
		int32_t logType = 0;
		opt_string_t identifier;
		// For GCC, GCC doesn't interpret operators very well
		identifier = "";
		string_t message = "";
		unix_time_t logTime;

		soci::statement st = (sql.prepare
			<< "INSERT INTO logs (log_time, origin, info_type, identifier, message) "
			<< "VALUES (:time, :origin, :infoType, :identifier, :message)",
			soci::use(logTime, "time"),
			soci::use(serverType, "origin"),
			soci::use(logType, "infoType"),
			soci::use(identifier, "identifier"),
			soci::use(message, "message"));

		for (const auto &bufferedMessage : m_buffer) {
			logType = static_cast<int32_t>(bufferedMessage.type);
			logTime = bufferedMessage.time;
			identifier = bufferedMessage.identifier;
			message = bufferedMessage.message;
			st.execute(true);
		}

		m_buffer.clear();
	}
}