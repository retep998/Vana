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

#include "common/lua/config_file.hpp"
#include "common/types.hpp"
#include <string>

namespace vana {
	namespace config {
		struct log {
			bool perform = false;
			int32_t destination = 0;
			uint32_t buffer_size = 20;
			string format;
			string time_format;
			string file;
		};
	}

	template <>
	struct lua::lua_serialize<config::log> {
		auto read(lua_environment &config, const string &prefix) -> config::log {
			config::log ret;

			lua_variant obj = config.get<lua_variant>(prefix);
			config.validate_object(lua_type::table, obj, prefix);

			auto map = obj.as<hash_map<lua_variant, lua_variant>>();
			bool has_log = false;
			bool has_destination = false;
			bool has_format = false;
			for (const auto &kvp : map) {
				config.validate_key(lua_type::string, kvp.first, prefix);

				string key = kvp.first.as<string>();
				if (key == "log") {
					has_log = true;
					config.validate_value(lua_type::boolean, kvp.second, key, prefix);
					ret.perform = kvp.second.as<bool>();
					if (!ret.perform) break;
				}
				else if (key == "destination") {
					has_destination = true;
					config.validate_value(lua_type::number, kvp.second, key, prefix);
					ret.destination = kvp.second.as<int32_t>();
				}
				else if (key == "format") {
					has_format = true;
					config.validate_value(lua_type::string, kvp.second, key, prefix);
					ret.format = kvp.second.as<string>();
				}
				else if (key == "buffer_size") {
					if (config.validate_value(lua_type::number, kvp.second, key, prefix, true) == lua_type::nil) continue;
					ret.buffer_size = kvp.second.as<uint32_t>();
				}
				else if (key == "file") {
					if (config.validate_value(lua_type::string, kvp.second, key, prefix, true) == lua_type::nil) continue;
					ret.file = kvp.second.as<string>();
				}
				else if (key == "time_format") {
					if (config.validate_value(lua_type::string, kvp.second, key, prefix, true) == lua_type::nil) continue;
					ret.time_format = kvp.second.as<string>();
				}
			}

			config.required(has_log, "log", prefix);
			if (ret.perform) {
				config.required(has_destination, "destination", prefix);
				config.required(has_format, "format", prefix);
			}

			if (ret.time_format.empty()) {
				ret.time_format = config.get<string>("log_time_format");
			}

			return ret;
		}
	};
}