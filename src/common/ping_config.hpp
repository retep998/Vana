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

namespace vana {
	struct ping_config {
		bool enable = true;
		milliseconds initial_delay = milliseconds{60000};
		milliseconds interval = milliseconds{30000};
		int32_t timeout_ping_count = 4;
	};

	template <>
	struct lua::lua_serialize<ping_config> {
		auto read(lua::lua_environment &config, const string &prefix) -> ping_config {
			ping_config ret;

			lua_variant obj = config.get<lua_variant>(prefix);
			config.validate_object(lua_type::table, obj, prefix);

			auto map = obj.as<hash_map<lua_variant, lua_variant>>();
			bool has_enabled = false;
			for (const auto &kvp : map) {
				config.validate_key(lua_type::string, kvp.first, prefix);

				string key = kvp.first.as<string>();
				if (key == "enabled") {
					has_enabled = true;
					config.validate_value(lua_type::boolean, kvp.second, key, prefix);
					ret.enable = kvp.second.as<bool>();
				}
				else if (key == "initial_delay") {
					if (config.validate_value(lua_type::number, kvp.second, key, prefix, true) == lua_type::nil) continue;
					ret.initial_delay = milliseconds{kvp.second.as<int32_t>()};
				}
				else if (key == "interval") {
					if (config.validate_value(lua_type::number, kvp.second, key, prefix, true) == lua_type::nil) continue;
					ret.interval = milliseconds{kvp.second.as<int32_t>()};
				}
				else if (key == "timeout_ping_count") {
					if (config.validate_value(lua_type::number, kvp.second, key, prefix, true) == lua_type::nil) continue;
					ret.timeout_ping_count = kvp.second.as<int32_t>();
				}
			}

			config.required(has_enabled, "enabled", prefix);

			return ret;
		}
	};
}