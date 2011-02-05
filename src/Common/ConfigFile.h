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

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include "Ip.h"
#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

struct LogConfig;

class ConfigFile {
public:
	ConfigFile(const string &filename, bool executeFile = true);
	ConfigFile();
	~ConfigFile();
	void loadFile(const string &filename);
	bool keyExists(const string &value);
	bool execute();
	void handleError();
	void printError(const string &error);
	void setVariable(const string &name, const string &value);
	void setVariable(const string &name, int32_t value);
	int32_t getInt(const string &value);
	int16_t getShort(const string &value);
	uint16_t getUnsignedShort(const string &value);
	string getString(const string &value);
	IpMatrix getIpMatrix(const string &value);
	vector<int8_t> getBossChannels(const string &value, size_t maxChannels);
	bool getBool(const string &value);
	LogConfig getLogConfig(const string &server);
private:
	lua_State * getLuaState() const { return m_luaVm; }
	lua_State *m_luaVm;
	string m_file;
};
