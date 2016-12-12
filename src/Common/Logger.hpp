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
#pragma once

#include "common/database.hpp"
#include "common/server_type.hpp"
#include "common/types.hpp"
#include <string>
#include <unordered_map>

namespace vana {
	namespace log_destinations {
		enum log_destinations : int32_t {
			none = 0x00,
			file = 0x01,
			console = 0x02,
			sql = 0x04,
			// If more constants are added, please add them to ConfigFile.cpp as well
			file_sql = file | sql,
			file_console = file | console,
			sql_console = sql | console,
			file_sql_console = file | sql | console,
			all = file_sql_console
		};
	}

	enum class log_type {
		none = 0,
		info = 1, // Starting at 1 so it's easy enough to simply insert the logtype for SQL
		warning,
		debug,
		error,
		debug_error,
		critical_error,
		hacking,
		server_connect,
		server_disconnect,
		server_auth_failure,
		login,
		login_auth_failure,
		logout,
		client_error,
		gm_command,
		admin_command,
		boss_kill,
		trade,
		shop_transaction,
		storage_transaction,
		instance_begin,
		drop,
		chat,
		whisper,
		malformed_packet,
		script_log,
		ban,
		unban,
		// If any more are added, please add them to the implementation of get_level_string
	};

	struct log_message {
		log_type type;
		time_t time;
		string message;
		opt_string identifier;
	};

	// Base logger
	class base_logger {
	public:
		base_logger(const string &filename, const string &format, const string &time_format, server_type type, size_t buffer_size = 10);
		virtual ~base_logger() = default;
		virtual auto log(log_type type, const opt_string &identifier, const string &message) -> void { }

		auto get_server_type() const -> server_type { return m_server_type; }
		auto get_format() const -> const string & { return m_format; }
		auto get_time_format() const -> const string & { return m_time_format; }
	protected:
		base_logger() = default;
		static auto format_log(const string &format, log_type type, base_logger *logger, const opt_string &id, const string &message) -> string;
		static auto format_log(const string &format, log_type type, base_logger *logger, time_t time, const opt_string &id, const string &message) -> string;
	private:
		friend struct log_replacements;

		struct replacement_args {
			log_type type;
			base_logger *logger;
			time_t time;
			const opt_string &id;
			const string &msg;
			replacement_args(log_type type, base_logger *logger, time_t time, const opt_string &id, const string &msg);
		};

		struct log_replacements {
			log_replacements();
			using func = function<void(out_stream &, const replacement_args &args)>;
			auto add(const string &key, func func) -> void;
			static auto get_level_string(log_type type) -> string;
			static auto get_server_type_string(server_type type) -> string;

			hash_map<string, func> m_replacements;
		};

		string m_format;
		string m_time_format;
		server_type m_server_type;
	};
}