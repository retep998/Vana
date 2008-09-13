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
#include "ConfigFile.h"
#include <iostream>
#include <sys/stat.h>

ConfigFile::ConfigFile(string filename) {
	loadFile(filename);
}

ConfigFile::ConfigFile() { }

void ConfigFile::loadFile(string filename) {
	// Check for file existance first
	struct stat fileInfo;
	if (stat(filename.c_str(), &fileInfo)) {
		std::cerr << "ERROR: Configuration file " << filename << " does not exist!" << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(1);
	}

	luaVm = lua_open();
	luaopen_base(luaVm);

	luaL_dofile(luaVm, filename.c_str());
}

bool ConfigFile::keyExist(string value) {
	lua_getglobal(luaVm, value.c_str());
	return !lua_isnil(luaVm, -1);
}

int ConfigFile::getInt(string value) {
	lua_getglobal(luaVm, value.c_str());
	return lua_tointeger(luaVm, -1);
}

string ConfigFile::getString(string value) {
	lua_getglobal(luaVm, value.c_str());
	return string(lua_tostring(luaVm, -1));
}

bool ConfigFile::getBool(string value) {
	lua_getglobal(luaVm, value.c_str());
	return (lua_toboolean(luaVm, -1) != 0);
}
