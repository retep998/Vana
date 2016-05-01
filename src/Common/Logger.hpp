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

#include "Common/Database.hpp"
#include "Common/ServerType.hpp"
#include "Common/Types.hpp"
#include <string>
#include <unordered_map>

namespace Vana {
	namespace LogDestinations {
		enum LogDestinations : int32_t {
			None = 0x00,
			File = 0x01,
			Console = 0x02,
			Sql = 0x04,
			// If more constants are added, please add them to ConfigFile.cpp as well
			FileSql = File | Sql,
			FileConsole = File | Console,
			SqlConsole = Sql | Console,
			FileSqlConsole = File | Sql | Console,
			All = FileSqlConsole
		};
	}

	enum class LogType {
		None,
		Info = 1, // Starting at 1 so it's easy enough to simply insert the logtype for SQL
		Warning,
		Debug,
		Error,
		DebugError,
		CriticalError,
		Hacking,
		ServerConnect,
		ServerDisconnect,
		ServerAuthFailure,
		Login,
		LoginAuthFailure,
		Logout,
		ClientError,
		GmCommand,
		AdminCommand,
		BossKill,
		Trade,
		ShopTransaction,
		StorageTransaction,
		InstanceBegin,
		Drop,
		Chat,
		Whisper,
		MalformedPacket,
		ScriptLog,
		Ban,
		Unban
	};

	struct LogMessage {
		LogType type;
		time_t time;
		string_t message;
		opt_string_t identifier;
	};

	// Base logger
	class Logger {
	public:
		Logger(const string_t &filename, const string_t &format, const string_t &timeFormat, ServerType serverType, size_t bufferSize = 10);
		virtual ~Logger() = default;
		virtual auto log(LogType type, const opt_string_t &identifier, const string_t &message) -> void { }

		auto getServerType() const -> ServerType { return m_serverType; }
		auto getFormat() const -> const string_t & { return m_format; }
		auto getTimeFormat() const -> const string_t & { return m_timeFormat; }
	protected:
		Logger() = default;
		static auto formatLog(const string_t &format, LogType type, Logger *logger, const opt_string_t &id, const string_t &message) -> string_t;
		static auto formatLog(const string_t &format, LogType type, Logger *logger, time_t time, const opt_string_t &id, const string_t &message) -> string_t;
	private:
		friend struct LogReplacements;

		struct ReplacementArgs {
			LogType logType;
			Logger *logger;
			time_t time;
			const opt_string_t &id;
			const string_t &msg;
			ReplacementArgs(LogType logType, Logger *logger, time_t time, const opt_string_t &id, const string_t &msg);
		};

		struct LogReplacements {
			LogReplacements();
			using func_t = function_t<void(out_stream_t &, const ReplacementArgs &args)>;
			auto add(const string_t &key, func_t func) -> void;
			static auto getLevelString(LogType type) -> string_t;
			static auto getServerTypeString(ServerType type) -> string_t;

			hash_map_t<string_t, func_t> m_replacementMap;
		};

		string_t m_format;
		string_t m_timeFormat;
		ServerType m_serverType;
	};
}