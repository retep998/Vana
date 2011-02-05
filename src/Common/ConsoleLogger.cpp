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
#include "ConsoleLogger.h"
#include <iostream>
#include <sstream>

ConsoleLogger::ConsoleLogger(const string &format, const string &timeFormat, int16_t serverType) :
Logger(format, timeFormat, serverType)
{
}

void ConsoleLogger::log(LogTypes::LogTypes type, const string &identifier, const string &message) {
	std::cout << Logger::getLogFormatted(type, this, identifier, message) << std::endl;
}
