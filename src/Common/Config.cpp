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
#include "Config.h"
#include <cstring>

Config::Config(char *filename) {
	loadFile(filename);
}

Config::Config() { }

void Config::loadFile(char *filename) {
	luaVm = lua_open();
	luaopen_base(luaVm);

	luaL_dofile(luaVm, filename);
}

bool Config::keyExist(char *value) {
	lua_getglobal(luaVm, value);
	return !lua_isnil(luaVm, -1);
}

int Config::getInt(char *value) {
	lua_getglobal(luaVm, value);
	return lua_tointeger(luaVm, -1);
}

const char * Config::getString(char *value) {
	lua_getglobal(luaVm, value);
	return lua_tostring(luaVm, -1);
}

bool Config::getBool(char *value) {
	lua_getglobal(luaVm, value);
	return (lua_toboolean(luaVm, -1) != 0);
}
