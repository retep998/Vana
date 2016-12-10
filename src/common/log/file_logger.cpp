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
#include "file_logger.hpp"
#include "common/util/time.hpp"
#ifdef WIN32
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace vana {
namespace log {

#ifdef WIN32
namespace fs = std::tr2::sys;
#else
namespace fs = boost::filesystem;
#endif

file_logger::file_logger(const string &filename, const string &format, const string &time_format, server_type type, size_t buffer_size) :
	base_logger{filename, format, time_format, type, buffer_size},
	m_buffer_size{buffer_size},
	m_filename_format{filename}
{
	m_buffer.reserve(buffer_size);
}

file_logger::~file_logger() {
	flush();
}

auto file_logger::log(vana::log::type type, const opt_string &identifier, const string &message) -> void {
	file_log file;
	file.message = base_logger::format_log(get_format(), type, this, identifier, message);
	file.file = base_logger::format_log(get_filename_format(), type, this, identifier, message);
	m_buffer.push_back(file);
	if (m_buffer.size() >= m_buffer_size) {
		flush();
	}
}

auto file_logger::flush() -> void {
	for (const auto &buffered_message : m_buffer) {
		const string &file = buffered_message.file;
		fs::path fullPath = fs::system_complete(fs::path{file.substr(0, file.find_last_of("/"))});
		if (!fs::exists(fullPath)) {
			fs::create_directories(fullPath);
		}
		std::fstream f{file, std::ios_base::out};
		f << buffered_message.message;
		f.close();
	}
	m_buffer.clear();
}

}
}