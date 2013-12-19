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
#pragma once

#include "Logger.h"
#include "Types.h"
#include <functional>
#include <string>
#include <unordered_map>

using std::string;

// Prevents them from being loaded a bunch of times, keeps them in memory for the life of the application
class LogReplacements {
public:
	static string format(const string &format, LogTypes::LogTypes logType, Logger *logger, time_t time, const opt_string &id, const string &message);
private:
	LogReplacements();
	LogReplacements(const LogReplacements &) = delete;
	static LogReplacements *singleton;

	struct ReplacementArgs {
		LogTypes::LogTypes logType;
		Logger *logger;
		time_t time;
		const opt_string &id;
		const string &msg;
		ReplacementArgs(LogTypes::LogTypes logType, Logger *logger, time_t time, const opt_string &id, const string &msg);
	};

	typedef std::function<void(std::ostringstream &, const ReplacementArgs &args)> func_t;
	void add(const string &key, func_t func);
	std::unordered_map<string, func_t> m_replacementMap;
};