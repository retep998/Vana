/*
Copyright (C) 2008 Vana Development Team

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
#ifndef CONFIGFILE_H
#define CONFIGFILE_H

extern "C" {
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
}

#include "Types.h"
#include <string>

using std::string;

class ConfigFile {
public:
	ConfigFile(string filename);
	ConfigFile();
	void loadFile(string filename);
	bool keyExist(string value);
	int32_t getInt(string value);
	int16_t getShort(string value);
	string getString(string value);
	bool getBool(string value);
private:
	lua_State *luaVm;
	void initialize();
};

#endif
