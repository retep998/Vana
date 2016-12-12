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
#include "LuaEnvironment.hpp"
#include "FileUtilities.hpp"
#include "StringUtilities.hpp"
#include <iostream>
#include <stdexcept>

ObjectPool<int32_t, LuaEnvironment *> LuaEnvironment::s_environments = ObjectPool<int32_t, LuaEnvironment *>(1, 1000000);

auto LuaEnvironment::getEnvironment(lua_State *luaVm) -> LuaEnvironment & {
	lua_getglobal(luaVm, "system_environment_id");
	int32_t id = lua_tointeger(luaVm, -1);
	lua_pop(luaVm, 1);
	auto kvp = s_environments.find(id);
	return *kvp->second;
}

LuaEnvironment::LuaEnvironment(const string_t &filename)
{
	loadFile(filename);

	m_environmentIdentifier = s_environments.store(this);
	set<int32_t>("system_environment_id", m_environmentIdentifier);
}

LuaEnvironment::LuaEnvironment(const string_t &filename, bool useThread)
{
	loadFile(filename);

	m_environmentIdentifier = s_environments.store(this);
	if (useThread) {
		m_luaThread = lua_newthread(m_luaVm);
	}
	set<int32_t>("system_environment_id", m_environmentIdentifier);
}

LuaEnvironment::~LuaEnvironment() {
	s_environments.release(m_environmentIdentifier);
	lua_close(m_luaVm);
	m_luaVm = nullptr;
}

auto LuaEnvironment::loadFile(const string_t &filename) -> void {
	if (m_luaVm != nullptr) {
		throw std::runtime_error("LuaVM was still specified");
	}
	if (!FileUtilities::fileExists(filename)) {
		handleFileNotFound(filename);
	}

	m_file = filename;
	m_luaVm = luaL_newstate();

	requireStandardLib("base", luaopen_base);
	requireStandardLib("string", luaopen_string);
	requireStandardLib("math", luaopen_math);
	requireStandardLib("table", luaopen_table);
}

auto LuaEnvironment::run() -> Result {
	if (m_luaThread == nullptr) {
		if (luaL_dofile(m_luaVm, m_file.c_str())) {
			handleError(m_file, get<string_t>(m_luaVm, -1));
			pop();
			return Result::Failure;
		}
	}
	else {
		if (luaL_loadfile(m_luaThread, m_file.c_str())) {
			handleError(m_file, get<string_t>(m_luaThread, -1));
			pop();
			return Result::Failure;
		}
		return resume(0);
	}
	return Result::Successful;
}

auto LuaEnvironment::resume(lua_return_t pushedArgCount) -> Result {
	int ret = lua_resume(m_luaThread, m_luaVm, pushedArgCount);
	if (ret == 0) {
		handleThreadCompletion();
	}
	else if (ret != LUA_YIELD) {
		// Error, a working script returns either 0 or LUA_YIELD
		string_t error = lua_tostring(m_luaThread, -1);
		handleError(m_file, error);
		return Result::Failure;
	}
	return Result::Successful;
}

auto LuaEnvironment::handleError(const string_t &filename, const string_t &error) -> void {
	printError(error);
}

auto LuaEnvironment::handleFileNotFound(const string_t &filename) -> void {
	// Intentionally blank
}

auto LuaEnvironment::handleThreadCompletion() -> void {
	// Intentionally blank
}

auto LuaEnvironment::handleKeyNotFound(const string_t &filename, const string_t &key) -> void {
	throw std::runtime_error("Key '" + key + "' does not exist and is required in file '" + filename + "'");
}

auto LuaEnvironment::printError(const string_t &error) const -> void {
	std::cerr << error << std::endl;
}

auto LuaEnvironment::expose(const string_t &name, lua_function_t func) -> void {
	lua_register(m_luaVm, name.c_str(), func);
}

auto LuaEnvironment::requireStandardLib(const string_t &localName, lua_function_t func) -> void {
	luaL_requiref(m_luaVm, localName.c_str(), func, 1);
}

auto LuaEnvironment::yield(lua_return_t numberOfReturnResultsPassedToResume) -> lua_return_t {
	return lua_yield(m_luaThread, numberOfReturnResultsPassedToResume);
}

auto LuaEnvironment::pushNil() -> LuaEnvironment & {
	return pushNil(m_luaVm);
}

auto LuaEnvironment::pushNil(lua_State *luaVm) -> LuaEnvironment & {
	lua_pushnil(luaVm);
	return *this;
}

auto LuaEnvironment::getScriptName() -> string_t {
	vector_t<string_t> parts = StringUtilities::split(m_file, "/");
	return FileUtilities::removeExtension(parts[parts.size() - 1]);
}

auto LuaEnvironment::getScriptPath() -> vector_t<string_t> {
	vector_t<string_t> parts = StringUtilities::split(m_file, "/");
	if (parts.size() == 1) {
		vector_t<string_t> ret;
		return ret;
	}
	vector_t<string_t> ret{parts.begin(), parts.end() - 1};
	return ret;
}

auto LuaEnvironment::exists(const string_t &key) -> bool {
	return exists(m_luaVm, key);
}

auto LuaEnvironment::exists(lua_State *luaVm, const string_t &key) -> bool {
	lua_getglobal(luaVm, key.c_str());
	bool ret = !lua_isnil(luaVm, -1);
	lua_pop(luaVm, 1);
	return ret;
}

auto LuaEnvironment::keyMustExist(const string_t &key) -> void {
	if (!exists(key)) {
		handleKeyNotFound(m_file, key);
	}
}

auto LuaEnvironment::error(const string_t &text) -> void {
	handleError(m_file, text);
}

auto LuaEnvironment::is(const string_t &value, LuaType type) -> bool {
	return is(m_luaVm, value, type);
}

auto LuaEnvironment::is(lua_State *luaVm, const string_t &value, LuaType type) -> bool {
	lua_getglobal(luaVm, value.c_str());
	bool ret = is(luaVm, -1, type);
	lua_pop(luaVm, 1);
	return ret;
}

auto LuaEnvironment::pop(int count) -> void {
	pop(m_luaVm, count);
}

auto LuaEnvironment::pop(lua_State *luaVm, int count) -> void {
	lua_pop(luaVm, count);
}

auto LuaEnvironment::count() -> int {
	return count(m_luaVm);
}

auto LuaEnvironment::count(lua_State *luaVm) -> int {
	return lua_gettop(luaVm);
}

auto LuaEnvironment::is(int index, LuaType type) -> bool {
	return is(m_luaVm, index, type);
}

auto LuaEnvironment::is(lua_State *luaVm, int index, LuaType type) -> bool {
	return lua_type(luaVm, index) == static_cast<int>(type);
}

auto LuaEnvironment::typeOf(int index) -> LuaType {
	return static_cast<LuaType>(lua_type(m_luaVm, index));
}

auto LuaEnvironment::typeOf(lua_State *luaVm, int index) -> LuaType {
	return static_cast<LuaType>(lua_type(luaVm, index));
}