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
#include "ConsoleLogger.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace vana {

console_logger::console_logger(const string &filename, const string &format, const string &time_format, server_type type, size_t buffer_size) :
	base_logger{filename, format, time_format, type, buffer_size}
{
}

auto console_logger::log(log_type type, const opt_string &identifier, const string &message) -> void {
	switch (type) {
		case log_type::critical_error:
		case log_type::debug_error:
		case log_type::error:
		case log_type::server_auth_failure:
		case log_type::warning:
		case log_type::malformed_packet:
			std::cerr << base_logger::format_log(get_format(), type, this, identifier, message) << std::endl;
			break;
		default:
			std::cout << base_logger::format_log(get_format(), type, this, identifier, message) << std::endl;
			break;
	}
}

}