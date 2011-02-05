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
#include "FileLogger.h"
#include "LogReplacements.h"
#include "TimeUtilities.h"
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

string FileLogger::prepareFileName(LogTypes::LogTypes type, FileLogger *logger, time_t start, const string &id, const string &message) {
	// This function is gloriously unelegant
	const LogReplacements::map_t &butts = LogReplacements::Instance()->getMap();
	string ret = logger->getFilenameFormat();
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
				case Replacements::Time: strm << getTimeFormatted(logger->getTimeFormat()); break;
				case Replacements::Event: strm << getLevelString(type); break;
				case Replacements::Origin: strm << getServerTypeString(logger->getServerType()); break;
				case Replacements::Id: strm << id; break;
				case Replacements::Message: strm << message; break;
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

FileLogger::FileLogger(const string &filename, const string &format, const string &timeFormat, int16_t serverType, size_t bufferSize) :
Logger(format, timeFormat, serverType),
m_bufferSize(bufferSize),
m_filenameFormat(filename)
{
	m_buffer.reserve(bufferSize);
}

FileLogger::~FileLogger() {
	flush();
}

void FileLogger::log(LogTypes::LogTypes type, const string &identifier, const string &message) {
	FileLog file;
	file.message = Logger::getLogFormatted(type, this, identifier, message);
	file.file = FileLogger::prepareFileName(type, this, time(0), identifier, message);
	m_buffer.push_back(file);
	if (m_buffer.size() >= m_bufferSize) {
		flush();
	}
}

void FileLogger::flush() {
	for (vector<FileLog>::const_iterator iter = m_buffer.begin(); iter != m_buffer.end(); ++iter) {
		const string &file = iter->file;
		fs::path fullPath = fs::system_complete(fs::path(file.substr(0, file.find_last_of("/")), fs::native));
		if (!fs::exists(fullPath)) {
			fs::create_directories(fullPath);
		}
		std::fstream f(file.c_str(), std::ios_base::out);
		f << iter->message;
		f.close();
	}
	m_buffer.clear();
}
