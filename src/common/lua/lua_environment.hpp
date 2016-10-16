/*
Copyright (C) 2008-2016 Vana Development Team

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
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include "common/lua/lua_type.hpp"
#include "common/lua/lua_variant.hpp"
#include "common/types.hpp"
#include "common/util/object_pool.hpp"
#include <stdexcept>
#include <string>
#include <vector>

namespace vana {
	namespace lua {
		class lua_environment;

		template <typename T>
		struct lua_serialize {
			auto read(lua_environment &config, int stack_index) -> T {
				static_assert(false, "index read of T is not appropriately specialized for that type");
				throw std::logic_error{"index read of T is not appropriately specialized for that type"};
			}

			auto read(lua_environment &config, const string &prefix) -> T {
				static_assert(false, "string prefix read of T is not appropriately specialized for that type");
				throw std::logic_error{"string prefix read of T is not appropriately specialized for that type"};
			}
		};

		// TODO FIXME lua
		// Lua 5.2 doesn't support 64-bit integers out of the box, it needs special compilation and #defines to make it right
		// Lua 5.3 tentatively looks like 64-bit integer will be the default integer size, so at that point, I can update this
		class lua_environment {
			NONCOPYABLE(lua_environment);
			NO_DEFAULT_CONSTRUCTOR(lua_environment);
		public:
			lua_environment(const string &filename);
			~lua_environment();

			static auto get_environment(lua_State *lua_vm) -> lua_environment &;

			auto run() -> result;
			auto error(const string &text) -> void;

			auto exists(const string &key) -> bool;
			auto exists(lua_State *lua_vm, const string &key) -> bool;
			auto is(const string &key, lua_type type) -> bool;
			auto is(lua_State *lua_vm, const string &key, lua_type type) -> bool;
			auto is(int index, lua_type type) -> bool;
			auto is(lua_State *lua_vm, int index, lua_type type) -> bool;
			auto is_any_of(const string &key, init_list<lua_type> types) -> bool;
			auto is_any_of(lua_State *lua_vm, const string &key, init_list<lua_type> types) -> bool;
			auto is_any_of(int index, init_list<lua_type> types) -> bool;
			auto is_any_of(lua_State *lua_vm, int index, init_list<lua_type> types) -> bool;
			auto type_of(int index) -> lua_type;
			auto type_of(lua_State *lua_vm, int index) -> lua_type;
			auto pop(int count = 1) -> void;
			auto pop(lua_State *lua_vm, int count = 1) -> void;
			auto count() -> int;
			auto count(lua_State *lua_vm) -> int;
			auto validate_value(lua_type expected_type, const lua_variant &v, const string &key, const string &prefix, bool nil_is_valid = false) -> lua_type;
			auto validate_key(lua_type expected_type, const lua_variant &v, const string &prefix) -> void;
			auto validate_object(lua_type expected_type, const lua_variant &v, const string &prefix) -> void;
			auto required(bool present, const string &key, const string &prefix) -> void;

			auto yield(lua::lua_return quantity_return_results_passed_to_resume) -> lua::lua_return;
			auto push_nil() -> lua_environment &;
			auto push_nil(lua_State *lua_vm) -> lua_environment &;

			auto get_script_name() -> string;
			auto get_script_path() -> vector<string>;

			template <typename T>
			auto set(const string &key, const T &value) -> lua_environment &;
			template <typename T>
			auto push(const T &value) -> lua_environment &;
			template <typename T>
			auto get(const string &key) -> T;
			template <typename T>
			auto get(int index) -> T;
			template <typename T>
			auto get(const string &key, T default_value) -> T;
			template <typename T>
			auto get(int index, T default_value) -> T;

			template <typename T>
			auto set(lua_State *lua_vm, const string &key, const T &value) -> lua_environment &;
			template <typename T>
			auto push(lua_State *lua_vm, const T &value) -> lua_environment &;
			template <typename T>
			auto get(lua_State *lua_vm, const string &key) -> T;
			template <typename T>
			auto get(lua_State *lua_vm, int index) -> T;
			template <typename T>
			auto get(lua_State *lua_vm, const string &key, T default_value) -> T;
			template <typename T>
			auto get(lua_State *lua_vm, int index, T default_value) -> T;

			template <typename ... TArgs>
			auto call(const string &func, TArgs ... args) -> result;
			template <typename ... TArgs>
			auto call(int quantity_return_results, const string &func, TArgs ... args) -> result;
			template <typename ... TArgs>
			auto call(lua_State *lua_vm, const string &func, TArgs ... args) -> result;
			template <typename ... TArgs>
			auto call(lua_State *lua_vm, int quantity_return_results, const string &func, TArgs ... args) -> result;
		protected:
			lua_environment(const string &filename, bool use_thread);

			virtual auto handle_error(const string &filename, const string &error) -> void;
			virtual auto handle_file_not_found(const string &filename) -> void;
			virtual auto handle_key_not_found(const string &filename, const string &key) -> void;
			virtual auto handle_thread_completion() -> void;
			auto print_error(const string &error) const -> void;
			auto expose(const string &name, lua::lua_function func) -> void;
			auto resume(lua::lua_return pushed_arg_count) -> result;
			auto require_standard_lib(const string &local_name, lua::lua_function func) -> void;

			template <typename T>
			auto push_thread(const T &value) -> void;
		private:
			auto load_file(const string &filename) -> void;
			auto key_must_exist(const string &key) -> void;
			template <typename THead, typename ... TTail>
			auto call_impl(lua_State *lua_vm, const THead &arg, const TTail & ... rest) -> void;
			auto call_impl(lua_State *lua_vm) -> void;

			// For these Impl functions, they aren't specialized for anything except primitives/string
			// The others are overloads - this allows the dispatch mechanism to allow templated types
			// Phrased differently, if your only argument is TElement and TElement is vector<something>, how do you know you can iterate it?

			// Begin push_impl
			template <typename T>
			auto push_impl(lua_State *lua_vm, const T &value) -> void;
			template <>
			auto push_impl<bool>(lua_State *lua_vm, const bool &value) -> void;
			template <>
			auto push_impl<double>(lua_State *lua_vm, const double &value) -> void;
			template <>
			auto push_impl<string>(lua_State *lua_vm, const string &value) -> void;
			template <>
			auto push_impl<int8_t>(lua_State *lua_vm, const int8_t &value) -> void;
			template <>
			auto push_impl<int16_t>(lua_State *lua_vm, const int16_t &value) -> void;
			template <>
			auto push_impl<int32_t>(lua_State *lua_vm, const int32_t &value) -> void;
			template <>
			auto push_impl<uint8_t>(lua_State *lua_vm, const uint8_t &value) -> void;
			template <>
			auto push_impl<uint16_t>(lua_State *lua_vm, const uint16_t &value) -> void;
			template <>
			auto push_impl<uint32_t>(lua_State *lua_vm, const uint32_t &value) -> void;
			template <>
			auto push_impl<milliseconds>(lua_State *lua_vm, const milliseconds &value) -> void;
			template <>
			auto push_impl<seconds>(lua_State *lua_vm, const seconds &value) -> void;
			template <>
			auto push_impl<minutes>(lua_State *lua_vm, const minutes &value) -> void;
			template <>
			auto push_impl<hours>(lua_State *lua_vm, const hours &value) -> void;
			template <>
			auto push_impl<lua_variant>(lua_State *lua_vm, const lua_variant &value) -> void;
			template <typename TElement>
			auto push_impl(lua_State *lua_vm, const vector<TElement> &value) -> void;
			template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
			auto push_impl(lua_State *lua_vm, const hash_map<TKey, TElement, THash, TOperation> &value) -> void;
			template <typename TKey, typename TElement, typename TOperation = std::less<TKey>>
			auto push_impl(lua_State *lua_vm, const ord_map<TKey, TElement, TOperation> &value) -> void;
			// End push_impl

			// Begin get_impl key
			template <typename T>
			auto get_impl_default(lua_State *lua_vm, const string &key) -> T;
			template <typename T>
			auto get_impl(lua_State *lua_vm, const string &key, T *) -> T;
			template <>
			auto get_impl<bool>(lua_State *lua_vm, const string &key, bool *) -> bool;
			template <>
			auto get_impl<double>(lua_State *lua_vm, const string &key, double *) -> double;
			template <>
			auto get_impl<string>(lua_State *lua_vm, const string &key, string *) -> string;
			template <>
			auto get_impl<int8_t>(lua_State *lua_vm, const string &key, int8_t *) -> int8_t;
			template <>
			auto get_impl<int16_t>(lua_State *lua_vm, const string &key, int16_t *) -> int16_t;
			template <>
			auto get_impl<int32_t>(lua_State *lua_vm, const string &key, int32_t *) -> int32_t;
			template <>
			auto get_impl<uint8_t>(lua_State *lua_vm, const string &key, uint8_t *) -> uint8_t;
			template <>
			auto get_impl<uint16_t>(lua_State *lua_vm, const string &key, uint16_t *) -> uint16_t;
			template <>
			auto get_impl<uint32_t>(lua_State *lua_vm, const string &key, uint32_t *) -> uint32_t;
			template <>
			auto get_impl<milliseconds>(lua_State *lua_vm, const string &key, milliseconds *) -> milliseconds;
			template <>
			auto get_impl<seconds>(lua_State *lua_vm, const string &key, seconds *) -> seconds;
			template <>
			auto get_impl<minutes>(lua_State *lua_vm, const string &key, minutes *) -> minutes;
			template <>
			auto get_impl<hours>(lua_State *lua_vm, const string &key, hours *) -> hours;
			template <>
			auto get_impl<lua_variant>(lua_State *lua_vm, const string &key, lua_variant *) -> lua_variant;
			template <typename TElement>
			auto get_impl(lua_State *lua_vm, const string &key, vector<TElement> *) -> vector<TElement>;
			template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
			auto get_impl(lua_State *lua_vm, const string &key, hash_map<TKey, TElement, THash, TOperation> *) -> hash_map<TKey, TElement, THash, TOperation>;
			template <typename TKey, typename TElement, typename TOperation = std::less<TKey>>
			auto get_impl(lua_State *lua_vm, const string &key, ord_map<TKey, TElement, TOperation> *) -> ord_map<TKey, TElement, TOperation>;
			// End get_impl key

			// Begin get_impl index
			template <typename TInteger>
			auto getInteger(lua_State *lua_vm, int index) -> TInteger;
			template <typename T>
			auto get_impl(lua_State *lua_vm, int index, T *) -> T;
			template <>
			auto get_impl<bool>(lua_State *lua_vm, int index, bool *) -> bool;
			template <>
			auto get_impl<double>(lua_State *lua_vm, int index, double *) -> double;
			template <>
			auto get_impl<string>(lua_State *lua_vm, int index, string *) -> string;
			template <>
			auto get_impl<int8_t>(lua_State *lua_vm, int index, int8_t *) -> int8_t;
			template <>
			auto get_impl<int16_t>(lua_State *lua_vm, int index, int16_t *) -> int16_t;
			template <>
			auto get_impl<int32_t>(lua_State *lua_vm, int index, int32_t *) -> int32_t;
			template <>
			auto get_impl<uint8_t>(lua_State *lua_vm, int index, uint8_t *) -> uint8_t;
			template <>
			auto get_impl<uint16_t>(lua_State *lua_vm, int index, uint16_t *) -> uint16_t;
			template <>
			auto get_impl<uint32_t>(lua_State *lua_vm, int index, uint32_t *) -> uint32_t;
			template <>
			auto get_impl<milliseconds>(lua_State *lua_vm, int index, milliseconds *) -> milliseconds;
			template <>
			auto get_impl<seconds>(lua_State *lua_vm, int index, seconds *) -> seconds;
			template <>
			auto get_impl<minutes>(lua_State *lua_vm, int index, minutes *) -> minutes;
			template <>
			auto get_impl<hours>(lua_State *lua_vm, int index, hours *) -> hours;
			template <>
			auto get_impl<lua_variant>(lua_State *lua_vm, int index, lua_variant *) -> lua_variant;
			template <typename TElement>
			auto get_impl(lua_State *lua_vm, int index, vector<TElement> *) -> vector<TElement>;
			template <typename TKey, typename TElement, typename THash = std::hash<TKey>, typename TOperation = std::equal_to<TKey>>
			auto get_impl(lua_State *lua_vm, int index, hash_map<TKey, TElement, THash, TOperation> *) -> hash_map<TKey, TElement, THash, TOperation>;
			template <typename TKey, typename TElement, typename TOperation = std::less<TKey>>
			auto get_impl(lua_State *lua_vm, int index, ord_map<TKey, TElement, TOperation> *) -> ord_map<TKey, TElement, TOperation>;
			// End get_impl index

			static vana::util::object_pool<int32_t, lua_environment *> s_environments;

			lua_State *m_lua_vm = nullptr;
			lua_State *m_lua_thread = nullptr;
			string m_file;
			int32_t m_environment_id;
		};

		template <typename T>
		auto lua_environment::set(const string &key, const T &value) -> lua_environment & {
			return set(m_lua_vm, key, value);
		}

		template <typename T>
		auto lua_environment::set(lua_State *lua_vm, const string &key, const T &value) -> lua_environment & {
			push(lua_vm, value);
			lua_setglobal(lua_vm, key.c_str());
			return *this;
		}

		template <typename T>
		auto lua_environment::push(const T &value) -> lua_environment & {
			return push(m_lua_vm, value);
		}

		template <typename T>
		auto lua_environment::push(lua_State *lua_vm, const T &value) -> lua_environment & {
			push_impl(lua_vm, value);
			return *this;
		}

		template <typename T>
		auto lua_environment::get(const string &key) -> T {
			return get<T>(m_lua_vm, key);
		}

		template <typename T>
		auto lua_environment::get(const string &key, T default_value) -> T {
			if (!exists(m_lua_vm, key)) {
				return default_value;
			}
			return get<T>(m_lua_vm, key);
		}

		template <typename T>
		auto lua_environment::get(lua_State *lua_vm, const string &key) -> T {
			auto v = get_impl(lua_vm, key, static_cast<T *>(nullptr));
			return v;
		}

		template <typename T>
		auto lua_environment::get(lua_State *lua_vm, const string &key, T default_value) -> T {
			if (!exists(lua_vm, key)) {
				return default_value;
			}

			auto v = get_impl(lua_vm, key, static_cast<T *>(nullptr));
			return v;
		}

		template <typename T>
		auto lua_environment::get(int index) -> T {
			return get<T>(m_lua_vm, index);
		}

		template <typename T>
		auto lua_environment::get(int index, T default_value) -> T {
			if (is(index, lua_type::nil) || is(index, lua_type::none)) {
				return default_value;
			}
			return get<T>(m_lua_vm, index);
		}

		template <typename T>
		auto lua_environment::get(lua_State *lua_vm, int index) -> T {
			auto v = get_impl(lua_vm, index, static_cast<T *>(nullptr));
			return v;
		}

		template <typename T>
		auto lua_environment::get(lua_State *lua_vm, int index, T default_value) -> T {
			if (is(lua_vm, index, lua_type::nil) || is(lua_vm, index, lua_type::none)) {
				return default_value;
			}
			auto v = get_impl(lua_vm, index, static_cast<T *>(nullptr));
			return v;
		}

		template <typename ... TArgs>
		auto lua_environment::call(const string &func, TArgs ... args) -> result {
			return call(m_lua_vm, func, args...);
		}

		template <typename ... TArgs>
		auto lua_environment::call(int quantity_return_results, const string &func, TArgs ... args) -> result {
			return call(m_lua_vm, quantity_return_results, func, args...);
		}

		template <typename ... TArgs>
		auto lua_environment::call(lua_State *lua_vm, const string &func, TArgs ... args) -> result {
			return call(lua_vm, 0, func, args...);
		}

		template <typename ... TArgs>
		auto lua_environment::call(lua_State *lua_vm, int quantity_return_results, const string &func, TArgs ... args) -> result {
			lua_getglobal(lua_vm, func.c_str());
			call_impl(lua_vm, args...);

			if (lua_pcall(lua_vm, sizeof...(args), quantity_return_results, 0)) {
				string error = lua_tostring(lua_vm, -1);
				handle_error(m_file, error);
				return result::failure;
			}

			return result::success;
		}

		template <typename THead, typename ... TTail>
		auto lua_environment::call_impl(lua_State *lua_vm, const THead &arg, const TTail & ... rest) -> void {
			push<THead>(lua_vm, arg);
			call_impl(lua_vm, rest...);
		}

		inline
		auto lua_environment::call_impl(lua_State *lua_vm) -> void {
			// Intentionally blank
		}

		template <typename T>
		auto lua_environment::push_thread(const T &value) -> void {
			push(m_lua_thread, value);
		}

		// Begin push_impl
		template <typename T>
		auto lua_environment::push_impl(lua_State *lua_vm, const T &value) -> void {
			static_assert(false, "T is not appropriately specialized for that type");
			throw std::logic_error{"T is not appropriately specialized for that type"};
		}

		template <>
		auto lua_environment::push_impl<bool>(lua_State *lua_vm, const bool &value) -> void {
			lua_pushboolean(lua_vm, value);
		}

		template <>
		auto lua_environment::push_impl<double>(lua_State *lua_vm, const double &value) -> void {
			lua_pushnumber(lua_vm, value);
		}

		template <>
		auto lua_environment::push_impl<string>(lua_State *lua_vm, const string &value) -> void {
			lua_pushstring(lua_vm, value.c_str());
		}

		template <>
		auto lua_environment::push_impl<int8_t>(lua_State *lua_vm, const int8_t &value) -> void {
			lua_pushinteger(lua_vm, value);
		}

		template <>
		auto lua_environment::push_impl<int16_t>(lua_State *lua_vm, const int16_t &value) -> void {
			lua_pushinteger(lua_vm, value);
		}

		template <>
		auto lua_environment::push_impl<int32_t>(lua_State *lua_vm, const int32_t &value) -> void {
			lua_pushinteger(lua_vm, value);
		}

		template <>
		auto lua_environment::push_impl<uint8_t>(lua_State *lua_vm, const uint8_t &value) -> void {
			lua_pushinteger(lua_vm, value);
		}

		template <>
		auto lua_environment::push_impl<uint16_t>(lua_State *lua_vm, const uint16_t &value) -> void {
			lua_pushinteger(lua_vm, value);
		}

		template <>
		auto lua_environment::push_impl<uint32_t>(lua_State *lua_vm, const uint32_t &value) -> void {
			lua_pushinteger(lua_vm, value);
		}

		template <>
		auto lua_environment::push_impl<milliseconds>(lua_State *lua_vm, const milliseconds &value) -> void {
			lua_pushinteger(lua_vm, static_cast<int32_t>(value.count()));
		}

		template <>
		auto lua_environment::push_impl<seconds>(lua_State *lua_vm, const seconds &value) -> void {
			lua_pushinteger(lua_vm, static_cast<int32_t>(value.count()));
		}

		template <>
		auto lua_environment::push_impl<minutes>(lua_State *lua_vm, const minutes &value) -> void {
			lua_pushinteger(lua_vm, static_cast<int32_t>(value.count()));
		}

		template <>
		auto lua_environment::push_impl<hours>(lua_State *lua_vm, const hours &value) -> void {
			lua_pushinteger(lua_vm, static_cast<int32_t>(value.count()));
		}

		template <>
		auto lua_environment::push_impl<lua_variant>(lua_State *lua_vm, const lua_variant &value) -> void {
			switch (value.get_type()) {
				case lua_type::nil: push_nil(lua_vm); break;
				case lua_type::boolean: push<bool>(lua_vm, value.as<bool>()); break;
				case lua_type::string: push<string>(lua_vm, value.as<string>()); break;
				case lua_type::number: push<double>(lua_vm, value.as<double>()); break;
				case lua_type::table: push<hash_map<lua_variant, lua_variant>>(lua_vm, value.as<hash_map<lua_variant, lua_variant>>()); break;
				default: THROW_CODE_EXCEPTION(not_implemented_exception, "lua_type");
			}
		}

		template <typename TElement>
		auto lua_environment::push_impl(lua_State *lua_vm, const vector<TElement> &value) -> void {
			lua_newtable(lua_vm);
			int top = lua_gettop(lua_vm);
			for (size_t i = 0; i < value.size(); ++i) {
				push<int>(lua_vm, i + 1);
				push<TElement>(lua_vm, value[i]);
				lua_settable(lua_vm, top);
			}
		}

		template <typename TKey, typename TElement, typename THash, typename TOperation>
		auto lua_environment::push_impl(lua_State *lua_vm, const hash_map<TKey, TElement, THash, TOperation> &value) -> void {
			lua_newtable(lua_vm);
			int top = lua_gettop(lua_vm);
			for (const auto &kvp : value) {
				push<TKey>(lua_vm, kvp.first);
				push<TElement>(lua_vm, kvp.second);
				lua_settable(lua_vm, top);
			}
		}

		template <typename TKey, typename TElement, typename TOperation>
		auto lua_environment::push_impl(lua_State *lua_vm, const ord_map<TKey, TElement, TOperation> &value) -> void {
			lua_newtable(lua_vm);
			int top = lua_gettop(lua_vm);
			for (const auto &kvp : value) {
				push<TKey>(lua_vm, kvp.first);
				push<TElement>(lua_vm, kvp.second);
				lua_settable(lua_vm, top);
			}
		}
		// End push_impl

		// Begin get_impl key
		template <typename T>
		auto lua_environment::get_impl_default(lua_State *lua_vm, const string &key) -> T {
			key_must_exist(key);
			lua_getglobal(lua_vm, key.c_str());
			T val = get<T>(lua_vm, -1);
			lua_pop(lua_vm, 1);
			return val;
		}

		template <typename T>
		auto lua_environment::get_impl(lua_State *lua_vm, const string &key, T *) -> T {
			lua_serialize<T> x;
			return x.read(*this, key);
		}

		template <>
		auto lua_environment::get_impl<bool>(lua_State *lua_vm, const string &key, bool *) -> bool {
			return get_impl_default<bool>(lua_vm, key);
		}

		template <>
		auto lua_environment::get_impl<double>(lua_State *lua_vm, const string &key, double *) -> double {
			return get_impl_default<double>(lua_vm, key);
		}

		template <>
		auto lua_environment::get_impl<string>(lua_State *lua_vm, const string &key, string *) -> string {
			return get_impl_default<string>(lua_vm, key);
		}

		template <>
		auto lua_environment::get_impl<int8_t>(lua_State *lua_vm, const string &key, int8_t *) -> int8_t {
			return get_impl_default<int8_t>(lua_vm, key);
		}

		template <>
		auto lua_environment::get_impl<int16_t>(lua_State *lua_vm, const string &key, int16_t *) -> int16_t {
			return get_impl_default<int16_t>(lua_vm, key);
		}

		template <>
		auto lua_environment::get_impl<int32_t>(lua_State *lua_vm, const string &key, int32_t *) -> int32_t {
			return get_impl_default<int32_t>(lua_vm, key);
		}

		template <>
		auto lua_environment::get_impl<uint8_t>(lua_State *lua_vm, const string &key, uint8_t *) -> uint8_t {
			return get_impl_default<uint8_t>(lua_vm, key);
		}

		template <>
		auto lua_environment::get_impl<uint16_t>(lua_State *lua_vm, const string &key, uint16_t *) -> uint16_t {
			return get_impl_default<uint16_t>(lua_vm, key);
		}

		template <>
		auto lua_environment::get_impl<uint32_t>(lua_State *lua_vm, const string &key, uint32_t *) -> uint32_t {
			return get_impl_default<uint32_t>(lua_vm, key);
		}

		template <>
		auto lua_environment::get_impl<milliseconds>(lua_State *lua_vm, const string &key, milliseconds *) -> milliseconds {
			return milliseconds{get_impl_default<int32_t>(lua_vm, key)};
		}

		template <>
		auto lua_environment::get_impl<seconds>(lua_State *lua_vm, const string &key, seconds *) -> seconds {
			return seconds{get_impl_default<int32_t>(lua_vm, key)};
		}

		template <>
		auto lua_environment::get_impl<minutes>(lua_State *lua_vm, const string &key, minutes *) -> minutes {
			return minutes{get_impl_default<int32_t>(lua_vm, key)};
		}

		template <>
		auto lua_environment::get_impl<hours>(lua_State *lua_vm, const string &key, hours *) -> hours {
			return hours{get_impl_default<int32_t>(lua_vm, key)};
		}

		template <>
		auto lua_environment::get_impl<lua_variant>(lua_State *lua_vm, const string &key, lua_variant *) -> lua_variant {
			if (is(lua_vm, key, lua_type::number)) return lua_variant{get<double>(lua_vm, key)};
			if (is(lua_vm, key, lua_type::string)) return lua_variant{get<string>(lua_vm, key)};
			if (is(lua_vm, key, lua_type::boolean)) return lua_variant{get<bool>(lua_vm, key)};
			if (is(lua_vm, key, lua_type::nil)) return lua_variant{};
			if (is(lua_vm, key, lua_type::table)) return lua_variant{get<hash_map<lua_variant, lua_variant>>(lua_vm, key)};
			THROW_CODE_EXCEPTION(not_implemented_exception, "lua_type");
		}

		template <typename TElement>
		auto lua_environment::get_impl(lua_State *lua_vm, const string &key, vector<TElement> *) -> vector<TElement> {
			key_must_exist(key);
			lua_getglobal(lua_vm, key.c_str());
			vector<TElement> val = get<vector<TElement>>(lua_vm, -1);
			lua_pop(lua_vm, 1);
			return val;
		}

		template <typename TKey, typename TElement, typename THash, typename TOperation>
		auto lua_environment::get_impl(lua_State *lua_vm, const string &key, hash_map<TKey, TElement, THash, TOperation> *) -> hash_map<TKey, TElement, THash, TOperation> {
			key_must_exist(key);
			lua_getglobal(lua_vm, key.c_str());
			hash_map<TKey, TElement, THash, TOperation> val = get<hash_map<TKey, TElement, THash, TOperation>>(lua_vm, -1);
			lua_pop(lua_vm, 1);
			return val;
		}

		template <typename TKey, typename TElement, typename TOperation>
		auto lua_environment::get_impl(lua_State *lua_vm, const string &key, ord_map<TKey, TElement, TOperation> *) -> ord_map<TKey, TElement, TOperation> {
			key_must_exist(key);
			lua_getglobal(lua_vm, key.c_str());
			ord_map<TKey, TElement, TOperation> val = get<ord_map<TKey, TElement, TOperation>>(lua_vm, -1);
			lua_pop(lua_vm, 1);
			return val;
		}
		// End get_impl key

		// Begin get_impl index
		template <typename TInteger>
		auto lua_environment::getInteger(lua_State *lua_vm, int index) -> TInteger {
			TInteger val = static_cast<TInteger>(lua_tointeger(lua_vm, index));
			return val;
		}

		template <typename T>
		auto lua_environment::get_impl(lua_State *lua_vm, int index, T *) -> T {
			lua_serialize<T> x;
			return x.read(*this, index);
		}

		template <>
		auto lua_environment::get_impl<bool>(lua_State *lua_vm, int index, bool *) -> bool {
			bool val = lua_toboolean(lua_vm, index) != 0;
			return val;
		}

		template <>
		auto lua_environment::get_impl<double>(lua_State *lua_vm, int index, double *) -> double {
			double val = lua_tonumber(lua_vm, index);
			return val;
		}

		template <>
		auto lua_environment::get_impl<string>(lua_State *lua_vm, int index, string *) -> string {
			string val = lua_tostring(lua_vm, index);
			return val;
		}

		template <>
		auto lua_environment::get_impl<int8_t>(lua_State *lua_vm, int index, int8_t *) -> int8_t {
			return getInteger<int8_t>(lua_vm, index);
		}

		template <>
		auto lua_environment::get_impl<int16_t>(lua_State *lua_vm, int index, int16_t *) -> int16_t {
			return getInteger<int16_t>(lua_vm, index);
		}

		template <>
		auto lua_environment::get_impl<int32_t>(lua_State *lua_vm, int index, int32_t *) -> int32_t {
			return getInteger<int32_t>(lua_vm, index);
		}

		template <>
		auto lua_environment::get_impl<uint8_t>(lua_State *lua_vm, int index, uint8_t *) -> uint8_t {
			return getInteger<uint8_t>(lua_vm, index);
		}

		template <>
		auto lua_environment::get_impl<uint16_t>(lua_State *lua_vm, int index, uint16_t *) -> uint16_t {
			return getInteger<uint16_t>(lua_vm, index);
		}

		template <>
		auto lua_environment::get_impl<uint32_t>(lua_State *lua_vm, int index, uint32_t *) -> uint32_t {
			return getInteger<uint32_t>(lua_vm, index);
		}

		template <>
		auto lua_environment::get_impl<milliseconds>(lua_State *lua_vm, int index, milliseconds *) -> milliseconds {
			return milliseconds{getInteger<int32_t>(lua_vm, index)};
		}

		template <>
		auto lua_environment::get_impl<seconds>(lua_State *lua_vm, int index, seconds *) -> seconds {
			return seconds{getInteger<int32_t>(lua_vm, index)};
		}

		template <>
		auto lua_environment::get_impl<minutes>(lua_State *lua_vm, int index, minutes *) -> minutes {
			return minutes{getInteger<int32_t>(lua_vm, index)};
		}

		template <>
		auto lua_environment::get_impl<hours>(lua_State *lua_vm, int index, hours *) -> hours {
			return hours{getInteger<int32_t>(lua_vm, index)};
		}

		template <>
		auto lua_environment::get_impl<lua_variant>(lua_State *lua_vm, int index, lua_variant *) -> lua_variant {
			if (is(lua_vm, index, lua_type::number)) return lua_variant{get<double>(lua_vm, index)};
			if (is(lua_vm, index, lua_type::string)) return lua_variant{get<string>(lua_vm, index)};
			if (is(lua_vm, index, lua_type::boolean)) return lua_variant{get<bool>(lua_vm, index)};
			if (is(lua_vm, index, lua_type::nil)) return lua_variant{};
			if (is(lua_vm, index, lua_type::table)) return lua_variant{get<hash_map<lua_variant, lua_variant>>(lua_vm, index)};
			THROW_CODE_EXCEPTION(not_implemented_exception, "lua_type");
		}

		template <typename TElement>
		auto lua_environment::get_impl(lua_State *lua_vm, int index, vector<TElement> *) -> vector<TElement> {
			vector<TElement> val;
			// Adjust index for the name
			lua_pushvalue(lua_vm, index);

			push_nil(lua_vm);
			while (lua_next(lua_vm, -2)) {
				// We don't have to account for string conversions here, it's only important for keys

				val.push_back(get<TElement>(lua_vm, -1));

				lua_pop(lua_vm, 1);
			}

			lua_pop(lua_vm, 1);

			return val;
		}

		template <typename TKey, typename TElement, typename THash, typename TOperation>
		auto lua_environment::get_impl(lua_State *lua_vm, int index, hash_map<TKey, TElement, THash, TOperation> *) -> hash_map<TKey, TElement, THash, TOperation> {
			hash_map<TKey, TElement, THash, TOperation> val;
			// Adjust index for the name
			lua_pushvalue(lua_vm, index);

			push_nil(lua_vm);
			while (lua_next(lua_vm, -2)) {
				// We have to account for string conversions here, but it's only important for keys
				lua_pushvalue(lua_vm, -2);

				auto key = get<TKey>(lua_vm, -1);
				auto value = get<TElement>(lua_vm, -2);
				val[key] = value;

				lua_pop(lua_vm, 2);
			}

			lua_pop(lua_vm, 1);

			return val;
		}

		template <typename TKey, typename TElement, typename TOperation>
		auto lua_environment::get_impl(lua_State *lua_vm, int index, ord_map<TKey, TElement, TOperation> *) -> ord_map<TKey, TElement, TOperation> {
			ord_map<TKey, TElement, TOperation> val;
			// Adjust index for the name
			lua_pushvalue(lua_vm, index);

			push_nil(lua_vm);
			while (lua_next(lua_vm, -2)) {
				// We have to account for string conversions here, but it's only important for keys
				lua_pushvalue(lua_vm, -2);

				auto key = get<TKey>(lua_vm, -1);
				auto value = get<TElement>(lua_vm, -2);
				val[key] = value;

				lua_pop(lua_vm, 2);
			}

			lua_pop(lua_vm, 1);

			return val;
		}
		// End get_impl index
	}
}