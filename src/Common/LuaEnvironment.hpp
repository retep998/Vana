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

#include "LoopingId.hpp"
#include "Types.hpp"
#include <stdexcept>
#include <string>
#include <vector>

enum class LuaType : int {
	None = LUA_TNONE,
	Nil = LUA_TNIL,
	Bool = LUA_TBOOLEAN,
	LightUserData = LUA_TLIGHTUSERDATA,
	Number = LUA_TNUMBER,
	String = LUA_TSTRING,
	Table = LUA_TTABLE,
	Function = LUA_TFUNCTION,
	UserData = LUA_TUSERDATA,
	Thread = LUA_TTHREAD,
};

using lua_function_t = int (*)(lua_State *);

class LuaEnvironment;

template <typename T>
struct LuaSerialize {
	auto read(LuaEnvironment &config, const string_t &prefix) -> T {
		static_assert(false, "T is not appropriately specialized for that type");
		throw std::logic_error("T is not appropriately specialized for that type");
	};
};

// TODO FIXME lua
// Lua 5.2 doesn't support 64-bit integers out of the box, it needs special compilation and #defines to make it right
// Lua 5.3 tentatively looks like 64-bit integer will be the default integer size, so at that point, I can update this
class LuaEnvironment {
	NONCOPYABLE(LuaEnvironment);
	NO_DEFAULT_CONSTRUCTOR(LuaEnvironment);
public:
	LuaEnvironment(const string_t &filename);
	~LuaEnvironment();

	static auto getEnvironment(lua_State *luaVm) -> LuaEnvironment &;

	auto run() -> Result;
	auto error(const string_t &text) -> void;

	auto exists(const string_t &key) -> bool;
	auto exists(lua_State *luaVm, const string_t &key) -> bool;
	auto is(const string_t &value, LuaType type) -> bool;
	auto is(lua_State *luaVm, const string_t &value, LuaType type) -> bool;
	auto is(int index, LuaType type) -> bool;
	auto is(lua_State *luaVm, int index, LuaType type) -> bool;
	auto typeOf(int index) -> LuaType;
	auto typeOf(lua_State *luaVm, int index) -> LuaType;
	auto pop(int count = 1) -> void;
	auto pop(lua_State *luaVm, int count = 1) -> void;
	auto count() -> int;
	auto count(lua_State *luaVm) -> int;

	auto yield(int numberOfReturnResultsPassedToResume) -> int;
	auto pushNil() -> LuaEnvironment &;
	auto pushNil(lua_State *luaVm) -> LuaEnvironment &;

	template <typename T>
	auto set(const string_t &key, const T &value) -> LuaEnvironment &;
	template <typename T>
	auto push(const T &value) -> LuaEnvironment &;
	template <typename T>
	auto get(const string_t &key) -> T;
	template <typename T>
	auto get(int index) -> T;
	template <typename T>
	auto get(const string_t &key, T defaultValue) -> T;
	template <typename T>
	auto get(int index, T defaultValue) -> T;

	template <typename T>
	auto set(lua_State *luaVm, const string_t &key, const T &value) -> LuaEnvironment &;
	template <typename T>
	auto push(lua_State *luaVm, const T &value) -> LuaEnvironment &;
	template <typename T>
	auto get(lua_State *luaVm, const string_t &key) -> T;
	template <typename T>
	auto get(lua_State *luaVm, int index) -> T;
	template <typename T>
	auto get(lua_State *luaVm, const string_t &key, T defaultValue) -> T;
	template <typename T>
	auto get(lua_State *luaVm, int index, T defaultValue) -> T;

	template <typename ... TArgs>
	auto call(const string_t &func, TArgs ... args) -> Result;
	template <typename ... TArgs>
	auto call(int numberOfReturnResults, const string_t &func, TArgs ... args) -> Result;
	template <typename ... TArgs>
	auto call(lua_State *luaVm, const string_t &func, TArgs ... args) -> Result;
	template <typename ... TArgs>
	auto call(lua_State *luaVm, int numberOfReturnResults, const string_t &func, TArgs ... args) -> Result;
protected:
	LuaEnvironment(const string_t &filename, bool useThread);

	virtual auto handleError(const string_t &filename, const string_t &error) -> void;
	virtual auto handleFileNotFound(const string_t &filename) -> void;
	virtual auto handleKeyNotFound(const string_t &filename, const string_t &key) -> void;
	virtual auto handleThreadCompletion() -> void;
	auto printError(const string_t &error) const -> void;
	auto expose(const string_t &name, lua_function_t func) -> void;
	auto resume(int pushedArgCount) -> Result;
	auto requireStandardLib(const string_t &localName, lua_function_t func) -> void;

	template <typename T>
	auto pushThread(const T &value) -> void;
private:
	auto loadFile(const string_t &filename) -> void;
	auto keyMustExist(const string_t &key) -> void;
	template <typename THead, typename ... TTail>
	auto callImpl(lua_State *luaVm, const THead &arg, const TTail & ... rest) -> void;
	auto callImpl(lua_State *luaVm) -> void;

	// For these Impl functions, they aren't specialized for anything except primitives/string
	// The others are overloads - this allows the dispatch mechanism to allow templated types
	// Phrased differently, if your only argument is TElement and TElement is vector_t<something>, how do you know you can iterate it?

	// Begin pushImpl
	template <typename T>
	auto pushImpl(lua_State *luaVm, const T &value) -> void;
	template <>
	auto pushImpl<bool>(lua_State *luaVm, const bool &value) -> void;
	template <>
	auto pushImpl<double>(lua_State *luaVm, const double &value) -> void;
	template <>
	auto pushImpl<string_t>(lua_State *luaVm, const string_t &value) -> void;
	template <>
	auto pushImpl<int8_t>(lua_State *luaVm, const int8_t &value) -> void;
	template <>
	auto pushImpl<int16_t>(lua_State *luaVm, const int16_t &value) -> void;
	template <>
	auto pushImpl<int32_t>(lua_State *luaVm, const int32_t &value) -> void;
	template <>
	auto pushImpl<uint8_t>(lua_State *luaVm, const uint8_t &value) -> void;
	template <>
	auto pushImpl<uint16_t>(lua_State *luaVm, const uint16_t &value) -> void;
	template <>
	auto pushImpl<uint32_t>(lua_State *luaVm, const uint32_t &value) -> void;
	template <>
	auto pushImpl<milliseconds_t>(lua_State *luaVm, const milliseconds_t &value) -> void;
	template <>
	auto pushImpl<seconds_t>(lua_State *luaVm, const seconds_t &value) -> void;
	template <>
	auto pushImpl<minutes_t>(lua_State *luaVm, const minutes_t &value) -> void;
	template <>
	auto pushImpl<hours_t>(lua_State *luaVm, const hours_t &value) -> void;
	template <typename TElement>
	auto pushImpl(lua_State *luaVm, const vector_t<TElement> &value) -> void;
	template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
	auto pushImpl(lua_State *luaVm, const hash_map_t<TKey, TElement, THash, TOperation> &value) -> void;
	// End pushImpl

	// Begin getImpl key
	template <typename T>
	auto getImplDefault(lua_State *luaVm, const string_t &key) -> T;
	template <typename T>
	auto getImpl(lua_State *luaVm, const string_t &key, T *) -> T;
	template <>
	auto getImpl<bool>(lua_State *luaVm, const string_t &key, bool *) -> bool;
	template <>
	auto getImpl<double>(lua_State *luaVm, const string_t &key, double *) -> double;
	template <>
	auto getImpl<string_t>(lua_State *luaVm, const string_t &key, string_t *) -> string_t;
	template <>
	auto getImpl<int8_t>(lua_State *luaVm, const string_t &key, int8_t *) -> int8_t;
	template <>
	auto getImpl<int16_t>(lua_State *luaVm, const string_t &key, int16_t *) -> int16_t;
	template <>
	auto getImpl<int32_t>(lua_State *luaVm, const string_t &key, int32_t *) -> int32_t;
	template <>
	auto getImpl<uint8_t>(lua_State *luaVm, const string_t &key, uint8_t *) -> uint8_t;
	template <>
	auto getImpl<uint16_t>(lua_State *luaVm, const string_t &key, uint16_t *) -> uint16_t;
	template <>
	auto getImpl<uint32_t>(lua_State *luaVm, const string_t &key, uint32_t *) -> uint32_t;
	template <>
	auto getImpl<milliseconds_t>(lua_State *luaVm, const string_t &key, milliseconds_t *) -> milliseconds_t;
	template <>
	auto getImpl<seconds_t>(lua_State *luaVm, const string_t &key, seconds_t *) -> seconds_t;
	template <>
	auto getImpl<minutes_t>(lua_State *luaVm, const string_t &key, minutes_t *) -> minutes_t;
	template <>
	auto getImpl<hours_t>(lua_State *luaVm, const string_t &key, hours_t *) -> hours_t;
	template <typename TElement>
	auto getImpl(lua_State *luaVm, const string_t &key, vector_t<TElement> *) -> vector_t<TElement>;
	template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
	auto getImpl(lua_State *luaVm, const string_t &key, hash_map_t<TKey, TElement, THash, TOperation> *) -> hash_map_t<TKey, TElement, THash, TOperation>;
	// End getImpl key

	// Begin getImpl index
	template <typename TInteger>
	auto getInteger(lua_State *luaVm, int index) -> TInteger;
	template <typename T>
	auto getImpl(lua_State *luaVm, int index, T *) -> T;
	template <>
	auto getImpl<bool>(lua_State *luaVm, int index, bool *) -> bool;
	template <>
	auto getImpl<double>(lua_State *luaVm, int index, double *) -> double;
	template <>
	auto getImpl<string_t>(lua_State *luaVm, int index, string_t *) -> string_t;
	template <>
	auto getImpl<int8_t>(lua_State *luaVm, int index, int8_t *) -> int8_t;
	template <>
	auto getImpl<int16_t>(lua_State *luaVm, int index, int16_t *) -> int16_t;
	template <>
	auto getImpl<int32_t>(lua_State *luaVm, int index, int32_t *) -> int32_t;
	template <>
	auto getImpl<uint8_t>(lua_State *luaVm, int index, uint8_t *) -> uint8_t;
	template <>
	auto getImpl<uint16_t>(lua_State *luaVm, int index, uint16_t *) -> uint16_t;
	template <>
	auto getImpl<uint32_t>(lua_State *luaVm, int index, uint32_t *) -> uint32_t;
	template <>
	auto getImpl<milliseconds_t>(lua_State *luaVm, int index, milliseconds_t *) -> milliseconds_t;
	template <>
	auto getImpl<seconds_t>(lua_State *luaVm, int index, seconds_t *) -> seconds_t;
	template <>
	auto getImpl<minutes_t>(lua_State *luaVm, int index, minutes_t *) -> minutes_t;
	template <>
	auto getImpl<hours_t>(lua_State *luaVm, int index, hours_t *) -> hours_t;
	template <typename TElement>
	auto getImpl(lua_State *luaVm, int index, vector_t<TElement> *) -> vector_t<TElement>;
	template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
	auto getImpl(lua_State *luaVm, int index, hash_map_t<TKey, TElement, THash, TOperation> *) -> hash_map_t<TKey, TElement, THash, TOperation>;
	// End getImpl index

	static LoopingId<int32_t> s_identifiers;
	static hash_map_t<int32_t, LuaEnvironment *> s_environments;

	lua_State *m_luaVm = nullptr;
	lua_State *m_luaThread = nullptr;
	string_t m_file;
	int32_t m_environmentIdentifier;
};

template <typename T>
auto LuaEnvironment::set(const string_t &key, const T &value) -> LuaEnvironment & {
	return set(m_luaVm, key, value);
}

template <typename T>
auto LuaEnvironment::set(lua_State *luaVm, const string_t &key, const T &value) -> LuaEnvironment & {
	push(luaVm, value);
	lua_setglobal(luaVm, key.c_str());
	return *this;
}

template <typename T>
auto LuaEnvironment::push(const T &value) -> LuaEnvironment & {
	return push(m_luaVm, value);
}

template <typename T>
auto LuaEnvironment::push(lua_State *luaVm, const T &value) -> LuaEnvironment & {
	pushImpl(luaVm, value);
	return *this;
}

template <typename T>
auto LuaEnvironment::get(const string_t &key) -> T {
	return get<T>(m_luaVm, key);
}

template <typename T>
auto LuaEnvironment::get(const string_t &key, T defaultValue) -> T {
	if (!exists(m_luaVm, key)) {
		return defaultValue;
	}
	return get<T>(m_luaVm, key);
}

template <typename T>
auto LuaEnvironment::get(lua_State *luaVm, const string_t &key) -> T {
	auto v = getImpl(luaVm, key, static_cast<T *>(nullptr));
	return v;
}

template <typename T>
auto LuaEnvironment::get(lua_State *luaVm, const string_t &key, T defaultValue) -> T {
	if (!exists(luaVm, key)) {
		return defaultValue;
	}

	auto v = getImpl(luaVm, key, static_cast<T *>(nullptr));
	return v;
}

template <typename T>
auto LuaEnvironment::get(int index) -> T {
	return get<T>(m_luaVm, index);
}

template <typename T>
auto LuaEnvironment::get(int index, T defaultValue) -> T {
	if (is(index, LuaType::Nil) || is(index, LuaType::None)) {
		return defaultValue;
	}
	return get<T>(m_luaVm, index);
}

template <typename T>
auto LuaEnvironment::get(lua_State *luaVm, int index) -> T {
	auto v = getImpl(luaVm, index, static_cast<T *>(nullptr));
	return v;
}

template <typename T>
auto LuaEnvironment::get(lua_State *luaVm, int index, T defaultValue) -> T {
	if (is(luaVm, index, LuaType::Nil) || is(luaVm, index, LuaType::None)) {
		return defaultValue;
	}
	auto v = getImpl(luaVm, index, static_cast<T *>(nullptr));
	return v;
}

template <typename ... TArgs>
auto LuaEnvironment::call(const string_t &func, TArgs ... args) -> Result {
	return call(m_luaVm, func, args...);
}

template <typename ... TArgs>
auto LuaEnvironment::call(int numberOfReturnResults, const string_t &func, TArgs ... args) -> Result {
	return call(m_luaVm, numberOfReturnResults, func, args...);
}

template <typename ... TArgs>
auto LuaEnvironment::call(lua_State *luaVm, const string_t &func, TArgs ... args) -> Result {
	return call(luaVm, 0, func, args...);
}

template <typename ... TArgs>
auto LuaEnvironment::call(lua_State *luaVm, int numberOfReturnResults, const string_t &func, TArgs ... args) -> Result {
	lua_getglobal(luaVm, func.c_str());
	callImpl(luaVm, args...);

	if (lua_pcall(luaVm, sizeof...(args), numberOfReturnResults, 0)) {
		string_t error = lua_tostring(luaVm, -1);
		handleError(m_file, error);
		return Result::Failure;
	}

	return Result::Successful;
}

template <typename THead, typename ... TTail>
auto LuaEnvironment::callImpl(lua_State *luaVm, const THead &arg, const TTail & ... rest) -> void {
	push<THead>(luaVm, arg);
	callImpl(luaVm, rest...);
}

inline
auto LuaEnvironment::callImpl(lua_State *luaVm) -> void {
	// Intentionally blank
}

template <typename T>
auto LuaEnvironment::pushThread(const T &value) -> void {
	push(m_luaThread, value);
}

// Begin pushImpl
template <typename T>
auto LuaEnvironment::pushImpl(lua_State *luaVm, const T &value) -> void {
	static_assert(false, "T is not appropriately specialized for that type");
	throw std::logic_error("T is not appropriately specialized for that type");
}

template <>
auto LuaEnvironment::pushImpl<bool>(lua_State *luaVm, const bool &value) -> void {
	lua_pushboolean(luaVm, value);
}

template <>
auto LuaEnvironment::pushImpl<double>(lua_State *luaVm, const double &value) -> void {
	lua_pushnumber(luaVm, value);
}

template <>
auto LuaEnvironment::pushImpl<string_t>(lua_State *luaVm, const string_t &value) -> void {
	lua_pushstring(luaVm, value.c_str());
}

template <>
auto LuaEnvironment::pushImpl<int8_t>(lua_State *luaVm, const int8_t &value) -> void {
	lua_pushinteger(luaVm, value);
}

template <>
auto LuaEnvironment::pushImpl<int16_t>(lua_State *luaVm, const int16_t &value) -> void {
	lua_pushinteger(luaVm, value);
}

template <>
auto LuaEnvironment::pushImpl<int32_t>(lua_State *luaVm, const int32_t &value) -> void {
	lua_pushinteger(luaVm, value);
}

template <>
auto LuaEnvironment::pushImpl<uint8_t>(lua_State *luaVm, const uint8_t &value) -> void {
	lua_pushinteger(luaVm, value);
}

template <>
auto LuaEnvironment::pushImpl<uint16_t>(lua_State *luaVm, const uint16_t &value) -> void {
	lua_pushinteger(luaVm, value);
}

template <>
auto LuaEnvironment::pushImpl<uint32_t>(lua_State *luaVm, const uint32_t &value) -> void {
	lua_pushinteger(luaVm, value);
}

template <>
auto LuaEnvironment::pushImpl<milliseconds_t>(lua_State *luaVm, const milliseconds_t &value) -> void {
	lua_pushinteger(luaVm, static_cast<int32_t>(value.count()));
}

template <>
auto LuaEnvironment::pushImpl<seconds_t>(lua_State *luaVm, const seconds_t &value) -> void {
	lua_pushinteger(luaVm, static_cast<int32_t>(value.count()));
}

template <>
auto LuaEnvironment::pushImpl<minutes_t>(lua_State *luaVm, const minutes_t &value) -> void {
	lua_pushinteger(luaVm, static_cast<int32_t>(value.count()));
}

template <>
auto LuaEnvironment::pushImpl<hours_t>(lua_State *luaVm, const hours_t &value) -> void {
	lua_pushinteger(luaVm, static_cast<int32_t>(value.count()));
}

template <typename TElement>
auto LuaEnvironment::pushImpl(lua_State *luaVm, const vector_t<TElement> &value) -> void {
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (size_t i = 0; i < value.size(); ++i) {
		push<int>(luaVm, i + 1);
		push<TElement>(luaVm, value[i]);
		lua_settable(luaVm, top);
	}
}

template <typename TKey, typename TElement, typename THash, typename TOperation>
auto LuaEnvironment::pushImpl(lua_State *luaVm, const hash_map_t<TKey, TElement, THash, TOperation> &value) -> void {
	lua_newtable(luaVm);
	int top = lua_gettop(luaVm);
	for (const auto &kvp : value) {
		push<TKey>(luaVm, kvp.first);
		push<TElement>(luaVm, kvp.second);
		lua_settable(luaVm, top);
	}
}
// End pushImpl

// Begin getImpl key
template <typename T>
auto LuaEnvironment::getImplDefault(lua_State *luaVm, const string_t &key) -> T {
	keyMustExist(key);
	lua_getglobal(luaVm, key.c_str());
	T val = get<T>(luaVm, -1);
	lua_pop(luaVm, 1);
	return val;
}

template <typename T>
auto LuaEnvironment::getImpl(lua_State *luaVm, const string_t &key, T *) -> T {
	LuaSerialize<T> x;
	return x.read(*this, key);
}

template <>
auto LuaEnvironment::getImpl<bool>(lua_State *luaVm, const string_t &key, bool *) -> bool {
	return getImplDefault<bool>(luaVm, key);
}

template <>
auto LuaEnvironment::getImpl<double>(lua_State *luaVm, const string_t &key, double *) -> double {
	return getImplDefault<double>(luaVm, key);
}

template <>
auto LuaEnvironment::getImpl<string_t>(lua_State *luaVm, const string_t &key, string_t *) -> string_t {
	return getImplDefault<string_t>(luaVm, key);
}

template <>
auto LuaEnvironment::getImpl<int8_t>(lua_State *luaVm, const string_t &key, int8_t *) -> int8_t {
	return getImplDefault<int8_t>(luaVm, key);
}

template <>
auto LuaEnvironment::getImpl<int16_t>(lua_State *luaVm, const string_t &key, int16_t *) -> int16_t {
	return getImplDefault<int16_t>(luaVm, key);
}

template <>
auto LuaEnvironment::getImpl<int32_t>(lua_State *luaVm, const string_t &key, int32_t *) -> int32_t {
	return getImplDefault<int32_t>(luaVm, key);
}

template <>
auto LuaEnvironment::getImpl<uint8_t>(lua_State *luaVm, const string_t &key, uint8_t *) -> uint8_t {
	return getImplDefault<uint8_t>(luaVm, key);
}

template <>
auto LuaEnvironment::getImpl<uint16_t>(lua_State *luaVm, const string_t &key, uint16_t *) -> uint16_t {
	return getImplDefault<uint16_t>(luaVm, key);
}

template <>
auto LuaEnvironment::getImpl<uint32_t>(lua_State *luaVm, const string_t &key, uint32_t *) -> uint32_t {
	return getImplDefault<uint32_t>(luaVm, key);
}

template <>
auto LuaEnvironment::getImpl<milliseconds_t>(lua_State *luaVm, const string_t &key, milliseconds_t *) -> milliseconds_t {
	return milliseconds_t(getImplDefault<int32_t>(luaVm, key));
}

template <>
auto LuaEnvironment::getImpl<seconds_t>(lua_State *luaVm, const string_t &key, seconds_t *) -> seconds_t {
	return seconds_t(getImplDefault<int32_t>(luaVm, key));
}

template <>
auto LuaEnvironment::getImpl<minutes_t>(lua_State *luaVm, const string_t &key, minutes_t *) -> minutes_t {
	return minutes_t(getImplDefault<int32_t>(luaVm, key));
}

template <>
auto LuaEnvironment::getImpl<hours_t>(lua_State *luaVm, const string_t &key, hours_t *) -> hours_t {
	return hours_t(getImplDefault<int32_t>(luaVm, key));
}

template <typename TElement>
auto LuaEnvironment::getImpl(lua_State *luaVm, const string_t &key, vector_t<TElement> *) -> vector_t<TElement> {
	keyMustExist(key);
	lua_getglobal(luaVm, key.c_str());
	vector_t<TElement> val = get<vector_t<TElement>>(luaVm, -1);
	lua_pop(luaVm, 1);
	return val;
}

template <typename TKey, typename TElement, typename THash, typename TOperation>
auto LuaEnvironment::getImpl(lua_State *luaVm, const string_t &key, hash_map_t<TKey, TElement, THash, TOperation> *) -> hash_map_t<TKey, TElement, THash, TOperation> {
	keyMustExist(key);
	lua_getglobal(luaVm, key.c_str());
	hash_map_t<TKey, TElement, THash, TOperation> val = get<hash_map_t<TKey, TElement, THash, TOperation>>(luaVm, -1);
	lua_pop(luaVm, 1);
	return val;
}
// End getImpl key

// Begin getImpl index
template <typename TInteger>
auto LuaEnvironment::getInteger(lua_State *luaVm, int index) -> TInteger {
	TInteger val = static_cast<TInteger>(lua_tointeger(luaVm, index));
	return val;
}

template <typename T>
auto LuaEnvironment::getImpl(lua_State *luaVm, int index, T *) -> T {
	static_assert(false, "T is not appropriately specialized for that type");
	throw std::logic_error("T is not appropriately specialized for that type");
}

template <>
auto LuaEnvironment::getImpl<bool>(lua_State *luaVm, int index, bool *) -> bool {
	bool val = lua_toboolean(luaVm, index) != 0;
	return val;
}

template <>
auto LuaEnvironment::getImpl<double>(lua_State *luaVm, int index, double *) -> double {
	double val = lua_tonumber(luaVm, index);
	return val;
}

template <>
auto LuaEnvironment::getImpl<string_t>(lua_State *luaVm, int index, string_t *) -> string_t {
	string_t val = lua_tostring(luaVm, index);
	return val;
}

template <>
auto LuaEnvironment::getImpl<int8_t>(lua_State *luaVm, int index, int8_t *) -> int8_t {
	return getInteger<int8_t>(luaVm, index);
}

template <>
auto LuaEnvironment::getImpl<int16_t>(lua_State *luaVm, int index, int16_t *) -> int16_t {
	return getInteger<int16_t>(luaVm, index);
}

template <>
auto LuaEnvironment::getImpl<int32_t>(lua_State *luaVm, int index, int32_t *) -> int32_t {
	return getInteger<int32_t>(luaVm, index);
}

template <>
auto LuaEnvironment::getImpl<uint8_t>(lua_State *luaVm, int index, uint8_t *) -> uint8_t {
	return getInteger<uint8_t>(luaVm, index);
}

template <>
auto LuaEnvironment::getImpl<uint16_t>(lua_State *luaVm, int index, uint16_t *) -> uint16_t {
	return getInteger<uint16_t>(luaVm, index);
}

template <>
auto LuaEnvironment::getImpl<uint32_t>(lua_State *luaVm, int index, uint32_t *) -> uint32_t {
	return getInteger<uint32_t>(luaVm, index);
}

template <>
auto LuaEnvironment::getImpl<milliseconds_t>(lua_State *luaVm, int index, milliseconds_t *) -> milliseconds_t {
	return milliseconds_t(getInteger<int32_t>(luaVm, index));
}

template <>
auto LuaEnvironment::getImpl<seconds_t>(lua_State *luaVm, int index, seconds_t *) -> seconds_t {
	return seconds_t(getInteger<int32_t>(luaVm, index));
}

template <>
auto LuaEnvironment::getImpl<minutes_t>(lua_State *luaVm, int index, minutes_t *) -> minutes_t {
	return minutes_t(getInteger<int32_t>(luaVm, index));
}

template <>
auto LuaEnvironment::getImpl<hours_t>(lua_State *luaVm, int index, hours_t *) -> hours_t {
	return hours_t(getInteger<int32_t>(luaVm, index));
}

template <typename TElement>
auto LuaEnvironment::getImpl(lua_State *luaVm, int index, vector_t<TElement> *) -> vector_t<TElement> {
	vector_t<TElement> val;
	// Adjust index for the name
	lua_pushvalue(luaVm, index);

	pushNil(luaVm);
	while (lua_next(luaVm, -2)) {
		// We don't have to account for string conversions here, it's only important for keys

		val.push_back(get<TElement>(luaVm, -1));

		lua_pop(luaVm, 1);
	}

	lua_pop(luaVm, 1);

	return val;
}

template <typename TKey, typename TElement, typename THash, typename TOperation>
auto LuaEnvironment::getImpl(lua_State *luaVm, int index, hash_map_t<TKey, TElement, THash, TOperation> *) -> hash_map_t<TKey, TElement, THash, TOperation> {
	hash_map_t<TKey, TElement, THash, TOperation> val;
	// Adjust index for the name
	lua_pushvalue(luaVm, index);

	pushNil(luaVm);
	while (lua_next(luaVm, -2)) {
		// We have to account for string conversions here, but it's only important for keys
		lua_pushvalue(luaVm, -2);

		auto key = get<TKey>(luaVm, -1);
		auto value = get<TElement>(luaVm, -2);
		val[key] = value;

		lua_pop(luaVm, 2);
	}

	lua_pop(luaVm, 1);

	return val;
}
// End getImpl index