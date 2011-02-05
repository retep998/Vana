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
#include "ConfigFile.h"
#include "Configuration.h"
#include "ExitCodes.h"
#include "FileUtilities.h"
#include "IpUtilities.h"
#include <iostream>

ConfigFile::ConfigFile(const string &filename, bool executeFile) {
	loadFile(filename);
	if (executeFile) {
		execute();
	}
}

ConfigFile::ConfigFile() { }

ConfigFile::~ConfigFile() {
	lua_close(getLuaState());
}

void ConfigFile::loadFile(const string &filename) {
	if (!FileUtilities::fileExists(filename)) {
		std::cerr << "ERROR: Configuration file " << filename << " does not exist!" << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::ConfigFileMissing);
	}

	m_file = filename;
	m_luaVm = lua_open();
	luaopen_base(getLuaState());
}

bool ConfigFile::execute() {
	if (luaL_dofile(getLuaState(), m_file.c_str())) {
		handleError();
		return false;
	}
	return true;
}

void ConfigFile::handleError() {
	printError(lua_tostring(getLuaState(), -1));
}

void ConfigFile::printError(const string &error) {
	std::cout << error << std::endl;
}

bool ConfigFile::keyExists(const string &value) {
	lua_getglobal(getLuaState(), value.c_str());
	bool ret = !lua_isnil(getLuaState(), -1);
	lua_pop(getLuaState(), 1);
	return ret;
}

void ConfigFile::setVariable(const string &name, const string &value) {
	lua_pushstring(getLuaState(), value.c_str());
	lua_setglobal(getLuaState(), name.c_str());
}

void ConfigFile::setVariable(const string &name, int32_t value) {
	lua_pushinteger(getLuaState(), value);
	lua_setglobal(getLuaState(), name.c_str());
}

int32_t ConfigFile::getInt(const string &value) {
	if (!keyExists(value)) {
		std::cout << "ERROR: Couldn't get an integer from config file."<< std::endl;
		std::cout << "File: " << m_file << std::endl;
		std::cout << "Value: " << value << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::ConfigError);
	}
	else {
		lua_getglobal(getLuaState(), value.c_str());
		int32_t val = lua_tointeger(getLuaState(), -1);
		lua_pop(getLuaState(), 1);
		return val;
	}
}

int16_t ConfigFile::getShort(const string &value) {
	return static_cast<int16_t>(getInt(value));
}

uint16_t ConfigFile::getUnsignedShort(const string &value) {
	// For ports, as their range lies between 0 and 65,535 and not between -32,768 and 32,767.
	return static_cast<uint16_t>(getInt(value));
}

string ConfigFile::getString(const string &value) {
	if (!keyExists(value)) {
		std::cout << "ERROR: Couldn't get a string from config file." << std::endl;
		std::cout << "File: " << m_file << std::endl;
		std::cout << "Value: " << value << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::ConfigError);
	}
	else {
		lua_getglobal(getLuaState(), value.c_str());
		string x = lua_tostring(getLuaState(), -1);
		lua_pop(getLuaState(), 1);
		return x;
	}
}

IpMatrix ConfigFile::getIpMatrix(const string &value) {
	IpMatrix matrix;

	lua_getglobal(getLuaState(), value.c_str());
	lua_pushnil(getLuaState());
	while (lua_next(getLuaState(), -2)) {
		IpArray arr;
		arr.reserve(2);

		lua_pushnil(getLuaState());
		while (lua_next(getLuaState(), -2)) {
			arr.push_back(IpUtilities::stringToIp(lua_tostring(getLuaState(), -1)));
			lua_pop(getLuaState(), 1);
		}

		if (arr.size() != 2) {
			std::cerr << "ERROR: external_ip configuration is malformed!" << std::endl;
			std::cout << "Press enter to quit ...";
			getchar();
			exit(ExitCodes::ConfigError);
		}

		matrix.push_back(arr);

		lua_pop(getLuaState(), 1);
	}

	lua_pop(getLuaState(), 1);

	return matrix;
}

vector<int8_t> ConfigFile::getBossChannels(const string &value, size_t maxChannels) {
	vector<int8_t> channels;

	lua_getglobal(getLuaState(), value.c_str());
	lua_pushnil(getLuaState());
	while (lua_next(getLuaState(), -2)) {
		channels.push_back(lua_tointeger(getLuaState(), -1));
		lua_pop(getLuaState(), 1);
	}

	lua_pop(getLuaState(), 1);

	if (channels.size() == 1 && channels[0] == -1) {
		channels.clear();
		for (size_t i = 1; i <= maxChannels; i++) {
			channels.push_back(static_cast<int8_t>(i));
		}
	}
	return channels;
}

bool ConfigFile::getBool(const string &value) {
	if (!keyExists(value)) {
		std::cout << "ERROR: Couldn't get a boolean from config file." << std::endl;
		std::cout << "File: " << m_file << std::endl;
		std::cout << "Value: " << value << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::ConfigError);
	}
	else {
		lua_getglobal(getLuaState(), value.c_str());
		bool ret = (lua_toboolean(getLuaState(), -1) != 0);
		lua_pop(getLuaState(), 1);
		return ret;
	}
}

LogConfig ConfigFile::getLogConfig(const string &server) {
	LogConfig x;
	string t = server + "_log_";
	x.destination = getInt(t + "destination");
	x.bufferSize = getInt(t + "buffer_size");
	x.format = getString(t + "format");
	x.file = getString(t + "file");
	x.timeFormat = getString("log_time_format");
	return x;
}