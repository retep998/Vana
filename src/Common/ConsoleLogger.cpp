/*
Copyright (C) 2008-2014 Vana Development Team

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
#include <iostream>
#include <iomanip>
#include <sstream>

ConsoleLogger::ConsoleLogger(const string_t &filename, const string_t &format, const string_t &timeFormat, ServerType serverType, size_t bufferSize) :
	Logger(filename, format, timeFormat, serverType, bufferSize)
{
}

auto ConsoleLogger::log(LogType type, const opt_string_t &identifier, const string_t &message) -> void {
	std::cout << Logger::formatLog(getFormat(), type, this, identifier, message) << std::endl;
}