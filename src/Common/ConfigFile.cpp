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
#include "ConfigFile.h"
#include "Configuration.h"
#include "ExitCodes.h"
#include "FileUtilities.h"
#include <iostream>


ConfigFile::ConfigFile(const string_t &filename, bool executeFile)
{
	loadFile(filename);
	if (executeFile) {
		execute();
	}
}

ConfigFile::~ConfigFile() {
	lua_close(m_luaVm);
}

auto ConfigFile::loadFile(const string_t &filename) -> void {
	if (!FileUtilities::fileExists(filename)) {
		std::cerr << "ERROR: Configuration file " << filename << " does not exist!" << std::endl;
		ExitCodes::exit(ExitCodes::ConfigFileMissing);
	}

	m_file = filename;
	m_luaVm = luaL_newstate();
	luaopen_base(m_luaVm);
}

auto ConfigFile::execute() -> bool {
	if (luaL_dofile(m_luaVm, m_file.c_str())) {
		handleError();
		return false;
	}
	return true;
}

auto ConfigFile::handleError() -> void {
	printError(lua_tostring(m_luaVm, -1));
}

auto ConfigFile::printError(const string_t &error) -> void {
	std::cerr << error << std::endl;
}

auto ConfigFile::keyExists(const string_t &value) -> bool {
	lua_getglobal(m_luaVm, value.c_str());
	bool ret = !lua_isnil(m_luaVm, -1);
	lua_pop(m_luaVm, 1);
	return ret;
}

auto ConfigFile::keyMustExist(const string_t &value) -> void {
	if (!keyExists(value)) {
		std::cerr << "ERROR: Couldn't get a value from config file." << std::endl;
		std::cerr << "File: " << m_file << std::endl;
		std::cerr << "Value: " << value << std::endl;
		ExitCodes::exit(ExitCodes::ConfigError);
	}
}

auto ConfigFile::setVariable(const string_t &name, const string_t &value) -> void {
	lua_pushstring(m_luaVm, value.c_str());
	lua_setglobal(m_luaVm, name.c_str());
}

auto ConfigFile::setVariable(const string_t &name, int32_t value) -> void {
	lua_pushinteger(m_luaVm, value);
	lua_setglobal(m_luaVm, name.c_str());
}

auto ConfigFile::getIpMatrix(const string_t &value) -> IpMatrix {
	keyMustExist(value);
	IpMatrix matrix;

	lua_getglobal(m_luaVm, value.c_str());
	lua_pushnil(m_luaVm);
	while (lua_next(m_luaVm, -2)) {
		vector_t<uint32_t> arr;
		arr.reserve(2);

		lua_pushnil(m_luaVm);
		while (lua_next(m_luaVm, -2)) {
			arr.push_back(Ip::stringToIpv4(lua_tostring(m_luaVm, -1)));
			lua_pop(m_luaVm, 1);
		}

		if (arr.size() != 2) {
			std::cerr << "ERROR: " << value << " configuration is malformed!" << std::endl;
			ExitCodes::exit(ExitCodes::ConfigError);
		}

		matrix.push_back(ExternalIp(arr[0], arr[1]));

		lua_pop(m_luaVm, 1);
	}
	lua_pop(m_luaVm, 1);

	return matrix;
}

auto ConfigFile::getBossChannels(const string_t &value, size_t maxChannels) -> vector_t<int8_t> {
	keyMustExist(value);
	vector_t<int8_t> channels;

	lua_getglobal(m_luaVm, value.c_str());
	lua_pushnil(m_luaVm);
	while (lua_next(m_luaVm, -2)) {
		channels.push_back(static_cast<int8_t>(lua_tointeger(m_luaVm, -1)));
		lua_pop(m_luaVm, 1);
	}
	lua_pop(m_luaVm, 1);

	if (channels.size() == 1 && channels[0] == -1) {
		channels.clear();
		for (size_t i = 1; i <= maxChannels; i++) {
			channels.push_back(static_cast<int8_t>(i));
		}
	}
	return channels;
}