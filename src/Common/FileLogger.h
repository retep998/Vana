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
#pragma once

#include "Logger.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

struct FileLog {
	string message;
	string file;
};

class FileLogger : public Logger {
public:
	FileLogger(const string &filename, const string &format, const string &timeFormat, int16_t serverType, size_t bufferSize = 10);
	~FileLogger();

	void log(LogTypes::LogTypes type, const string &identifier, const string &message);
	void flush();
	const string & getFilenameFormat() const { return m_filenameFormat; }
private:
	static string prepareFileName(LogTypes::LogTypes type, FileLogger *logger, time_t start, const string &id, const string &message);
	string m_filenameFormat;
	size_t m_bufferSize;
	vector<FileLog> m_buffer;
};