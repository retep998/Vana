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
#include "sql_logger.hpp"
#include "common/io/database.hpp"

namespace vana {
namespace log {

sql_logger::sql_logger(const string &filename, const string &format, const string &time_format, server_type type, size_t buffer_size) :
	base_logger{filename, format, time_format, type, buffer_size},
	m_buffer_size{buffer_size}
{
	m_buffer.reserve(buffer_size);
}

sql_logger::~sql_logger() {
	flush();
}

auto sql_logger::log(vana::log::type type, const opt_string &identifier, const string &message) -> void {
	sql_log m;
	m.type = type;
	m.message = message;
	m.time = time(nullptr);
	m.identifier = identifier;
	m_buffer.push_back(m);
	if (m_buffer.size() >= m_buffer_size) {
		flush();
	}
}

auto sql_logger::flush() -> void {
	if (m_buffer.size() > 0) {
		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		server_type_underlying type = static_cast<server_type_underlying>(get_server_type());
		int32_t log_type = 0;
		opt_string identifier;
		// For GCC, GCC doesn't interpret operators very well
		identifier = "";
		string message = "";
		unix_time log_time;

		soci::statement st = (sql.prepare
			<< "INSERT INTO " << db.make_table(vana::table::logs) << " (log_time, origin, info_type, identifier, message) "
			<< "VALUES (:time, :origin, :info_type, :identifier, :message)",
			soci::use(log_time, "time"),
			soci::use(type, "origin"),
			soci::use(log_type, "info_type"),
			soci::use(identifier, "identifier"),
			soci::use(message, "message"));

		for (const auto &buffered_message : m_buffer) {
			log_type = static_cast<int32_t>(buffered_message.type);
			log_time = buffered_message.time;
			identifier = buffered_message.identifier;
			message = buffered_message.message;
			st.execute(true);
		}

		m_buffer.clear();
	}
}

}
}