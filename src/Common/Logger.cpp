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
#include "Logger.h"
#include "LogReplacements.h"
#include "TimeUtilities.h"
#include <iostream>
#include <iomanip>
#include <sstream>

string Logger::getLevelString(LogTypes::LogTypes type) {
	string ret;
	switch (type) {
		case LogTypes::Info: ret = "INFO"; break;
		case LogTypes::Warning: ret = "WARNING"; break;
		case LogTypes::Debug: ret = "DEBUG"; break;
		case LogTypes::Error: ret = "ERROR"; break;
		case LogTypes::CriticalError: ret = "CRITICAL ERROR"; break;
		case LogTypes::ServerConnect: ret = "SERVER CONNECT"; break;
		case LogTypes::ServerDisconnect: ret = "SERVER DISCONNECT"; break;
		case LogTypes::ServerAuthFailure: ret = "SERVER AUTH FAILURE"; break;
		case LogTypes::Login: ret = "LOGIN"; break;
		case LogTypes::LoginAuthFailure: ret = "USER AUTH FAILURE"; break;
		case LogTypes::Logout: ret = "LOGOUT"; break;
		case LogTypes::ClientError: ret = "CLIENT ERROR"; break;
		case LogTypes::GmCommand: ret = "GM COMMAND"; break;
		case LogTypes::AdminCommand: ret = "ADMIN COMMAND"; break;
		case LogTypes::BossKill: ret = "BOSS KILL"; break;
		case LogTypes::Trade: ret = "TRADE"; break;
		case LogTypes::ShopTransaction: ret = "SHOP"; break;
		case LogTypes::StorageTransaction: ret = "STORAGE"; break;
		case LogTypes::InstanceBegin: ret = "INSTANCE"; break;
		case LogTypes::Drop: ret = "DROP"; break;
		case LogTypes::Chat: ret = "CHAT"; break;
		case LogTypes::Whisper: ret = "WHISPER"; break;
		case LogTypes::MalformedPacket: ret = "MALFORMED PACKET"; break;
		case LogTypes::ScriptLog: ret = "SCRIPT"; break;
		default: ret = "UNSUPPORTED";
	}
	return ret;
}

string Logger::getServerTypeString(int16_t serverType) {
	string ret;
	switch (serverType) {
		case ServerTypes::Cash: ret = "Cash"; break;
		case ServerTypes::Channel: ret = "Channel"; break;
		case ServerTypes::Login: ret = "Login"; break;
		case ServerTypes::Mts: ret = "MTS"; break;
		case ServerTypes::World: ret = "World"; break;
	}
	return ret;
}

string Logger::getLogFormatted(LogTypes::LogTypes type, Logger *logger, const string &id, const string &message) {
	// This function is gloriously unelegant
	const LogReplacements::map_t &butts = LogReplacements::Instance()->getMap();
	string ret = logger->getFormat();
	time_t start = time(0);
	for (LogReplacements::map_t::const_iterator iter = butts.begin(); iter != butts.end(); ++iter) {
		size_t x = ret.find(iter->first);
		if (x != string::npos) {
			std::stringstream strm;
			int32_t mask = iter->second & Replacements::RemoveFlagMask;
			switch (mask) {
				case Replacements::Time: strm << getTimeFormatted(logger->getTimeFormat()); break;
				case Replacements::Event: strm << getLevelString(type); break;
				case Replacements::Origin: strm << getServerTypeString(logger->getServerType()); break;
				case Replacements::Id: strm << id; break;
				case Replacements::Message: strm << message; break;
			}
			string y = strm.str();
			ret.replace(x, iter->first.size(), y.c_str(), y.size());
		}
	}
	return ret;
}

string Logger::getTimeFormatted(const string &fmt) {
	// This function is gloriously unelegant
	const LogReplacements::map_t &butts = LogReplacements::Instance()->getMap();
	string ret = fmt;
	time_t start = time(0);
	for (LogReplacements::map_t::const_iterator iter = butts.begin(); iter != butts.end(); ++iter) {
		size_t x = ret.find(iter->first);
		if (x != string::npos) {
			std::stringstream strm;
			int32_t mask = iter->second & Replacements::RemoveFlagMask;
			int32_t flags = iter->second & Replacements::GetFlagMask;
			if (!(flags & Replacements::String)) {
				// Integer
				if (flags & Replacements::Long) {
					strm << std::setw(2) << std::setfill('0');
				}
			}
			switch (mask) {
				case Replacements::IntegerDate: strm << TimeUtilities::getDate(start); break;
				case Replacements::StringDate: strm << TimeUtilities::getDayString(!(flags & Replacements::Long), start); break;
				case Replacements::IntegerMonth: strm << TimeUtilities::getMonth(start); break;
				case Replacements::StringMonth: strm << TimeUtilities::getMonthString(!(flags & Replacements::Long), start); break;
				case Replacements::Hour: strm << TimeUtilities::getHour(true, start); break;
				case Replacements::MilitaryHour: strm << TimeUtilities::getHour(false, start); break;
				case Replacements::Minute: strm << TimeUtilities::getMinute(start); break;
				case Replacements::Second: strm << TimeUtilities::getSecond(start); break;
				case Replacements::TimeZone: strm << TimeUtilities::getTimeZone(); break;
				case Replacements::Year: strm << TimeUtilities::getYear(!(flags & Replacements::Long), start); break;
				case Replacements::AmPm: {
					bool pm = !(TimeUtilities::getHour(false, start) < 12);
					if (flags & Replacements::Uppercase) {
						strm << pm ? "P" : "A";
						if (flags & Replacements::Long) {
							strm << "M";
						}
					}
					else {
						strm << pm ? "p" : "a";
						if (flags & Replacements::Long) {
							strm << "m";
						}
					}
					break;
				}
			}
			string y = strm.str();
			ret.replace(x, iter->first.size(), y.c_str(), y.size());
		}
	}
	return ret;
}

Logger::Logger(const string &format, const string &timeFormat, int16_t serverType) :
m_format(format),
m_timeFormat(timeFormat),
m_serverType(serverType)
{
}

Logger::~Logger() {

}

void Logger::log(LogTypes::LogTypes type, const string &identifier, const string &message) {

}
