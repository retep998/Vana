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
#include "FileLogger.hpp"
#include "Common/TimeUtilities.hpp"
#ifdef WIN32
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace Vana {

#ifdef WIN32
namespace fs = std::tr2::sys;
#else
namespace fs = boost::filesystem;
#endif

FileLogger::FileLogger(const string_t &filename, const string_t &format, const string_t &timeFormat, ServerType serverType, size_t bufferSize) :
	Logger{filename, format, timeFormat, serverType, bufferSize},
	m_bufferSize{bufferSize},
	m_filenameFormat{filename}
{
	m_buffer.reserve(bufferSize);
}

FileLogger::~FileLogger() {
	flush();
}

auto FileLogger::log(LogType type, const opt_string_t &identifier, const string_t &message) -> void {
	FileLog file;
	file.message = Logger::formatLog(getFormat(), type, this, identifier, message);
	file.file = Logger::formatLog(getFilenameFormat(), type, this, identifier, message);
	m_buffer.push_back(file);
	if (m_buffer.size() >= m_bufferSize) {
		flush();
	}
}

auto FileLogger::flush() -> void {
	for (const auto &bufferedMessage : m_buffer) {
		const string_t &file = bufferedMessage.file;
		fs::path fullPath = fs::system_complete(fs::path{file.substr(0, file.find_last_of("/"))});
		if (!fs::exists(fullPath)) {
			fs::create_directories(fullPath);
		}
		std::fstream f{file, std::ios_base::out};
		f << bufferedMessage.message;
		f.close();
	}
	m_buffer.clear();
}

}