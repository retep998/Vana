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
#include "LogReplacements.h"
#include "TimeUtilities.h"
#include <utility>

LogReplacements * LogReplacements::singleton = new LogReplacements();

LogReplacements::ReplacementArgs::ReplacementArgs(LogTypes::LogTypes logType, Logger *logger, time_t time, const opt_string &id, const string &msg) :
	logType(logType),
	logger(logger),
	time(time),
	id(id),
	msg(msg)
{
}

LogReplacements::LogReplacements() {
	add("%yy", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getYear(true, args.time);
	});
	add("%YY", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getYear(false, args.time);
	});
	add("%mm", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getMonth(args.time);
	});
	add("%MM", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getMonth(args.time);
	});
	add("%oo", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getMonthString(true, args.time);
	});
	add("%OO", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getMonthString(false, args.time);
	});
	add("%dd", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getDate(args.time);
	});
	add("%DD", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getDate(args.time);
	});
	add("%aa", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getDayString(true, args.time);
	});
	add("%AA", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getDayString(false, args.time);
	});
	add("%hh", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getHour(true, args.time);
	});
	add("%HH", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getHour(true, args.time);
	});
	add("%mi", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getHour(false, args.time);
	});
	add("%MI", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getHour(false, args.time);
	});
	add("%ii", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getMinute(args.time);
	});
	add("%II", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getMinute(args.time);
	});
	add("%ss", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getSecond(args.time);
	});
	add("%SS", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getSecond(args.time);
	});
	add("%ww", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << !(TimeUtilities::getHour(false, args.time) < 12) ? "pm" : "am";
	});
	add("%WW", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << !(TimeUtilities::getHour(false, args.time) < 12) ? "PM" : "AM";
	});
	add("%qq", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << !(TimeUtilities::getHour(false, args.time) < 12) ? "p" : "a";
	});
	add("%QQ", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << !(TimeUtilities::getHour(false, args.time) < 12) ? "P" : "A";
	});
	add("%zz", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getTimeZone();
	});
	add("%id", [](std::ostringstream &stream, const ReplacementArgs &args) {
		if (args.id.is_initialized()) {
			stream << args.id.get();
		}
	});
	add("%t", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << format(args.logger->getTimeFormat(), args.logType, args.logger, args.time, args.id, args.msg);
	});
	add("%e", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << Logger::getLevelString(args.logType);
	});
	add("%orig", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << Logger::getServerTypeString(args.logger->getServerType());
	});
	add("%msg", [](std::ostringstream &stream, const ReplacementArgs &args) {
		stream << args.msg;
	});
}

void LogReplacements::add(const string &key, func_t func) {
	m_replacementMap.insert(std::make_pair(key, func));
}

string LogReplacements::format(const string &format, LogTypes::LogTypes logType, Logger *logger, time_t time, const opt_string &id, const string &message) {
	string ret = format;
	ReplacementArgs args{logType, logger, time, id, message};

	for (const auto &kvp : singleton->m_replacementMap) {
		size_t x = ret.find(kvp.first);
		if (x != string::npos) {
			std::ostringstream strm;
			kvp.second(strm, args);
			const string &y = strm.str();
			ret.replace(x, kvp.first.size(), y.c_str(), y.size());
		}
	}
	return ret;
}
