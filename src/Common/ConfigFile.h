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

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include "ExternalIp.h"
#include "IConfig.h"
#include "Types.h"
#include <string>
#include <vector>

struct DbConfig;
struct LogConfig;

class ConfigFile {
	NONCOPYABLE(ConfigFile);
	NO_DEFAULT_CONSTRUCTOR(ConfigFile);
public:
	ConfigFile(const string_t &filename, bool executeFile = true);
	~ConfigFile();

	auto keyExists(const string_t &value) -> bool;
	auto execute() -> bool;
	auto handleError() -> void;
	auto printError(const string_t &error) -> void;
	auto setVariable(const string_t &name, const string_t &value) -> void;
	auto setVariable(const string_t &name, int32_t value) -> void;
	template <typename TInteger>
	auto get(const string_t &value) -> TInteger;
	template <>
	auto get<bool>(const string_t &value) -> bool;
	template <>
	auto get<string_t>(const string_t &value) -> string_t;

	template <typename TClass>
	auto getClass(const string_t &prefix = "") -> TClass;
	auto getIpMatrix(const string_t &value) -> IpMatrix ;
	auto getBossChannels(const string_t &value, size_t maxChannels) -> vector_t<int8_t>;
private:
	auto keyMustExist(const string_t &value) -> void;
	auto loadFile(const string_t &filename) -> void;

	lua_State *m_luaVm = nullptr;
	string_t m_file;
};

template <typename TInteger>
auto ConfigFile::get(const string_t &value) -> TInteger {
	keyMustExist(value);
	lua_getglobal(m_luaVm, value.c_str());
	TInteger val = static_cast<TInteger>(lua_tointeger(m_luaVm, -1));
	lua_pop(m_luaVm, 1);
	return val;
}

template <>
auto ConfigFile::get<bool>(const string_t &value) -> bool {
	keyMustExist(value);
	lua_getglobal(m_luaVm, value.c_str());
	bool val = lua_toboolean(m_luaVm, -1) != 0;
	lua_pop(m_luaVm, 1);
	return val;
}

template <>
auto ConfigFile::get<string_t>(const string_t &value) -> string_t {
	keyMustExist(value);
	lua_getglobal(m_luaVm, value.c_str());
	string_t x = lua_tostring(m_luaVm, -1);
	lua_pop(m_luaVm, 1);
	return x;
}

template <typename TClass>
auto ConfigFile::getClass(const string_t &prefix) -> TClass {
	TClass obj;
	obj.read(*this, prefix);
	return obj;
}