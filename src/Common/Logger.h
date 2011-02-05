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

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <string>

using std::string;
using std::tr1::unordered_map;

namespace LogDestinations {
	enum LogDestinations {
		None,
		File,
		Console,
		Sql,
		FileSql,
		FileConsole,
		SqlConsole,
		FileSqlConsole
	};
}

namespace LogTypes {
	enum LogTypes {
		Info = 1, // Starting at 1 so it's easy enough to simply insert the logtype for SQL
		Warning,
		Debug,
		Error,
		CriticalError,
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
		ScriptLog
	};
}

struct LogMessage {
	LogTypes::LogTypes type;
	time_t time;
	string message;
	string identifier;
};

// Base logger
class Logger {
public:
	Logger(const string &format, const string &timeFormat, int16_t serverType);
	virtual ~Logger();
	virtual void log(LogTypes::LogTypes type, const string &identifier, const string &message);

	int16_t getServerType() const { return m_serverType; }
	const string & getFormat() const { return m_format; }
	const string & getTimeFormat() const { return m_timeFormat; }
protected:
	string m_format;
	string m_timeFormat;
	int16_t m_serverType;

	static string getLogFormatted(LogTypes::LogTypes type, Logger *logger, const string &id, const string &message);
	static string getLevelString(LogTypes::LogTypes type);
	static string getTimeFormatted(const string &fmt);
	static string getServerTypeString(int16_t serverType);
};