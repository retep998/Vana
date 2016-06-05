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
#include "common/lua_variant.hpp"
#include "common/password_transformation_config.hpp"
#include "common/salt_size_policy.hpp"
#include "common/salt_transformation_config.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	struct salt_size_config {
		salt_size_policy policy = salt_size_policy::fixed;
		int32_t size = 10;
		int32_t min = -1;
		int32_t max = -1;
	};

	template <>
	struct lua_variant_into<salt_size_config> {
		auto transform(lua_environment &config, const lua_variant &src, const string &prefix) -> salt_size_config {
			salt_size_config ret;

			if (!src.is_any_of({ lua::lua_type::number, lua::lua_type::table })) {
				config.error(prefix + " must be a valid Lua number or a table");
			}

			if (src.is(lua::lua_type::number)) {
				int32_t size = src.as<int32_t>();
				if (size < 0) {
					config.error(prefix + " must be a non-negative number");
				}
				if (size == 0) {
					ret.policy = salt_size_policy::random;
					ret.min = 10;
					ret.max = 30;
					ret.size = -1;
				}
				else {
					ret.size = size;
				}
			}
			else {
				ret.policy = salt_size_policy::random;
				auto values = src.as<hash_map<lua_variant, lua_variant>>();
				bool has_min = false;
				bool has_max = false;
				for (const auto &kvp : values) {
					config.validate_key(lua::lua_type::string, kvp.first, prefix);

					string key = kvp.first.as<string>();
					if (key == "min") {
						has_min = true;
						config.validate_value(lua::lua_type::number, kvp.second, key, prefix);
						ret.min = kvp.second.as<int32_t>();
					}
					else if (key == "max") {
						has_max = true;
						config.validate_value(lua::lua_type::number, kvp.second, key, prefix);
						ret.max = kvp.second.as<int32_t>();
					}
				}

				config.required(has_min, "min", prefix);
				config.required(has_max, "max", prefix);
				if (ret.min <= 0) {
					config.error(prefix + ".min must be a positive number");
				}
				if (ret.max <= 0) {
					config.error(prefix + ".max must be a positive number");
				}
				if (ret.min > ret.max) {
					config.error(prefix + ".min must be lower than " + prefix + ".max");
				}
				ret.size = -1;
			}

			return ret;
		}
	};
}