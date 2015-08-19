/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Logger.hpp"
#include "ServerType.hpp"
#include "TimeUtilities.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <sstream>

Logger::Logger(const string_t &filename, const string_t &format, const string_t &timeFormat, ServerType serverType, size_t bufferSize) :
	m_format{format},
	m_timeFormat{timeFormat},
	m_serverType{serverType}
{
}

auto Logger::formatLog(const string_t &format, LogType type, Logger *logger, const opt_string_t &id, const string_t &message) -> string_t {
	return formatLog(format, type, logger, time(nullptr), id, message);
}

auto Logger::formatLog(const string_t &format, LogType type, Logger *logger, time_t time, const opt_string_t &id, const string_t &message) -> string_t {
	string_t ret = format;
	ReplacementArgs args{type, logger, time, id, message};
	static LogReplacements replacements;

	for (const auto &kvp : replacements.m_replacementMap) {
		size_t x = ret.find(kvp.first);
		if (x != string_t::npos) {
			out_stream_t strm;
			kvp.second(strm, args);
			const string_t &y = strm.str();
			ret.replace(x, kvp.first.size(), y.c_str(), y.size());
		}
	}
	return ret;
}

Logger::ReplacementArgs::ReplacementArgs(LogType logType, Logger *logger, time_t time, const opt_string_t &id, const string_t &msg) :
	logType{logType},
	logger{logger},
	time{time},
	id{id},
	msg{msg}
{
}

Logger::LogReplacements::LogReplacements() {
	add("%yy", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getYear(true, args.time);
	});
	add("%YY", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getYear(false, args.time);
	});
	add("%mm", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getMonth(args.time);
	});
	add("%MM", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getMonth(args.time);
	});
	add("%oo", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getMonthString(true, args.time);
	});
	add("%OO", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getMonthString(false, args.time);
	});
	add("%dd", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getDate(args.time);
	});
	add("%DD", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getDate(args.time);
	});
	add("%aa", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getDayString(true, args.time);
	});
	add("%AA", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getDayString(false, args.time);
	});
	add("%hh", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getHour(true, args.time);
	});
	add("%HH", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getHour(true, args.time);
	});
	add("%mi", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getHour(false, args.time);
	});
	add("%MI", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getHour(false, args.time);
	});
	add("%ii", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getMinute(args.time);
	});
	add("%II", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getMinute(args.time);
	});
	add("%ss", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getSecond(args.time);
	});
	add("%SS", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << std::setw(2) << std::setfill('0') << TimeUtilities::getSecond(args.time);
	});
	add("%ww", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << !(TimeUtilities::getHour(false, args.time) < 12) ? "pm" : "am";
	});
	add("%WW", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << !(TimeUtilities::getHour(false, args.time) < 12) ? "PM" : "AM";
	});
	add("%qq", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << !(TimeUtilities::getHour(false, args.time) < 12) ? "p" : "a";
	});
	add("%QQ", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << !(TimeUtilities::getHour(false, args.time) < 12) ? "P" : "A";
	});
	add("%zz", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << TimeUtilities::getTimeZone();
	});
	add("%id", [](out_stream_t &stream, const ReplacementArgs &args) {
		if (args.id.is_initialized()) {
			stream << args.id.get();
		}
	});
	add("%t", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << Logger::formatLog(args.logger->getTimeFormat(), args.logType, args.logger, args.time, args.id, args.msg);
	});
	add("%e", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << getLevelString(args.logType);
	});
	add("%orig", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << getServerTypeString(args.logger->getServerType());
	});
	add("%msg", [](out_stream_t &stream, const ReplacementArgs &args) {
		stream << args.msg;
	});
}

auto Logger::LogReplacements::add(const string_t &key, func_t func) -> void {
	m_replacementMap.emplace(key, func);
}

auto Logger::LogReplacements::getLevelString(LogType type) -> string_t {
	switch (type) {
		case LogType::Info: return "INFO";
		case LogType::Warning: return "WARNING";
		case LogType::Debug: return "DEBUG";
		case LogType::Error: return "ERROR";
		case LogType::DebugError: return "DEBUG ERROR";
		case LogType::CriticalError: return "CRITICAL ERROR";
		case LogType::Hacking: return "HACKING";
		case LogType::ServerConnect: return "SERVER CONNECT";
		case LogType::ServerDisconnect: return "SERVER DISCONNECT";
		case LogType::ServerAuthFailure: return "SERVER AUTH FAILURE";
		case LogType::Login: return "LOGIN";
		case LogType::LoginAuthFailure: return "USER AUTH FAILURE";
		case LogType::Logout: return "LOGOUT";
		case LogType::ClientError: return "CLIENT ERROR";
		case LogType::GmCommand: return "GM COMMAND";
		case LogType::AdminCommand: return "ADMIN COMMAND";
		case LogType::BossKill: return "BOSS KILL";
		case LogType::Trade: return "TRADE";
		case LogType::ShopTransaction: return "SHOP";
		case LogType::StorageTransaction: return "STORAGE";
		case LogType::InstanceBegin: return "INSTANCE";
		case LogType::Drop: return "DROP";
		case LogType::Chat: return "CHAT";
		case LogType::Whisper: return "WHISPER";
		case LogType::MalformedPacket: return "MALFORMED PACKET";
		case LogType::ScriptLog: return "SCRIPT";
	}
	throw NotImplementedException{"LogType"};
}

auto Logger::LogReplacements::getServerTypeString(ServerType type) -> string_t {
	switch (type) {
		case ServerType::Cash: return "Cash";
		case ServerType::Channel: return "Channel";
		case ServerType::Login: return "Login";
		case ServerType::Mts: return "MTS";
		case ServerType::World: return "World";
	}
	throw NotImplementedException{"ServerType"};
}