/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "ConfigFile.hpp"
#include "ExitCodes.hpp"
#include "FileUtilities.hpp"
#include "Logger.hpp"
#include "SaltLeftoverPolicy.hpp"
#include "SaltModifyPolicy.hpp"
#include "SaltPolicy.hpp"
#include <iostream>

ConfigFile::ConfigFile(const string_t &filename) :
	LuaEnvironment{filename}
{
}

ConfigFile::~ConfigFile() {
	// Intentionally blank
}

auto ConfigFile::handleFileNotFound(const string_t &filename) -> void {
	std::cerr << "ERROR: Configuration file " << filename << " does not exist!" << std::endl;
	ExitCodes::exit(ExitCodes::ConfigFileMissing);
	throw ConfigException{};
}

auto ConfigFile::handleKeyNotFound(const string_t &filename, const string_t &key) -> void {
	std::cerr << "ERROR: Couldn't get a value from config file." << std::endl;
	std::cerr << "File: " << filename << std::endl;
	std::cerr << "Value: " << key << std::endl;
	ExitCodes::exit(ExitCodes::ConfigError);
	throw ConfigException{};
}

auto ConfigFile::handleError(const string_t &filename, const string_t &error) -> void {
	std::cerr << "ERROR: " << error << std::endl;
	std::cerr << "File: " << filename << std::endl;
	ExitCodes::exit(ExitCodes::ConfigError);
	throw ConfigException{};
}

auto ConfigFile::getSaltingConfig() -> owned_ptr_t<ConfigFile> {
	auto env = make_owned_ptr<ConfigFile>("conf/salting.lua");
	auto cast = [&](string_t key, auto value) {
		env->set<int32_t>(key, static_cast<int32_t>(value));
	};

	// SaltPolicy
	cast("system_salt_append", SaltPolicy::Append);
	cast("system_salt_prepend", SaltPolicy::Prepend);
	cast("system_salt_intersperse", SaltPolicy::Intersperse);
	cast("system_salt_chopdistribute", SaltPolicy::ChopDistribute);
	cast("system_salt_chopcrossdistribute", SaltPolicy::ChopCrossDistribute);
	// SaltLeftoverPolicy
	cast("system_salt_leftover_append", SaltLeftoverPolicy::Append);
	cast("system_salt_leftover_prepend", SaltLeftoverPolicy::Prepend);
	cast("system_salt_leftover_discard", SaltLeftoverPolicy::Discard);
	cast("system_salt_leftover_rollover", SaltLeftoverPolicy::Rollover);
	// SaltModifyPolicy
	cast("system_salt_modify_xor_byte", SaltModifyPolicy::XorByteCipher);
	cast("system_salt_modify_xor", SaltModifyPolicy::XorCipher);
	cast("system_salt_modify_bit_rotate", SaltModifyPolicy::BitRotateCipher);
	cast("system_salt_modify_overall_bit_rotate", SaltModifyPolicy::OverallBitRotateCipher);
	cast("system_salt_modify_rotate", SaltModifyPolicy::ByteRotateCipher);
	cast("system_salt_modify_bit_reverse", SaltModifyPolicy::BitReverseCipher);
	cast("system_salt_modify_reverse", SaltModifyPolicy::ReverseCipher);
	cast("system_salt_modify_overwrite", SaltModifyPolicy::OverwriteCipher);
	cast("system_salt_modify_add", SaltModifyPolicy::AddCipher);
	cast("system_salt_modify_subtract", SaltModifyPolicy::SubtractCipher);
	cast("system_salt_modify_multiply", SaltModifyPolicy::MultiplyCipher);
	cast("system_salt_modify_divide", SaltModifyPolicy::DivideCipher);
	cast("system_salt_modify_append", SaltModifyPolicy::Append);
	cast("system_salt_modify_prepend", SaltModifyPolicy::Prepend);
	cast("system_salt_modify_intersperse", SaltModifyPolicy::Intersperse);

	return env;
}

auto ConfigFile::getWorldsConfig() -> owned_ptr_t<ConfigFile> {
	auto env = make_owned_ptr<ConfigFile>("conf/worlds.lua");
	env->set<int8_t>("system_ribbon_none", 0);
	env->set<int8_t>("system_ribbon_event", 1);
	env->set<int8_t>("system_ribbon_new", 2);
	env->set<int8_t>("system_ribbon_hot", 3);
	return env;
}

auto ConfigFile::getLoginServerConfig() -> owned_ptr_t<ConfigFile> {
	auto env = make_owned_ptr<ConfigFile>("conf/loginserver.lua");
	return env;
}

auto ConfigFile::getLoggerConfig() -> owned_ptr_t<ConfigFile> {
	auto env = make_owned_ptr<ConfigFile>("conf/logger.lua");

	hash_map_t<string_t, int32_t> constants;
	constants["console"] = LogDestinations::Console;
	constants["file"] = LogDestinations::File;
	constants["sql"] = LogDestinations::Sql;

	function_t<void(string_t, int32_t, uint32_t)> options = [&](string_t base, int32_t val, uint32_t depth) {
		int32_t originalVal = val;
		for (const auto &kvp : constants) {
			if (base.find(kvp.first) != string_t::npos) continue;

			string_t newBase = base + "_" + kvp.first;
			val |= kvp.second;
			env->set<int32_t>(newBase, val);

			if (depth < constants.size()) {
				options(newBase, val, depth + 1);
			}
			val = originalVal;
		}
	};

	env->set<int32_t>("system_log_none", LogDestinations::None);
	env->set<int32_t>("system_log_all", LogDestinations::All);

	options("system_log", 0, 0);

	return env;
}

auto ConfigFile::getDatabaseConfig() -> owned_ptr_t<ConfigFile> {
	auto env = make_owned_ptr<ConfigFile>("conf/database.lua");
	return env;
}

auto ConfigFile::getConnectionPropertiesConfig() -> owned_ptr_t<ConfigFile> {
	auto env = make_owned_ptr<ConfigFile>("conf/connection_properties.lua");
	return env;
}
