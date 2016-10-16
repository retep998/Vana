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
#include "config_file.hpp"
#include "common/exit_code.hpp"
#include "common/log/base_logger.hpp"
#include "common/salt_leftover_policy.hpp"
#include "common/salt_modify_policy.hpp"
#include "common/salt_policy.hpp"
#include "common/util/file.hpp"
#include <iostream>

namespace vana {
namespace lua {

config_file::config_file(const string &filename) :
	lua_environment{filename}
{
}

config_file::~config_file() {
	// Intentionally blank
}

auto config_file::handle_file_not_found(const string &filename) -> void {
	std::cerr << "ERROR: Configuration file '" << filename << "' does not exist!" << std::endl;
	exit(exit_code::config_file_missing);
	throw config_exception{};
}

auto config_file::handle_key_not_found(const string &filename, const string &key) -> void {
	std::cerr << "ERROR: Couldn't get a value from config file." << std::endl;
	std::cerr << "File: " << filename << std::endl;
	std::cerr << "Value: " << key << std::endl;
	exit(exit_code::config_error);
	throw config_exception{};
}

auto config_file::handle_error(const string &filename, const string &error) -> void {
	std::cerr << "ERROR: " << error << std::endl;
	std::cerr << "File: " << filename << std::endl;
	exit(exit_code::config_error);
	throw config_exception{};
}

auto config_file::get_salting_config() -> owned_ptr<config_file> {
	auto env = make_owned_ptr<config_file>("conf/salting.lua");
	auto cast = [&](string key, auto value) {
		env->set<int32_t>(key, static_cast<int32_t>(value));
	};

	// salt_policy
	cast("system_salt_append", salt_policy::append);
	cast("system_salt_prepend", salt_policy::prepend);
	cast("system_salt_intersperse", salt_policy::intersperse);
	cast("system_salt_chopdistribute", salt_policy::chop_distribute);
	cast("system_salt_chopcrossdistribute", salt_policy::chop_cross_distribute);
	// salt_leftover_policy
	cast("system_salt_leftover_append", salt_leftover_policy::append);
	cast("system_salt_leftover_prepend", salt_leftover_policy::prepend);
	cast("system_salt_leftover_discard", salt_leftover_policy::discard);
	cast("system_salt_leftover_rollover", salt_leftover_policy::rollover);
	// salt_modify_policy
	cast("system_salt_modify_xor_byte", salt_modify_policy::xor_byte_cipher);
	cast("system_salt_modify_xor", salt_modify_policy::xor_cipher);
	cast("system_salt_modify_bit_rotate", salt_modify_policy::bit_rotate_cipher);
	cast("system_salt_modify_overall_bit_rotate", salt_modify_policy::overall_bit_rotate_cipher);
	cast("system_salt_modify_rotate", salt_modify_policy::byte_rotate_cipher);
	cast("system_salt_modify_bit_reverse", salt_modify_policy::bit_reverse_cipher);
	cast("system_salt_modify_reverse", salt_modify_policy::reverse_cipher);
	cast("system_salt_modify_overwrite", salt_modify_policy::overwrite_cipher);
	cast("system_salt_modify_add", salt_modify_policy::add_cipher);
	cast("system_salt_modify_subtract", salt_modify_policy::subtract_cipher);
	cast("system_salt_modify_multiply", salt_modify_policy::multiply_cipher);
	cast("system_salt_modify_divide", salt_modify_policy::divide_cipher);
	cast("system_salt_modify_append", salt_modify_policy::append);
	cast("system_salt_modify_prepend", salt_modify_policy::prepend);
	cast("system_salt_modify_intersperse", salt_modify_policy::intersperse);

	return env;
}

auto config_file::get_worlds_config() -> owned_ptr<config_file> {
	auto env = make_owned_ptr<config_file>("conf/worlds.lua");
	env->set<int8_t>("system_ribbon_none", 0);
	env->set<int8_t>("system_ribbon_event", 1);
	env->set<int8_t>("system_ribbon_new", 2);
	env->set<int8_t>("system_ribbon_hot", 3);
	return env;
}

auto config_file::get_login_config() -> owned_ptr<config_file> {
	auto env = make_owned_ptr<config_file>("conf/loginserver.lua");
	return env;
}

auto config_file::get_logger_config() -> owned_ptr<config_file> {
	auto env = make_owned_ptr<config_file>("conf/logger.lua");

	hash_map<string, int32_t> constants;
	constants["console"] = static_cast<int32_t>(vana::log::destination::console);
	constants["file"] = static_cast<int32_t>(vana::log::destination::file);
	constants["sql"] = static_cast<int32_t>(vana::log::destination::sql);

	function<void(string, int32_t, uint32_t)> options = [&](string base, int32_t val, uint32_t depth) {
		int32_t original_val = val;
		for (const auto &kvp : constants) {
			if (base.find(kvp.first) != string::npos) continue;

			string new_base = base + "_" + kvp.first;
			val |= kvp.second;
			env->set<int32_t>(new_base, val);

			if (depth < constants.size()) {
				options(new_base, val, depth + 1);
			}
			val = original_val;
		}
	};

	env->set<int32_t>("system_log_none", static_cast<int32_t>(vana::log::destination::none));
	env->set<int32_t>("system_log_all", static_cast<int32_t>(vana::log::destination::all));

	options("system_log", 0, 0);

	return env;
}

auto config_file::get_database_config() -> owned_ptr<config_file> {
	auto env = make_owned_ptr<config_file>("conf/database.lua");
	return env;
}

auto config_file::get_connection_properties_config() -> owned_ptr<config_file> {
	auto env = make_owned_ptr<config_file>("conf/connection_properties.lua");
	return env;
}

}
}