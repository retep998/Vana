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
#include "common/lua/lua_variant.hpp"
#include "common/salt_modify_policy.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	class lua::lua_variant;

	class salt_transformation_config {
	public:
		salt_transformation_config(salt_modify_policy policy, vector<lua::lua_variant> args);

		auto static validate_args(salt_modify_policy policy, const vector<lua::lua_variant> &args) -> validity_result;
		auto apply(string input) const -> string;
	private:
		salt_transformation_config() = default;
		friend struct lua::lua_variant_into<salt_transformation_config>;

		salt_modify_policy m_policy = salt_modify_policy::none;
		vector<lua::lua_variant> m_args;
	};

	template <>
	struct lua::lua_variant_into<salt_transformation_config> {
		auto transform(lua_environment &config, const lua_variant &src, const string &prefix) -> salt_transformation_config {
			salt_transformation_config ret;

			config.validate_object(lua_type::table, src, prefix);

			auto map = src.as<hash_map<lua_variant, lua_variant>>();
			bool has_type = false;
			bool has_args = false;
			salt_modify_policy salt_policy_data;
			lua_variant args;
			for (const auto &kvp : map) {
				config.validate_key(lua_type::string, kvp.first, prefix);

				string key = kvp.first.as<string>();
				if (key == "type") {
					has_type = true;
					config.validate_value(lua_type::number, kvp.second, key, prefix);
					salt_policy_data = static_cast<salt_modify_policy>(kvp.second.as<int32_t>());
				}
				else if (key == "args") {
					has_args = true;
					config.validate_value(lua_type::table, kvp.second, key, prefix);
					args = kvp.second;
				}
			}

			config.required(has_type, "type", prefix);
			config.required(has_args, "args", prefix);

			ret.m_policy = salt_policy_data;
			ret.m_args = args.as<vector<lua_variant>>();

			if (salt_transformation_config::validate_args(ret.m_policy, ret.m_args) == validity_result::invalid) {
				config.error("Arguments for salt transformation " + prefix + " are invalid. Please consult the documentation.");
			}

			return ret;
		}
	};
}