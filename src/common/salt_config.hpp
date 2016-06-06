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
#include "common/password_transformation_config.hpp"
#include "common/salt_policy.hpp"
#include "common/salt_transformation_config.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	class lua::lua_variant;

	struct salt_config {
		password_transformation_config policy = {salt_policy::prepend, vector<lua::lua_variant>{}};
		vector<salt_transformation_config> modify_policies;
	};

	template <>
	struct lua::lua_serialize<salt_config> {
		auto read(lua_environment &config, const string &prefix) -> salt_config {
			salt_config ret;

			lua_variant obj = config.get<lua_variant>(prefix);
			config.validate_object(lua_type::table, obj, prefix);

			auto map = obj.as<hash_map<lua_variant, lua_variant>>();
			bool has_salt_policy = false;
			bool has_salt_modify_policy = false;
			lua_variant salt_policy;
			lua_variant salt_modify_policy;
			for (const auto &kvp : map) {
				config.validate_key(lua_type::string, kvp.first, prefix);

				string key = kvp.first.as<string>();
				if (key == "salt") {
					has_salt_policy = true;
					config.validate_value(lua_type::table, kvp.second, key, prefix);
					salt_policy = kvp.second;
				}
				else if (key == "salt_modify") {
					has_salt_modify_policy = true;
					config.validate_value(lua_type::table, kvp.second, key, prefix);
					salt_modify_policy = kvp.second;
				}
			}

			config.required(has_salt_policy, "salt", prefix);
			config.required(has_salt_modify_policy, "salt_modify", prefix);

			ret.policy = salt_policy.into<password_transformation_config>(config, prefix + ".salt");
		
			auto modifiers = salt_modify_policy.as<vector<lua_variant>>();
			for (const auto &modifier : modifiers) {
				ret.modify_policies.push_back(modifier.into<salt_transformation_config>(config, prefix + ".salt_modify"));
			}
			return ret;
		}
	};
}