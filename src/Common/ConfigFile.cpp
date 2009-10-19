/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "FileUtilities.h"
#include "IpUtilities.h"
#include <iostream>

ConfigFile::ConfigFile(const string &filename) {
	loadFile(filename);
}

ConfigFile::ConfigFile() { }

void ConfigFile::loadFile(const string &filename) {
	// Check for file existence first
	if (!FileUtilities::fileExists(filename)) {
		std::cerr << "ERROR: Configuration file " << filename << " does not exist!" << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(1);
	}

	luaVm = lua_open();
	luaopen_base(luaVm);

	luaL_dofile(luaVm, filename.c_str());
}

bool ConfigFile::keyExist(const string &value) {
	lua_getglobal(luaVm, value.c_str());
	return !lua_isnil(luaVm, -1);
}

int32_t ConfigFile::getInt(const string &value) {
	lua_getglobal(luaVm, value.c_str());
	return lua_tointeger(luaVm, -1);
}

int16_t ConfigFile::getShort(const string &value) {
	return static_cast<int16_t>(getInt(value));
}

string ConfigFile::getString(const string &value) {
	lua_getglobal(luaVm, value.c_str());
	return string(lua_tostring(luaVm, -1));
}

IpMatrix ConfigFile::getIpMatrix(const string &value) {
	IpMatrix matrix;
	
	lua_getglobal(luaVm, value.c_str());
	lua_pushnil(luaVm);
	while (lua_next(luaVm, -2)) {
		IpArray arr;
		arr.reserve(2);

		lua_pushnil(luaVm);
		while (lua_next(luaVm, -2)) {
			arr.push_back(IpUtilities::stringToIp(lua_tostring(luaVm, -1)));
			lua_pop(luaVm, 1);
		}

		if (arr.size() != 2) {
			std::cerr << "ERROR: external_ip configuration is malformed!" << std::endl;
			std::cout << "Press enter to quit ...";
			getchar();
			exit(1);
		}

		matrix.push_back(arr);

		lua_pop(luaVm, 1);
	}

	return matrix;
}

vector<int8_t> ConfigFile::getBossChannels(const string &value, size_t maxChannels) {
	vector<int8_t> channels;

	lua_getglobal(luaVm, value.c_str());
	lua_pushnil(luaVm);
	while (lua_next(luaVm, -2)) {
		channels.push_back(lua_tointeger(luaVm, -1));
		lua_pop(luaVm, 1);
	}

	if (channels.size() == 1 && channels[0] == -1) {
		channels.clear();
		for (size_t i = 1; i <= maxChannels; i++) {
			channels.push_back(static_cast<int8_t>(i));
		}
	}
	return channels;
}

bool ConfigFile::getBool(const string &value) {
	lua_getglobal(luaVm, value.c_str());
	return (lua_toboolean(luaVm, -1) != 0);
}
