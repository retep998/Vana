/*
Copyright (C) 2008-2012 Vana Development Team

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
#include <memory>
#include <string>

using std::string;
using std::unique_ptr;

template<typename Logger1, typename Logger2>
class DualLogger : public Logger {
public:
	DualLogger(const string &filename, const string &format, const string &timeFormat, int16_t serverType, size_t bufferSize = 10) {
		m_logger1.reset(new Logger1(filename, format, timeFormat, serverType, bufferSize));
		m_logger2.reset(new Logger2(filename, format, timeFormat, serverType, bufferSize));
	}

	void log(LogTypes::LogTypes type, const string &identifier, const string &message) {
		getLogger1()->log(type, identifier, message);
		getLogger2()->log(type, identifier, message);
	}
private:
	Logger1 * getLogger1() const { return m_logger1.get(); }
	Logger2 * getLogger2() const { return m_logger2.get(); }
	unique_ptr<Logger1> m_logger1;
	unique_ptr<Logger2> m_logger2;
};

template<typename Logger1, typename Logger2, typename Logger3>
class TriLogger : public Logger {
public:
	TriLogger(const string &filename, const string &format, const string &timeFormat, int16_t serverType, size_t bufferSize = 10) {
		m_logger1.reset(new Logger1(filename, format, timeFormat, serverType, bufferSize));
		m_logger2.reset(new Logger2(filename, format, timeFormat, serverType, bufferSize));
		m_logger3.reset(new Logger3(filename, format, timeFormat, serverType, bufferSize));
	}

	void log(LogTypes::LogTypes type, const string &identifier, const string &message) {
		getLogger1()->log(type, identifier, message);
		getLogger2()->log(type, identifier, message);
		getLogger3()->log(type, identifier, message);
	}
private:
	Logger1 * getLogger1() const { return m_logger1.get(); }
	Logger2 * getLogger2() const { return m_logger2.get(); }
	Logger3 * getLogger3() const { return m_logger3.get(); }
	unique_ptr<Logger1> m_logger1;
	unique_ptr<Logger2> m_logger2;
	unique_ptr<Logger3> m_logger3;
};