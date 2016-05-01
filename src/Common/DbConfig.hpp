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

#include "Common/ConfigFile.hpp"
#include "Common/Types.hpp"
#include <string>

namespace Vana {
	struct DbConfig {
		port_t port = 0;
		string_t database;
		string_t tablePrefix;
		string_t host;
		string_t username;
		string_t password;
	};

	template <>
	struct LuaSerialize<DbConfig> {
		auto read(LuaEnvironment &config, const string_t &prefix) -> DbConfig {
			DbConfig ret;

			LuaVariant obj = config.get<LuaVariant>(prefix);
			config.validateObject(LuaType::Table, obj, prefix);

			auto map = obj.as<hash_map_t<LuaVariant, LuaVariant>>();
			bool hasDatabase = false;
			bool hasHost = false;
			bool hasPort = false;
			bool hasUsername = false;
			for (const auto &kvp : map) {
				config.validateKey(LuaType::String, kvp.first, prefix);

				string_t key = kvp.first.as<string_t>();
				if (key == "database") {
					hasDatabase = true;
					config.validateValue(LuaType::String, kvp.second, key, prefix);
					ret.database = kvp.second.as<string_t>();
				}
				else if (key == "host") {
					hasHost = true;
					config.validateValue(LuaType::String, kvp.second, key, prefix);
					ret.host = kvp.second.as<string_t>();
				}
				else if (key == "username") {
					hasUsername = true;
					config.validateValue(LuaType::String, kvp.second, key, prefix);
					ret.username = kvp.second.as<string_t>();
				}
				else if (key == "port") {
					hasPort = true;
					config.validateValue(LuaType::Number, kvp.second, key, prefix);
					ret.port = kvp.second.as<port_t>();
				}
				else if (key == "password") {
					if (config.validateValue(LuaType::String, kvp.second, key, prefix, true) == LuaType::Nil) continue;
					ret.password = kvp.second.as<string_t>();
				}
				else if (key == "table_prefix") {
					if (config.validateValue(LuaType::String, kvp.second, key, prefix, true) == LuaType::Nil) continue;
					ret.tablePrefix = kvp.second.as<string_t>();
				}
			}

			config.required(hasDatabase, "database", prefix);
			config.required(hasHost, "host", prefix);
			config.required(hasUsername, "username", prefix);
			config.required(hasPort, "port", prefix);

			return ret;
		}
	};
}