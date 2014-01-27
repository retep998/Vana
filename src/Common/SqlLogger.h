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
#pragma once

#include "Logger.h"
#include <string>
#include <vector>

class SqlLogger : public Logger {
public:
	SqlLogger(const string_t &filename, const string_t &format, const string_t &timeFormat, int16_t serverType, size_t bufferSize = 10);
	~SqlLogger();

	auto log(LogTypes::LogTypes type, const opt_string_t &identifier, const string_t &message) -> void override;
	auto flush() -> void;
private:
	size_t m_bufferSize;
	vector_t<LogMessage> m_buffer;
};