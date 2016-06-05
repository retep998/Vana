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

#include "common/config_file.hpp"
#include "common/types_temp.hpp"
#include <string>

namespace vana {
	struct db_config {
		connection_port port = 0;
		string database;
		string table_prefix;
		string host;
		string username;
		string password;
	};

	template <>
	struct lua_serialize<db_config> {
		auto read(lua_environment &config, const string &prefix) -> db_config {
			db_config ret;

			lua_variant obj = config.get<lua_variant>(prefix);
			config.validate_object(lua::lua_type::table, obj, prefix);

			auto map = obj.as<hash_map<lua_variant, lua_variant>>();
			bool has_db = false;
			bool has_host = false;
			bool has_port = false;
			bool has_user = false;
			for (const auto &kvp : map) {
				config.validate_key(lua::lua_type::string, kvp.first, prefix);

				string key = kvp.first.as<string>();
				if (key == "database") {
					has_db = true;
					config.validate_value(lua::lua_type::string, kvp.second, key, prefix);
					ret.database = kvp.second.as<string>();
				}
				else if (key == "host") {
					has_host = true;
					config.validate_value(lua::lua_type::string, kvp.second, key, prefix);
					ret.host = kvp.second.as<string>();
				}
				else if (key == "username") {
					has_user = true;
					config.validate_value(lua::lua_type::string, kvp.second, key, prefix);
					ret.username = kvp.second.as<string>();
				}
				else if (key == "port") {
					has_port = true;
					config.validate_value(lua::lua_type::number, kvp.second, key, prefix);
					ret.port = kvp.second.as<connection_port>();
				}
				else if (key == "password") {
					if (config.validate_value(lua::lua_type::string, kvp.second, key, prefix, true) == lua::lua_type::nil) continue;
					ret.password = kvp.second.as<string>();
				}
				else if (key == "table_prefix") {
					if (config.validate_value(lua::lua_type::string, kvp.second, key, prefix, true) == lua::lua_type::nil) continue;
					ret.table_prefix = kvp.second.as<string>();
				}
			}

			config.required(has_db, "database", prefix);
			config.required(has_host, "host", prefix);
			config.required(has_user, "username", prefix);
			config.required(has_port, "port", prefix);

			return ret;
		}
	};
}