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
#include "lua_environment.hpp"
#include "common/util/file.hpp"
#include "common/util/string.hpp"
#include <iostream>
#include <stdexcept>

namespace vana {
namespace lua {

vana::util::object_pool<int32_t, lua_environment *> lua_environment::s_environments =
	vana::util::object_pool<int32_t, lua_environment *>{1, 1000000};

auto lua_environment::get_environment(lua_State *lua_vm) -> lua_environment & {
	lua_getglobal(lua_vm, "system_environment_id");
	int32_t id = lua_tointeger(lua_vm, -1);
	lua_pop(lua_vm, 1);
	auto kvp = s_environments.find(id);
	return *kvp->second;
}

lua_environment::lua_environment(const string &filename)
{
	load_file(filename);

	m_environment_id = s_environments.store(this);
	set<int32_t>("system_environment_id", m_environment_id);
}

lua_environment::lua_environment(const string &filename, bool use_thread)
{
	load_file(filename);

	m_environment_id = s_environments.store(this);
	if (use_thread) {
		m_lua_thread = lua_newthread(m_lua_vm);
	}
	set<int32_t>("system_environment_id", m_environment_id);
}

lua_environment::~lua_environment() {
	s_environments.release(m_environment_id);
	lua_close(m_lua_vm);
	m_lua_vm = nullptr;
}

auto lua_environment::load_file(const string &filename) -> void {
	if (m_lua_vm != nullptr) {
		throw std::runtime_error{"lua_vm was still specified"};
	}
	if (!vana::util::file::exists(filename)) {
		handle_file_not_found(filename);
	}

	m_file = filename;
	m_lua_vm = luaL_newstate();

	require_standard_lib("base", luaopen_base);
	require_standard_lib("string", luaopen_string);
	require_standard_lib("math", luaopen_math);
	require_standard_lib("table", luaopen_table);
	set<int32_t>("system_lua_version_major", 5);
	set<int32_t>("system_lua_version_minor", 2);
	set<string>("system_lua_version", "5.2");
}

auto lua_environment::run() -> result {
	if (m_lua_thread == nullptr) {
		if (luaL_dofile(m_lua_vm, m_file.c_str())) {
			handle_error(m_file, get<string>(m_lua_vm, -1));
			pop();
			return result::failure;
		}
	}
	else {
		if (luaL_loadfile(m_lua_thread, m_file.c_str())) {
			handle_error(m_file, get<string>(m_lua_thread, -1));
			pop();
			return result::failure;
		}
		return resume(0);
	}
	return result::success;
}

auto lua_environment::resume(lua::lua_return pushed_arg_count) -> result {
	int ret = lua_resume(m_lua_thread, m_lua_vm, pushed_arg_count);
	if (ret == 0) {
		handle_thread_completion();
	}
	else if (ret != LUA_YIELD) {
		// Error, a working script returns either 0 or LUA_YIELD
		string error = lua_tostring(m_lua_thread, -1);
		handle_error(m_file, error);
		return result::failure;
	}
	return result::success;
}

auto lua_environment::handle_error(const string &filename, const string &error) -> void {
	print_error(error);
}

auto lua_environment::handle_file_not_found(const string &filename) -> void {
	// Intentionally blank
}

auto lua_environment::handle_thread_completion() -> void {
	// Intentionally blank
}

auto lua_environment::handle_key_not_found(const string &filename, const string &key) -> void {
	throw std::runtime_error{"Key '" + key + "' does not exist and is required in file '" + filename + "'"};
}

auto lua_environment::print_error(const string &error) const -> void {
	std::cerr << error << std::endl;
}

auto lua_environment::expose(const string &name, lua::lua_function func) -> void {
	lua_register(m_lua_vm, name.c_str(), func);
}

auto lua_environment::require_standard_lib(const string &local_name, lua::lua_function func) -> void {
	luaL_requiref(m_lua_vm, local_name.c_str(), func, 1);
}

auto lua_environment::yield(lua::lua_return quantity_return_results_passed_to_resume) -> lua::lua_return {
	return lua_yield(m_lua_thread, quantity_return_results_passed_to_resume);
}

auto lua_environment::push_nil() -> lua_environment & {
	return push_nil(m_lua_vm);
}

auto lua_environment::push_nil(lua_State *lua_vm) -> lua_environment & {
	lua_pushnil(lua_vm);
	return *this;
}

auto lua_environment::get_script_name() -> string {
	vector<string> parts = vana::util::str::split(m_file, "/");
	return vana::util::file::remove_extension(parts[parts.size() - 1]);
}

auto lua_environment::get_script_path() -> vector<string> {
	vector<string> parts = vana::util::str::split(m_file, "/");
	if (parts.size() == 1) {
		vector<string> ret;
		return ret;
	}
	vector<string> ret{parts.begin(), parts.end() - 1};
	return ret;
}

auto lua_environment::exists(const string &key) -> bool {
	return exists(m_lua_vm, key);
}

auto lua_environment::exists(lua_State *lua_vm, const string &key) -> bool {
	lua_getglobal(lua_vm, key.c_str());
	bool ret = ::lua_type(lua_vm, -1) != LUA_TNIL;
	lua_pop(lua_vm, 1);
	return ret;
}

auto lua_environment::key_must_exist(const string &key) -> void {
	if (!exists(key)) {
		handle_key_not_found(m_file, key);
	}
}

auto lua_environment::error(const string &text) -> void {
	handle_error(m_file, text);
}

auto lua_environment::is(const string &key, lua_type type) -> bool {
	return is(m_lua_vm, key, type);
}

auto lua_environment::is_any_of(const string &key, init_list<lua_type> types) -> bool {
	return std::any_of(std::begin(types), std::end(types), [&](lua_type type) -> bool {
		return is(key, type);
	});
}

auto lua_environment::is(lua_State *lua_vm, const string &key, lua_type type) -> bool {
	lua_getglobal(lua_vm, key.c_str());
	bool ret = is(lua_vm, -1, type);
	lua_pop(lua_vm, 1);
	return ret;
}

auto lua_environment::is_any_of(lua_State *lua_vm, const string &key, init_list<lua_type> types) -> bool {
	return std::any_of(std::begin(types), std::end(types), [&](lua_type type) -> bool {
		return is(lua_vm, key, type);
	});
}

auto lua_environment::pop(int count) -> void {
	pop(m_lua_vm, count);
}

auto lua_environment::pop(lua_State *lua_vm, int count) -> void {
	lua_pop(lua_vm, count);
}

auto lua_environment::count() -> int {
	return count(m_lua_vm);
}

auto lua_environment::count(lua_State *lua_vm) -> int {
	return lua_gettop(lua_vm);
}

auto lua_environment::validate_value(lua_type expected_type, const lua_variant &v, const string &key, const string &prefix, bool nil_is_valid) -> lua_type {
	lua_type type = v.get_type();
	if (nil_is_valid && type == lua_type::nil) return type;
	if (type != expected_type) {
		string representation;
		switch (type) {
			case lua_type::boolean: representation = "bool"; break;
			case lua_type::number: representation = "number"; break;
			case lua_type::string: representation = "string"; break;
			case lua_type::table: representation = "table"; break;
			default: THROW_CODE_EXCEPTION(not_implemented_exception, "LuaType");
		}
		error("Key '" + key + "' on object " + prefix + " must have a " + representation + " value");
	}
	return type;
}

auto lua_environment::validate_key(lua_type expected_type, const lua_variant &v, const string &prefix) -> void {
	lua_type type = v.get_type();
	if (type != expected_type) {
		string representation;
		switch (type) {
			case lua_type::boolean: representation = "bool"; break;
			case lua_type::number: representation = "number"; break;
			case lua_type::string: representation = "string"; break;
			case lua_type::table: representation = "table"; break;
			default: THROW_CODE_EXCEPTION(not_implemented_exception, "LuaType");
		}
		error("Object " + prefix + " keys must be " + representation + "s");
	}
}

auto lua_environment::validate_object(lua_type expected_type, const lua_variant &v, const string &prefix) -> void {
	lua_type found_type = v.get_type();
	if (found_type != expected_type) {
		string expectedRepresentation;
		switch (expected_type) {
			case lua_type::nil: expectedRepresentation = "nil"; break;
			case lua_type::boolean: expectedRepresentation = "bool"; break;
			case lua_type::number: expectedRepresentation = "number"; break;
			case lua_type::string: expectedRepresentation = "string"; break;
			case lua_type::table: expectedRepresentation = "table"; break;
			default: THROW_CODE_EXCEPTION(not_implemented_exception, "LuaType");
		}
		string foundRepresentation;
		switch (found_type) {
			case lua_type::nil: foundRepresentation = "nil"; break;
			case lua_type::boolean: foundRepresentation = "bool"; break;
			case lua_type::number: foundRepresentation = "number"; break;
			case lua_type::string: foundRepresentation = "string"; break;
			case lua_type::table: foundRepresentation = "table"; break;
			default: THROW_CODE_EXCEPTION(not_implemented_exception, "LuaType");
		}
		error("Object " + prefix + " must be a " + expectedRepresentation + " object, found " + foundRepresentation);
	}
}

auto lua_environment::required(bool present, const string &key, const string &prefix) -> void {
	if (!present) error("Missing required key '" + key + "' on object " + prefix);
}

auto lua_environment::is(int index, lua_type type) -> bool {
	return is(m_lua_vm, index, type);
}

auto lua_environment::is(lua_State *lua_vm, int index, lua_type type) -> bool {
	return ::lua_type(lua_vm, index) == static_cast<int>(type);
}

auto lua_environment::is_any_of(int index, init_list<lua_type> types) -> bool {
	return std::any_of(std::begin(types), std::end(types), [&](lua_type type) -> bool {
		return is(index, type);
	});
}

auto lua_environment::is_any_of(lua_State *lua_vm, int index, init_list<lua_type> types) -> bool {
	return std::any_of(std::begin(types), std::end(types), [&](lua_type type) -> bool {
		return is(lua_vm, index, type);
	});
}

auto lua_environment::type_of(int index) -> lua_type {
	return static_cast<lua_type>(::lua_type(m_lua_vm, index));
}

auto lua_environment::type_of(lua_State *lua_vm, int index) -> lua_type {
	return static_cast<lua_type>(::lua_type(lua_vm, index));
}

}
}